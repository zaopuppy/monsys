#ifndef _Z_API_SERVER_H__
#define _Z_API_SERVER_H__

#include "zserver.h"

#include <event2/event.h>

class ZApiServer : public ZServer {
	public:
		ZApiServer(const char *ip, uint16_t port, event_base *base)
			: ZServer(ip, port, base) {
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

#endif // _Z_API_SERVER_H__

