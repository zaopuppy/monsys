#include "zapi_server.h"

#include <assert.h>

#include "zframework.h"

#include "zapi_module.h"

static void SOCKET_CALLBACK(evutil_socket_t fd, short events, void *arg)
{
	assert(arg);
	ZApiModule *p = (ZApiModule*)arg;
	p->event(fd, events);
}

int ZApiServer::init()
{
	return super_::init();
}

void ZApiServer::close()
{
	super_::close();
}

int ZApiServer::onInnerMsg(ZInnerMsg *msg)
{
	printf("ZApiServer::onInnerMsg()");
	return 0;
}

void ZApiServer::onAccept(
		evutil_socket_t fd, struct sockaddr_in *addr, unsigned short port)
{
	ZApiModule *task = new ZApiModule(base_);
	assert(task);

	task->read_event_ =
		event_new(base_, fd, EV_READ|EV_PERSIST, SOCKET_CALLBACK, task);

	assert(task->init() == OK);

	event_add(task->read_event_, NULL);
}


