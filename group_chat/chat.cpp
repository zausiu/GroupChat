/*********************************************
 * Chat.cpp
 * Author: kamuszhou@tencent.com kamuszhou@qq.com
 * website: v.qq.com  www.dogeye.net
 * Created on: Aug 5, 2014
 * Praise Be to the Lord. BUG-FREE CODE !
 ********************************************/
#include <iostream>
#include <assert.h>
#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>
#include <zmq.h>
#include "chat.h"
#include "utils.h"
#include "zmq_ctx.h"

Chat g_chat;

Chat::Chat()
{
}

Chat::~Chat()
{
	zmq_close(gate_);
	zmq_close(publisher_);
	zmq_close(subscriber_);
}

void Chat::init(const std::string& my_id)
{
	std::string bind_ep;

	listening_port_ = create_port(my_id);
	bind_ep = std::string("tcp://*:") + boost::lexical_cast<std::string>(listening_port_);

	zmq_ctx_ = g_zmq_ctx.get_zmq_ctx();

	publisher_ = zmq_socket(zmq_ctx_, ZMQ_PUB);
	zmq_bind(publisher_, bind_ep.c_str());

	subscriber_ = zmq_socket(zmq_ctx_, ZMQ_SUB);
	zmq_setsockopt(subscriber_, ZMQ_SUBSCRIBE, "", 0);

	gate_ = zmq_socket(zmq_ctx_, ZMQ_PAIR);
	zmq_bind(gate_, MAGIC_GATE);

	std::string test_msg("hello from ");
	test_msg += my_id;
	zmq_send(publisher_, test_msg.c_str(), test_msg.length(), 0);
}

void Chat::join(const std::string& peer_id, const std::string& peer_ip)
{
	std::string peer_ep;
	unsigned short peer_port = create_port(peer_id);
	peer_ep = (boost::format("tcp://%s:%d") % peer_ip % peer_port).str();

	if(zmq_connect(subscriber_, peer_ep.c_str()) < 0)
	{
		zmq_strerror(errno);
	}
}

void Chat::leave(const std::string& peer_id, const std::string& peer_ip)
{
	std::string peer_ep;
	unsigned short peer_port = create_port(peer_id);
//	peer_ep = std::string("tcp://*:") + boost::lexical_cast<std::string>(peer_port);
	peer_ep = (boost::format("tcp://%s:%d") % peer_ip % peer_port).str();

	if (zmq_disconnect(subscriber_, peer_ep.c_str()) < 0)
	{
		zmq_strerror(errno);
	}
}

void Chat::handle_gate(Action* action)
{
	std::string id(action->id);
	std::string ip(action->ip);

	if (action->type == JOIN)
	{
		join(id, ip);
	}
	else if (action->type == LEAVE)
	{
		leave(id, ip);
	}
}

void Chat::run()
{
	char buff[1024] = { 0 };
	zmq_pollitem_t items[3] = { 0 };

	items[0].socket = subscriber_;
	items[0].events = ZMQ_POLLIN;

	items[1].socket = gate_;
	items[1].events = ZMQ_POLLIN;

	items[2].fd = STDIN_FILENO;
	items[2].events = ZMQ_POLLIN;

	while(true)
	{
		int len;
		int ret = zmq_poll(items, 3, -1);

		if (items[0].revents & ZMQ_POLLIN)
		{
			len = zmq_recv(items[0].socket, buff, sizeof(buff), 0);
			if (len > 0)
			{
				buff[len] = 0;
				std::cout << buff << std::endl;
			}
			else
			{
				std::cerr << zmq_strerror(errno);
			}
		}

		if (items[1].revents & ZMQ_POLLIN)
		{
			len = zmq_recv(items[1].socket, buff, sizeof(buff), 0);
			if (len > 0)
			{
				handle_gate((Action*)buff);
			}
			else
			{
				std::cerr << zmq_strerror(errno);
			}
		}

		if (items[2].revents & ZMQ_POLLIN)
		{
			std::cin.getline(buff, sizeof(buff));
			zmq_send(publisher_, buff, std::cin.gcount(), 0);
		}
	}
}
