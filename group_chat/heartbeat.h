/*********************************************
 * heartbeat.h
 * Author: kamuszhou@tencent.com kamuszhou@qq.com
 * website: v.qq.com  www.dogeye.net
 * Created on: Aug 3, 2014
 * Praise Be to the Lord. BUG-FREE CODE !
 ********************************************/

#ifndef HEARTBEAT_H_
#define HEARTBEAT_H_

#include <boost/asio.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/unordered_map.hpp>
#include <zmq.h>
#include "chat.h"

class Heartbeat;
extern Heartbeat g_heartbeat;

class Heartbeat
{
public:
	Heartbeat();
	virtual ~Heartbeat();

	void init(std::string id);

private:
	void start_receive();
	void send_heartbeat();

	void handle_receive(boost::shared_ptr<boost::asio::ip::udp::endpoint> remote_ep,
						const boost::system::error_code& error, std::size_t bytes_transferred);
	void handle_send(const boost::system::error_code& error, std::size_t bytes_transferred);

	void reflesh_peer(const std::string& peer_id, const std::string& peer_ip);
	void kill_peer(const std::string& peer_id, const boost::system::error_code& err);

	void report_newcomer(const std::string& peer_id, const std::string& peer_ip);
	void report_departure(const std::string& peer_id, const std::string& peer_ip);

private:
	boost::asio::io_service* ios_ptr_;
	boost::scoped_ptr<boost::asio::strand> strand_;
	boost::scoped_ptr<boost::asio::strand> strand4peermap_;

	boost::scoped_ptr<boost::asio::ip::udp::socket> udp_socket_;
	boost::array<char,1024> recv_buffer_;
	boost::asio::ip::address_v4 broadcast_addr_;
	boost::asio::ip::udp::endpoint broadcast_ep_;
	boost::scoped_ptr<boost::asio::deadline_timer> heartbeat_timer_;

	std::string myid_;

	struct Peer
	{
		boost::shared_ptr<boost::asio::deadline_timer> timer;
	};
	typedef boost::unordered_map<std::string, Peer> PeerMap;
	PeerMap peers_;

	void* gate_;
};

#endif /* HEARTBEAT_H_ */
