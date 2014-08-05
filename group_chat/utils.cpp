/*********************************************
 * Utils.cpp
 * Author: kamuszhou@tencent.com kamuszhou@qq.com
 * website: v.qq.com  www.dogeye.net
 * Created on: Aug 5, 2014
 * Praise Be to the Lord. BUG-FREE CODE !
 ********************************************/

#include "utils.h"

unsigned short create_port(const std::string& id)
{
	unsigned short port = 0;
	int len = id.length();

	if (len >= 1)
	{
		port += id[0];
	}
	if (len >= 2)
	{
		port |= (id[len - 1] << 8);
	}

	return port;
}
