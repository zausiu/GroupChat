/*********************************************
 * Chat.h
 * Author: kamuszhou@tencent.com kamuszhou@qq.com
 * website: v.qq.com  www.dogeye.net
 * Created on: Aug 5, 2014
 * Praise Be to the Lord. BUG-FREE CODE !
 ********************************************/

#ifndef CHAT_H_
#define CHAT_H_

#include <string>
#include <zmq.h>

#define MAGIC_GATE "inproc://gate"

class Chat;
extern Chat g_chat;

enum ActionType { JOIN = 0xf, LEAVE };

struct Action
{
	ActionType type;
	char id[16];
	char ip[16];
};

class Chat
{
public:
	Chat();
	virtual ~Chat();

	void init(const std::string& my_id);

	void run();

private:
	void join(const std::string& peer_id, const std::string& peer_ip);
	void leave(const std::string& peer_id, const std::string& peer_ip);

	void handle_gate(Action* action);

private:
	unsigned short listening_port_;

	void* zmq_ctx_;
	void* publisher_;
	void* subscriber_;
	void* gate_;
};

#endif /* CHAT_H_ */
