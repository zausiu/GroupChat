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

int main(int argc, char** argv)
{
	g_proactor.start();

	return 0;
}
