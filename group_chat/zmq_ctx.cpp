/*********************************************
 * zmq.cpp
 * Author: kamuszhou@tencent.com kamuszhou@qq.com
 * website: v.qq.com  www.dogeye.net
 * Created on: Aug 5, 2014
 * Praise Be to the Lord. BUG-FREE CODE !
 ********************************************/

#include <assert.h>
#include <zmq.h>
#include "zmq_ctx.h"

Zmq g_zmq_ctx;

Zmq::Zmq()
{
	zmq_ctx_ = NULL;
}

Zmq::~Zmq()
{
	if (NULL != zmq_ctx_)
	{
		int ret = zmq_ctx_term(zmq_ctx_);
	}
}

void Zmq::init()
{
	zmq_ctx_ = zmq_ctx_new();
	assert(NULL != zmq_ctx_);
}

void* Zmq::get_zmq_ctx()
{
	return zmq_ctx_;
}
