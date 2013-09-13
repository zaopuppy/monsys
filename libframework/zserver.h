#ifndef _ZSERVER_H__
#define _ZSERVER_H__

#include <iostream>
#include <event2/event.h>

#include "zmodule.h"
#include "zevent_proxy.h"

class ZServer : public ZModule {
 public:
	ZServer(const char *ip, uint16_t port, event_base *base, int type)
		: ZModule(type)
		, ip_(ip), port_(port), base_(base)
		, socket_event_proxy_(base, socket_callback)
	{
	}

	typedef ZModule super_;

 public:
	virtual int init();
	virtual void close();
	virtual int sendMsg(ZInnerMsg *msg);
	virtual int onInnerMsg(ZInnerMsg *msg);

 public:
	void event(evutil_socket_t fd, short events);
	event_base* getBase() { return base_; }

 protected:
	virtual void onAccept(evutil_socket_t fd, struct sockaddr_in *addr, unsigned short port) = 0;

 private:
	void acceptClient(evutil_socket_t fd, short events);

	static void socket_callback(evutil_socket_t fd, short events, void *arg);

 private:
	std::string ip_;
	uint16_t port_;

	evutil_socket_t fd_;
	event_base *base_;

	ZEventProxy socket_event_proxy_;
};


#endif // _ZSERVER__H__


