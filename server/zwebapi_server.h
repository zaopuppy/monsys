#ifndef _Z_WEBAPI_SERVER_H__
#define _Z_WEBAPI_SERVER_H__

#include "zserver.h"

#include <event2/event.h>

class ZWebApiServer : public ZServer {
	public:
		ZWebApiServer(const char *ip, uint16_t port, event_base *base)
			: ZServer(ip, port, base, Z_MODULE_WEBAPI) {
		}

		typedef ZServer super_;

	public:
		virtual int init();
		virtual void close();
		// virtual void doTimeout();
		virtual int onInnerMsg(ZInnerMsg *msg);

	protected:
		virtual void onAccept(evutil_socket_t fd, struct sockaddr_in *addr, unsigned short port);

	private:
};

#endif // _Z_WEBAPI_SERVER_H__

