#ifndef _ZSERVER_H__
#define _ZSERVER_H__

#include <iostream>
#include <event2/event.h>

#include "zmodule.h"

class ZServer : public ZModule {
 public:
	ZServer(const char *ip, uint16_t port, event_base *base, int type)
		: ip_(ip), port_(port), base_(base), type_(type) {
	}

	typedef ZModule super_;

 public:
	virtual int init();
	virtual void close();
	virtual int sendMsg(ZInnerMsg *msg);
	virtual int onInnerMsg(ZInnerMsg *msg);
	virtual int getType() { return type_; }

 public:
	void event(evutil_socket_t fd, short events);

 protected:
	virtual void onAccept(evutil_socket_t fd, struct sockaddr_in *addr, unsigned short port) = 0;

 protected:
	void acceptClient(evutil_socket_t fd, short events);

 protected:
	std::string ip_;
	uint16_t port_;

	evutil_socket_t fd_;
	event_base *base_;
	int type_;
};


#endif // _ZSERVER__H__


