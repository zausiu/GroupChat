/********************************************************************
 * Proactor.h
 * Author: kamuszhou@tencent.com, kamuszhou@qq.com
 * Website: v.qq.com www.dogeye.net
 * Created on: 2014/05/08 13:08
 * Purpose:	The proactor design pattern based on the boost implementation asio.
*********************************************************************/

#ifndef _H_PROACTOR_
#define _H_PROACTOR_

#include <boost/thread.hpp>
#include <boost/asio.hpp>

class Proactor;
extern Proactor g_proactor;

class Proactor
{
public:
	Proactor(void);
	~Proactor(void);

	void start();
 
	boost::asio::io_service& get_ios()
	{
		return ios_;
	}

	void stop();

private:
	boost::asio::io_service ios_;
	boost::asio::io_service::work work_;
	boost::thread_group threads_;

	bool running_;
};

#endif
