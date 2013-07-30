#include "fgw_server.h"

#include "zlog.h"

void FGWServer::routine(long delta)
{
	Z_LOG_D("FGWServer::routine()\n");
}

void FGWServer::onAccept(evutil_socket_t fd, struct sockaddr_in *addr, unsigned short port)
{
	Z_LOG_D("FGWServer::onAccept\n");

	handler_id_t handler_id = genHandlerId();
	if (handler_id == INVALID_ID) {
		Z_LOG_E("Failed to generate new handler id, handler full?");
		return;
	}

	ZServerHandler *h = new FGWHandler();
	assert(h);

	h->setId(handler_id);
	h->setModuleType(0);	// should be Z_MODULE_FGW
	h->read_event_ =
		event_new(base_, fd, EV_READ|EV_PERSIST, ZServerHandler::SOCKET_CALLBACK, h);

	assert(h->init() == OK);

	event_add(h->read_event_, NULL);

	handler_map_[h->getId()] = h;
}

handler_id_t FGWServer::genHandlerId()
{
	return 0;
}


