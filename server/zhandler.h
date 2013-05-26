#ifndef _Z_HANDLER_H__
#define _Z_HANDLER_H__

#include <event2/event.h>

#include "zinner_message.h"

class ZHandler {
 public:
	virtual int init() = 0;
	virtual void close() = 0;
	// virtual int event(evutil_socket_t fd, short events) = 0;
	virtual int onRead(char *buf, uint32_t buf_len) = 0;
	virtual int onInnerMsg(ZInnerMsg *msg) = 0;

	virtual int send(const char *buf, uint32_t buf_len) = 0;

 public:
	evutil_socket_t fd_;
	struct event *read_event_;
};


#endif // _Z_HANDLER_H__


