#ifndef _Z_SERVER_HANDLER_H__
#define _Z_SERVER_HANDLER_H__

#include <event2/event.h>

#include "zhandler.h"

class ZServerHandler : public ZHandler {
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


