#include "zwebapi_server.h"

#include <assert.h>

#include "framework/zframework.h"

#include "zwebapi_handler.h"

// static void SOCKET_CALLBACK(evutil_socket_t fd, short events, void *arg)
// {
// 	assert(arg);
// 	ZWebApiHandler *h = (ZWebApiHandler*)arg;
// 	h->event(fd, events);
// }

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

	int handler_id = msg->dst_addr_.handler_id_;
	if (handler_id < MIN_HANDLER_ID || handler_id > MAX_HANDLER_ID) {
		printf("Bad handler id: %d\n", handler_id);
		return -1;
	}

	// if (ANY_ID == handler_id) {
	// 	// TODO:
	// 	// XXX: should do load-balancing, current just use the first one
	// 	HANDLER_MAP_TYPE::iterator iter = handler_map_.begin();
	// 	if (iter == handler_map_.end()) {
	// 		printf("Empty handler map...:(\n");
	// 		return;
	// 	}
	// 	iter->second->onInnerMsg(msg);
	// } else if (BROADCAST_ID = handler_id) {
	// 	// TODO:
	// } else {
		HANDLER_MAP_TYPE::iterator iter = handler_map_.find(handler_id);
		if (iter == handler_map_.end()) {
			printf("No such handler: %d\n", handler_id);
			return -1;
		}

		iter->second->onInnerMsg(msg);
	// }
	return 0;
}

void ZWebApiServer::routine(long delta)
{
	HANDLER_MAP_TYPE::iterator iter = handler_map_.begin();
	for (; iter != handler_map_.end(); ++iter) {
		iter->second->routine(delta);
	}
}

void ZWebApiServer::onAccept(
		evutil_socket_t fd, struct sockaddr_in *addr, unsigned short port)
{
	int handler_id = genHandlerId();
	if (handler_id == INVALID_ID) {
		printf("Failed to generate handler id, handler full?\n");
		return;
	}

	ZServerHandler *h = new ZWebApiHandler(base_);
	assert(h);

	h->setId(handler_id);
	h->setModuleType(Z_MODULE_WEBAPI);

	h->read_event_ =
		event_new(base_, fd, EV_READ|EV_PERSIST, ZServerHandler::SOCKET_CALLBACK, h);

	assert(h->init() == OK);

	event_add(h->read_event_, NULL);

	// add to handler map
	handler_map_[h->getId()] = h;
}

int ZWebApiServer::genHandlerId()
{
	static int id = MIN_HANDLER_ID;
	if (id > MAX_HANDLER_ID) {
		id = MIN_HANDLER_ID;
	}

	return id++;
}

