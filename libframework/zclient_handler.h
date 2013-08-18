#ifndef _Z_CLIENT_HANDLER_H__
#define _Z_CLIENT_HANDLER_H__

#include "zhandler.h"

class ZClientHandler : public ZHandler {
 public:
 	ZClientHandler(int id, ZModule *module)
 		: ZHandler(id, module)
 		, fd_(-1), read_event_(NULL)
 	{}

 public:
 	virtual void onConnected() = 0;

 public:
	evutil_socket_t fd_;
	struct event *read_event_;
};

#endif // _Z_CLIENT_HANDLER_H__

