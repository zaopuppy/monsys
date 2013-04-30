#ifndef _ZCLIENT_H__
#define _ZCLIENT_H__

#include "ztask.h"

#include "zmodule.h"

class ZClient : public ZTask {
public:
	ZClient(event_base* base):
		ZTask(base, Z_MODULE_CLIENT), fd_(-1)	// XXX: should not be Z_MODULE_CLIENT
	{
	}

	typedef ZTask super_;

public:
	virtual int init();
	virtual void close();
	virtual void event(evutil_socket_t fd, short events);
	// XXX: use `timeout event' instead of freqent timeout check
	virtual void doTimeout();
	virtual bool isComplete();
	virtual int onInnerMsg(ZInnerMsg *msg);

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
	evutil_socket_t fd_;
	struct event* read_event_;
	// struct event* write_event_;
	STATE state_;
	char buf_[1 << 10];
};

#endif // _ZCLIENT_H__

