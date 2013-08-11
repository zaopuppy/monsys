#ifndef _Z_WEBAPI_SERVER_H__
#define _Z_WEBAPI_SERVER_H__

#include <event2/event.h>
#include <map>

#include "libframework/zframework.h"

#include "module.h"

class ZWebApiServer : public ZServer {
 public:
	ZWebApiServer(const char *ip, uint16_t port, event_base *base)
		: ZServer(ip, port, base, Z_MODULE_WEBAPI) {
	}

	typedef ZServer super_;

 public:
	virtual int init();
	virtual void close();
	virtual int onInnerMsg(ZInnerMsg *msg);
	virtual void routine(long delta);

	typedef std::map<int, ZServerHandler*> HANDLER_MAP_TYPE;

 protected:
	virtual void onAccept(evutil_socket_t fd, struct sockaddr_in *addr, unsigned short port);

 private:
 	int genHandlerId();

 private:
	// XXX
	// ZServerHandler *handler_;
	std::map<int, ZServerHandler*> handler_map_;
};

#endif // _Z_WEBAPI_SERVER_H__

