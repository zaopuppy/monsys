#ifndef _Z_API_SERVER_H__
#define _Z_API_SERVER_H__

#include <event2/event.h>

#include "framework/zframework.h"

#include "zmodule_ex.h"

class ZApiServer : public ZServer {
 public:
	ZApiServer(const char *ip, uint16_t port, event_base *base)
		: ZServer(ip, port, base, Z_MODULE_API) {
	}

	typedef ZServer super_;

 public:
	virtual int init();
	virtual void close();
	// virtual void doTimeout();
	virtual int onInnerMsg(ZInnerMsg *msg);
	virtual void routine(long delta) {}

 protected:
	virtual void onAccept(evutil_socket_t fd, struct sockaddr_in *addr, unsigned short port);

};

#endif // _Z_API_SERVER_H__

