#ifndef _ZSERVER_H__
#define _ZSERVER_H__

#include "ztask.h"

#include <iostream>
#include <event2/event.h>

#include "zmodule.h"

class ZServer : public ZModule {
 public:
	ZServer(const char *ip, uint16_t port, event_base *base, Z_MODULE_TYPE type)
		: ip_(ip), port_(port), base_(base), state_(STATE_INIT), type_(type) {
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
	// virtual void onAccept_o(evutil_socket_t fd, short events);
	virtual void onAccept(evutil_socket_t fd, struct sockaddr_in *addr, unsigned short port) = 0;

 protected:
	void acceptClient(evutil_socket_t fd, short events);

 private:
	enum STATE {
		STATE_INIT,
		STATE_ACCEPTING,
		STATE_FINISHED,
	};

 protected:
	std::string ip_;
	uint16_t port_;

	// int type_;
	evutil_socket_t fd_;
	event_base *base_;
	STATE state_;
	Z_MODULE_TYPE type_;
};


#endif // _ZSERVER__H__


