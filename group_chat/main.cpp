/*********************************************
 * main.h
 * Author: kamuszhou@tencent.com kamuszhou@qq.com
 * website: v.qq.com  www.dogeye.net
 * Created on: Aug 3, 2014
 * Praise Be to the Lord. BUG-FREE CODE !
 ********************************************/

/*
 *
 *             _=,_
 *         o_/6 /#\  woof ! woof!! bark !!!
 *         \__ |##/
 *          ='|--\
 *            /   #'-.
 *            \#|_   _'-. /
 *             |/ \_( # |"
 *            C/ ,--___/
 *
 *      www.tencent.com & www.dogeye.net co-present
 *      Praise Be to the Lord. BUG-FREE CODE !
 */


#include <iostream>
#include <string>
#include "proactor.h"
#include "zmq_ctx.h"
#include "heartbeat.h"
#include "chat.h"

int main(int argc, char** argv)
{
	std::string id;
	if (argc > 1)
	{
		id = argv[1];
		std::cout << "Welcome " << id << " to the chat room.\n";
	}
	else
	{
		std::cerr << "Need an ID.\n";
		exit(1);
	}

	g_proactor.start();
	g_zmq_ctx.init();
	g_chat.init(id);
	g_heartbeat.init(id);

	g_chat.run();

	return 0;
}
