#include "zwebapi_server.h"

#include <assert.h>

#include "libframework/zframework.h"

#include "zwebapi_handler.h"

int ZWebApiServer::init()
{
	return super_::init();
}

void ZWebApiServer::close()
{
	super_::close();
}

void ZWebApiServer::removeHandler(ZServerHandler *h)
{
	HANDLER_MAP_TYPE::iterator iter = handler_map_.find(h->getId());
	if (iter != handler_map_.end()) {
		handler_map_.erase(iter);
	} else {
		Z_LOG_W("id[%d] doesn't exist");
		// no, we can't return, this handler must be deleted
		// return;
	}

	delete_handler_list_.push_back(h);

	Z_LOG_I("%u clients", handler_map_.size());
}

void ZWebApiServer::deleteClosedHandlers()
{
	size_t list_len = delete_handler_list_.size();
	for (size_t i = 0; i < list_len; ++i) {
		Z_LOG_D("deleting: %p", delete_handler_list_[i]);
		delete delete_handler_list_[i];
	}

	delete_handler_list_.clear();
}

int ZWebApiServer::onInnerMsg(ZInnerMsg *msg)
{
	Z_LOG_D("ZWebApiServer::onInnerMsg()");

	handler_id_t handler_id = msg->dst_addr_.handler_id_;
	// if (handler_id < MIN_HANDLER_ID || handler_id > MAX_HANDLER_ID) {
	// 	Z_LOG_D("Bad handler id: %d", handler_id);
	// 	return FAIL;
	// }

	if (ANY_ID == handler_id) {
		// TODO:
		// XXX: should do load-balancing, current just use the first one
		HANDLER_MAP_TYPE::iterator iter = handler_map_.begin();
		if (iter == handler_map_.end()) {
			Z_LOG_D("Empty handler map...:(");
			return FAIL;
		}
		iter->second->onInnerMsg(msg);
	// } else if (BROADCAST_ID = handler_id) {
	// 	// TODO:
	} else {
		HANDLER_MAP_TYPE::iterator iter = handler_map_.find(handler_id);
		if (iter == handler_map_.end()) {
			Z_LOG_D("No such handler: %lu, %d", handler_map_.size(), handler_id);
			return -1;
		}

		iter->second->onInnerMsg(msg);
	}
	return OK;
}

void ZWebApiServer::routine(long delta)
{
	HANDLER_MAP_TYPE::iterator iter = handler_map_.begin();
	for (; iter != handler_map_.end(); ++iter) {
		iter->second->routine(delta);
	}

	deleteClosedHandlers();
}

void ZWebApiServer::onAccept(
		evutil_socket_t fd, struct sockaddr_in *addr, unsigned short port)
{
	Z_LOG_D("ZWebApiServer::onAccept(fd=%d)", fd);

	int handler_id = genHandlerId();
	if (handler_id == INVALID_ID) {
		Z_LOG_D("Failed to generate handler id, handler full?");
		return;
	}

	ZServerHandler *h = new ZWebApiHandler(getBase(), handler_id, fd, this);
	assert(h);

	h->read_event_ =
		event_new(getBase(), fd, EV_READ|EV_PERSIST, ZServerHandler::SOCKET_CALLBACK, h);

	assert(h->init() == OK);

	event_add(h->read_event_, NULL);

	// add to handler map
	handler_map_[h->getId()] = h;
}

/// XXX: 
int ZWebApiServer::genHandlerId()
{
	static int id = MIN_HANDLER_ID;
	if (id > MAX_HANDLER_ID) {
		id = MIN_HANDLER_ID;
	}

	return id++;
}

