#ifndef _FGW_SERVER_H__
#define _FGW_SERVER_H__

#include "framework/zframework.h"
#include "module.h"
#include "fgw_handler.h"

// FGW is short for "Family gateway"
class FGWServer : public ZServer {
 public:
	FGWServer(const char *ip, uint16_t port, event_base *base)
	: ZServer(ip, port, base, MODULE_FGW_SERVER)
	{}
	~FGWServer() {}

 public:
 	virtual void routine(long delta);

 protected:
	virtual void onAccept(evutil_socket_t fd, struct sockaddr_in *addr, unsigned short port);

	handler_id_t genHandlerId();

 private:
 	std::map<handler_id_t, ZServerHandler*> handler_map_;
};

#endif // _FGW_SERVER_H__

