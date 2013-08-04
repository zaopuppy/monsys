#ifndef _ZCLIENT_H__
#define _ZCLIENT_H__

#include <event2/event.h>

#include "zmodule.h"
#include "zinner_message.h"

class ZClient : public ZModule {
public:
	ZClient(event_base* base, int type)
		: base_(base), fd_(-1), type_(type)
	{
	}

	typedef ZModule super_;

public:
	virtual int init();
	virtual void close();
	virtual int sendMsg(ZInnerMsg *msg);
	virtual int onInnerMsg(ZInnerMsg *msg);
	virtual int getType() { return type_; }

	// XXX: use `timeout event' instead of freqent timeout check
	// virtual void doTimeout();
	// virtual bool isComplete();

	void event(evutil_socket_t fd, short events);

private:
	int onWaitingForConnect(evutil_socket_t fd, short events);
	void onConnected(evutil_socket_t fd, short events);
	int onDisconnected(evutil_socket_t fd, short events);

	int connect();
	void scheduleReconnect();

private:
	enum STATE {
		STATE_WAITING_FOR_CONNECT,
		STATE_CONNECTED,
		STATE_DISCONNECTED,
		STATE_FINISHED,
	};

private:
	event_base *base_;
	evutil_socket_t fd_;
	struct event* read_event_;
	// struct event* write_event_;
	STATE state_;
	char buf_[1 << 10];
	int type_;
};

#endif // _ZCLIENT_H__

