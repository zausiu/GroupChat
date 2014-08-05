/********************************************************************
 * Proactor.cpp
 * Author: kamuszhou@tencent.com, kamuszhou@qq.com
 * Website: v.qq.com www.dogeye.net
 * Created on: 2014/05/08 13:09
 * Purpose:	The proactor design pattern.
*********************************************************************/
#include <boost/make_shared.hpp>
#include "proactor.h"

using namespace boost::asio;

Proactor g_proactor;

Proactor::Proactor(void) : work_(ios_)
{
	running_ = false;
}

Proactor::~Proactor(void)
{
	stop();
}

void Proactor::start()
{
	// launch the thread group.
	int thrd_num = 0;
	int core_nbr = boost::thread::hardware_concurrency();
	if (thrd_num <= 0)
	{
		thrd_num = core_nbr;
	}
	thrd_num = 1;
	for(int i = 0; i < thrd_num; ++i)
	{
		threads_.create_thread(boost::bind(&boost::asio::io_service::run, &ios_));
	}

	running_ = true;
}

void Proactor::stop()
{
	if (!running_)
		return;

	ios_.stop();
	threads_.join_all();

	running_ = false;
}
