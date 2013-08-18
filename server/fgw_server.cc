#include "fgw_server.h"

#include "libbase/zlog.h"

// XXX: this kind of code should be moved to upper class
int FGWServer::onInnerMsg(ZInnerMsg *msg)
{
	Z_LOG_D("FGWServer::onInnerMsg\n");

	handler_id_t handler_id = msg->dst_addr_.handler_id_;

	// if (handler_id < MIN_HANDLER_ID || handler_id > MAX_HANDLER_ID) {
	// 	printf("Bad handler id: %d\n", handler_id);
	// 	return FAIL;
	// }

	if (handler_map_.size() == 0) {
		Z_LOG_W("no client is connecting\n");
		return FAIL;
	}

	if (ANY_ID == handler_id) {
		MAP_TYPE::iterator iter = handler_map_.begin();
		if (iter == handler_map_.end()) {
			Z_LOG_E("No such handler: %d\n", handler_id);
			return FAIL;
		}

		iter->second->onInnerMsg(msg);
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
	} else {
		MAP_TYPE::iterator iter = handler_map_.find(handler_id);
		if (iter == handler_map_.end()) {
			printf("No such handler: %d\n", handler_id);
			return FAIL;
		}

		iter->second->onInnerMsg(msg);
	}

	return OK;
}


// FIXME: "W|id[1426196784] doesn't exist"
// happens when lots of client comes
void FGWServer::removeHandler(ZServerHandler *h)
{
	MAP_TYPE::iterator iter = handler_map_.find(h->getId());
	if (iter != handler_map_.end()) {
		handler_map_.erase(iter);
	} else {
		Z_LOG_W("id[%d] doesn't exist\n");
		// no, we can't return, this handler must be deleted
		// return;
	}

	delete_handler_list_.push_back(h);

	Z_LOG_D("%u clients\n", handler_map_.size());
}

void FGWServer::routine(long delta)
{
	// Z_LOG_D("FGWServer::routine()\n");

	deleteClosedHandlers();
}

void FGWServer::deleteClosedHandlers()
{
	size_t list_len = delete_handler_list_.size();
	for (size_t i = 0; i < list_len; ++i) {
		// Z_LOG_D("deleting: %p\n", delete_handler_list_[i]);
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

	ZServerHandler *h = new FGWHandler(handler_id, this);
	assert(h);

	h->fd_ = fd;
	// h->setId(handler_id);
	// h->setModuleType(0);	// should be Z_MODULE_FGW
	h->read_event_ =
		event_new(getBase(), fd, EV_READ|EV_PERSIST, ZServerHandler::SOCKET_CALLBACK, h);

	assert(h->init() == OK);

	event_add(h->read_event_, NULL);

	handler_map_[h->getId()] = h;

	Z_LOG_D("%u clients\n", handler_map_.size());
}

handler_id_t FGWServer::genHandlerId()
{
	static handler_id_t s_id = 1;
	// XXX: use defined max_handler_id in module.h
	static const handler_id_t MAX_HANDLER_NUM = 0xFFFFF; // > 100w

	MAP_TYPE::iterator iter;
	handler_id_t old_id = s_id;
	do {
		iter = handler_map_.find(s_id);
		if (iter == handler_map_.end()) {
			return s_id++;
		}
		++s_id;
		if (s_id > MAX_HANDLER_NUM) {
			s_id = 1;
		}
	} while (s_id != old_id);

	return -1;
}


