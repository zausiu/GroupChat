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
#include "heartbeat.h"

int main(int argc, char** argv)
{
	std::string id;
	if (argc > 1)
	{
		id = argv[1];
	}
	else
	{
		id = "kamus";
	}

	g_proactor.start();
	g_heartbeat.init(id);

	g_proactor.get_ios().run();

	return 0;
}
