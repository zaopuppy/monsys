#ifndef _Z_SERVER_HANDLER_H__
#define _Z_SERVER_HANDLER_H__

#include <assert.h>

#include <event2/event.h>

#include "framework/zframework.h"

class ZServerHandler : public ZHandler {
 public:
 	ZServerHandler(): fd_(-1), read_event_(NULL) {}

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

 public:
	virtual int event(evutil_socket_t fd, short events);

 public:
	evutil_socket_t fd_;
	struct event *read_event_;

 private:
	char buf_[1 << 10];
};

#endif // _Z_SERVER_HANDLER_H__


