#ifndef _FGW_SERVER_H__
#define _FGW_SERVER_H__

#include "libframework/zframework.h"
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


 public:
 	void removeHandler(ZServerHandler *h);

 protected:
	virtual void onAccept(evutil_socket_t fd, struct sockaddr_in *addr, unsigned short port);

 protected:
	handler_id_t genHandlerId();
	void deleteClosedHandlers();

	typedef std::map<handler_id_t, ZServerHandler*> MAP_TYPE;

 private:
 	MAP_TYPE handler_map_;
 	std::vector<ZServerHandler*> delete_handler_list_;
};

#endif // _FGW_SERVER_H__

