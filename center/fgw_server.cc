#include "fgw_server.h"

#include "libbase/zlog.h"

void FGWServer::removeHandler(ZServerHandler *h)
{
	MAP_TYPE::iterator iter = handler_map_.find(h->getId());
	if (iter != handler_map_.end()) {
		handler_map_.erase(iter);
	} else {
		Z_LOG_W("id[%d] doesn't exist");
		// no, we can't return, this handler must be deleted
		// return;
	}

	delete_handler_list_.push_back(h);
}

void FGWServer::routine(long delta)
{
	Z_LOG_D("FGWServer::routine()\n");

	deleteClosedHandlers();
}

void FGWServer::deleteClosedHandlers()
{
	size_t list_len = delete_handler_list_.size();
	for (size_t i = 0; i < list_len; ++i) {
		Z_LOG_D("deleting: %p\n", delete_handler_list_[i]);
		delete delete_handler_list_[i];
	}

	delete_handler_list_.clear();
}


void FGWServer::onAccept(evutil_socket_t fd, struct sockaddr_in *addr, unsigned short port)
{
	Z_LOG_D("FGWServer::onAccept\n");

	handler_id_t handler_id = genHandlerId();
	if (handler_id == INVALID_ID) {
		Z_LOG_E("Failed to generate new handler id, handler full?");
		return;
	}

	ZServerHandler *h = new FGWHandler(this);
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


