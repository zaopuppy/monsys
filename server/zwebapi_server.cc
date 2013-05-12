#include "zwebapi_server.h"

#include <assert.h>

#include "zwebapi_handler.h"
#include "zerrno.h"
#include "zhandler.h"

static void SOCKET_CALLBACK(evutil_socket_t fd, short events, void *arg)
{
	assert(arg);
	ZWebApiHandler *h = (ZWebApiHandler*)arg;
	h->event(fd, events);
}

int ZWebApiServer::init()
{
	return super_::init();
}

void ZWebApiServer::close()
{
	super_::close();
}

int ZWebApiServer::onInnerMsg(ZInnerMsg *msg)
{
	printf("ZWebApiServer::onInnerMsg()\n");

	handler_->onInnerMsg(msg);

	return 0;
}

void ZWebApiServer::onAccept(
		evutil_socket_t fd, struct sockaddr_in *addr, unsigned short port)
{
	// ZWebApiHandler *h = new ZWebApiHandler(base_);
	handler_ = new ZWebApiHandler(base_);
	assert(handler_);

	handler_->read_event_ =
		event_new(base_, fd, EV_READ|EV_PERSIST, SOCKET_CALLBACK, handler_);

	assert(handler_->init() == OK);

	event_add(handler_->read_event_, NULL);
}


