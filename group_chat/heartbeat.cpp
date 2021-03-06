/*********************************************
 * heartbeat.cpp
 * Author: kamuszhou@tencent.com kamuszhou@qq.com
 * website: v.qq.com  www.dogeye.net
 * Created on: Aug 3, 2014
 * Praise Be to the Lord. BUG-FREE CODE !
 ********************************************/

#include <boost/make_shared.hpp>
#include "heartbeat.h"
#include "proactor.h"
#include "zmq_ctx.h"

#define HEARTBEAT_PORT 9999
#define HEARTBEAT_INTERVAL 1000
#define PEER_LIFE_CYCLE 5000

Heartbeat g_heartbeat;

Heartbeat::Heartbeat()
{
}

Heartbeat::~Heartbeat()
{
	zmq_close(gate_);
}

void Heartbeat::init(std::string id)
{
	void* zmq_ctx = g_zmq_ctx.get_zmq_ctx();
	gate_ = zmq_socket(zmq_ctx, ZMQ_PAIR);
	if (zmq_connect(gate_, MAGIC_GATE))
	{
		zmq_strerror(errno);
	}

	myid_ = id;

	ios_ptr_ = &g_proactor.get_ios();

	udp_socket_.reset(new boost::asio::ip::udp::socket(*ios_ptr_));
	strand_.reset(new boost::asio::strand(*ios_ptr_));
	strand4peermap_.reset(new boost::asio::strand(*ios_ptr_));
	heartbeat_timer_.reset(new boost::asio::deadline_timer(*ios_ptr_, boost::posix_time::milliseconds(HEARTBEAT_INTERVAL)));

	broadcast_addr_ = boost::asio::ip::address_v4::broadcast();
	broadcast_ep_ = boost::asio::ip::udp::endpoint(broadcast_addr_, HEARTBEAT_PORT);

	udp_socket_->open(boost::asio::ip::udp::v4());
	boost::asio::ip::udp::socket::broadcast broadcast_option(true);
	boost::asio::ip::udp::socket::reuse_address reuse_option(true);
	boost::asio::ip::multicast::enable_loopback loopback_option(true);
	udp_socket_->set_option(reuse_option);
	udp_socket_->set_option(broadcast_option);
	udp_socket_->set_option(loopback_option);

	udp_socket_->bind(broadcast_ep_);

	strand_->post(boost::bind(&Heartbeat::start_receive, this));
	send_heartbeat();
}

void Heartbeat::start_receive()
{
	boost::shared_ptr<boost::asio::ip::udp::endpoint> remote_endpoint =
				boost::make_shared<boost::asio::ip::udp::endpoint>();

	recv_buffer_.fill(0);
	udp_socket_->async_receive_from(
				boost::asio::buffer(recv_buffer_), *remote_endpoint,
				strand_->wrap(boost::bind(&Heartbeat::handle_receive, this,
						remote_endpoint,
						boost::asio::placeholders::error,
						boost::asio::placeholders::bytes_transferred))
			);
}

void Heartbeat::send_heartbeat()
{
	udp_socket_->async_send_to(boost::asio::buffer(myid_), broadcast_ep_,
			boost::bind(
					&Heartbeat::handle_send, this,
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred
			)
		);

	heartbeat_timer_->expires_at(heartbeat_timer_->expires_at() + boost::posix_time::milliseconds(HEARTBEAT_INTERVAL));
	heartbeat_timer_->async_wait(boost::bind(&Heartbeat::send_heartbeat, this));

//	std::cout << "send heartbeat from " << myid_ << std::endl;
}

void Heartbeat::handle_receive(boost::shared_ptr<boost::asio::ip::udp::endpoint> remote_ep,
							   const boost::system::error_code& error, std::size_t bytes_transferred)
{
	if (!error)
	{
		std::string peer_id(recv_buffer_.data());
		if (myid_ != peer_id)
		{
			boost::asio::ip::address address = remote_ep->address();
			std::string peer_ip = address.to_string();

//			std::cout << "heartbeat from: " << peer_ip << " " << peer_id << std::endl;
			reflesh_peer(peer_id, peer_ip);
		}
		else
		{
//			std::cout << "xxxxxx\n";
		}
	}
	else
	{
		std::cerr << error.message() << std::endl;
	}

	strand_->post(boost::bind(&Heartbeat::start_receive, this));
}

void Heartbeat::handle_send(const boost::system::error_code& error, std::size_t bytes_transferred)
{
}

void Heartbeat::reflesh_peer(const std::string& peer_id, const std::string& peer_ip)
{
	PeerMap::iterator ite = peers_.find(peer_id);
	if (ite != peers_.end())  // find it !
	{
		boost::shared_ptr<boost::asio::deadline_timer> timer = ite->second.timer;
		timer->cancel();
		timer->expires_from_now(boost::posix_time::milliseconds(PEER_LIFE_CYCLE));
		timer->async_wait(boost::bind(&Heartbeat::kill_peer, this, peer_id, boost::asio::placeholders::error));
	}
	else // join in
	{
		Peer peer;
		boost::posix_time::milliseconds peer_life_cycle(PEER_LIFE_CYCLE);
		boost::shared_ptr<boost::asio::deadline_timer> timer =
				boost::make_shared<boost::asio::deadline_timer>(
						boost::ref(*ios_ptr_),
						boost::ref(peer_life_cycle)
			    );
		peer.timer = timer;
		peers_[peer_id] = peer;
		timer->async_wait(boost::bind(&Heartbeat::kill_peer, this, peer_id, boost::asio::placeholders::error));

		std::cout << peer_id << " has joined in the meeting.\n";
		report_newcomer(peer_id, peer_ip);
	}
}

void Heartbeat::kill_peer(const std::string& peer_id, const boost::system::error_code& err)
{
	if (err)
	{
		return;
	}

	PeerMap::iterator ite = peers_.find(peer_id);

	if (ite != peers_.end())
	{
		std::cout << ite->first << " has left the meeting.\n";
		peers_.erase(ite);
	}
}

void Heartbeat::report_newcomer(const std::string& peer_id, const std::string& peer_ip)
{
	Action action;
	action.type = JOIN;
	strcpy(action.id, peer_id.c_str());
	strcpy(action.ip, peer_ip.c_str());

	int ret = zmq_send(gate_, (void*)&action, sizeof(action), 0);
	if (ret < 0)
	{
		zmq_strerror(errno);
	}
}

void Heartbeat::report_departure(const std::string& peer_id, const std::string& peer_ip)
{
	Action action;
	action.type = LEAVE;
	memcpy(action.id, peer_id.c_str(), peer_id.length());
	memcpy(action.ip, peer_ip.c_str(), peer_ip.length());

	int ret = zmq_send(gate_, (void*)&action, sizeof(action), 0);
	if (ret < 0)
	{
		zmq_strerror(errno);
	}
}
