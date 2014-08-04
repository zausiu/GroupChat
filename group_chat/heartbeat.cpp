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

#define HEARTBEAT_PORT 9999
#define HEARTBEAT_INTERVAL 1000
#define PEER_LIFE_CYCLE 5000

Heartbeat g_heartbeat;

Heartbeat::Heartbeat()
{
}

Heartbeat::~Heartbeat()
{
}

void Heartbeat::init(std::string id)
{
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
			std::string str_addr = address.to_string();
			std::cout << "heartbeat from: " << str_addr << " " << peer_id << std::endl;
			reflesh_peer(recv_buffer_.data());
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

void Heartbeat::reflesh_peer(std::string peer_id)
{
	PeerMap::iterator ite = peers_.find(peer_id);
	if (ite != peers_.end())  // find it !
	{
		boost::shared_ptr<boost::asio::deadline_timer> timer = ite->second.timer_;
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
		peer.timer_ = timer;
		peers_[peer_id] = peer;
		timer->async_wait(boost::bind(&Heartbeat::kill_peer, this, peer_id, boost::asio::placeholders::error));

		std::cout << peer_id << " has joined in the meeting.\n";
	}
}

void Heartbeat::kill_peer(std::string peer_id, const boost::system::error_code& err)
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
