#ifndef _Z_SERVER_HANDLER_H__
#define _Z_SERVER_HANDLER_H__

#include <assert.h>

#include <event2/event.h>

#include "zhandler.h"

class ZServerHandler : public ZHandler {
 public:
 	ZServerHandler(int id, evutil_socket_t fd, ZModule *module)
 		: ZHandler(id, module)
 		, read_event_(NULL)
		,	fd_(fd)
	{}
 	virtual ~ZServerHandler() {}

 public:
 	static void SOCKET_CALLBACK(evutil_socket_t fd, short events, void *arg) {
 		assert(arg);
 		ZServerHandler *h = (ZServerHandler*)arg;
 		h->event(fd, events);
 	}

 public:
	virtual int init() = 0;
	virtual void close();
	virtual int onRead(char *buf, uint32_t buf_len) = 0;
	virtual int onInnerMsg(ZInnerMsg *msg) = 0;
	evutil_socket_t getFd() { return fd_; }

 public:
	virtual int event(evutil_socket_t fd, short events);

 public:
	struct event *read_event_;

 private:
	evutil_socket_t fd_;
	char buf_[1 << 10];
};

#endif // _Z_SERVER_HANDLER_H__


