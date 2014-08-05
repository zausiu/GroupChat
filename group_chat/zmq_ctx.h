/*********************************************
 * zmq.h
 * Author: kamuszhou@tencent.com kamuszhou@qq.com
 * website: v.qq.com  www.dogeye.net
 * Created on: Aug 5, 2014
 * Praise Be to the Lord. BUG-FREE CODE !
 ********************************************/

#ifndef ZMQ_H_
#define ZMQ_H_

class Zmq;
extern Zmq g_zmq_ctx;

class Zmq
{
public:
	Zmq();
	virtual ~Zmq();

	void init();

	void* get_zmq_ctx();

private:
	void* zmq_ctx_;
};

#endif /* ZMQ_H_ */
