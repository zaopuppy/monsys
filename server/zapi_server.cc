#include "zwebapi_server.h"

#include <assert.h>

#include "zwebapi_module.h"
#include "zerrno.h"

static void SOCKET_CALLBACK(evutil_socket_t fd, short events, void *arg)
{
	assert(arg);
	ZTask* task = (ZTask*)arg;
	task->event(fd, events);
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
	printf("ZWebApiServer::onInnerMsg()");
	return 0;
}

void ZWebApiServer::onAccept(
		evutil_socket_t fd, struct sockaddr_in *addr, unsigned short port)
{
	ZWebApiModule *task = new ZWebApiModule(base_);
	assert(task);

	task->read_event_ =
		event_new(base_, fd, EV_READ|EV_PERSIST, SOCKET_CALLBACK, task);

	assert(task->init() == OK);

	event_add(task->read_event_, NULL);
}


