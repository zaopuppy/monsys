#include "fgw_server.h"


// TODO: this kind of code should be moved to upper class
int FGWServer::onInnerMsg(ZInnerMsg *msg)
{
  Z_LOG_D("FGWServer::onInnerMsg");

  handler_id_t handler_id = msg->dst_addr_.handler_id_;

  if (handler_id < MIN_HANDLER_ID || handler_id > MAX_HANDLER_ID) {
   Z_LOG_E("Bad handler id: %d", handler_id);
   return FAIL;
  }

  if (handler_map_.size() == 0) {
    Z_LOG_W("no client is connected");
    return FAIL;
  }

  if (ANY_ID == handler_id) {
    Z_LOG_D("ANY_ID");
    handler_map_type_t::iterator iter = handler_map_.begin();
    if (iter == handler_map_.end()) {
      Z_LOG_E("Empty handler map: %d", handler_id);
      return FAIL;
    }

    iter->second->onInnerMsg(msg);
  //  // TODO:
  //  // XXX: should do load-balancing, current just use the first one
  //  HANDLER_MAP_TYPE::iterator iter = handler_map_.begin();
  //  if (iter == handler_map_.end()) {
  //    printf("Empty handler map...:(");
  //    return;
  //  }
  //  iter->second->onInnerMsg(msg);
  // } else if (BROADCAST_ID = handler_id) {
  //  // TODO:
  } else {
    handler_map_type_t::iterator iter = handler_map_.find(handler_id);
    if (iter == handler_map_.end()) {
      printf("No such handler: %d", handler_id);
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
  Z_LOG_D("FGWServer::removeHandler(%p)", h);
  handler_map_type_t::iterator iter = handler_map_.find(h->getId());
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

void FGWServer::routine(long delta)
{
  // Z_LOG_D("FGWServer::routine()");
  handler_map_type_t::iterator iter = handler_map_.begin();
  for (; iter != handler_map_.end(); ++iter) {
    iter->second->routine(delta);
  }

  deleteClosedHandlers();
}

void FGWServer::deleteClosedHandlers()
{
  size_t list_len = delete_handler_list_.size();
  for (size_t i = 0; i < list_len; ++i) {
    Z_LOG_D("deleting: %p", delete_handler_list_[i]);
    delete delete_handler_list_[i];
  }

  delete_handler_list_.clear();
}


void FGWServer::onAccept(evutil_socket_t fd, struct sockaddr_in *addr, unsigned short port)
{
  Z_LOG_D("FGWServer::onAccept");

  handler_id_t handler_id = genHandlerId();
  if (handler_id == INVALID_ID) {
    Z_LOG_E("Failed to generate new handler id, handler full?");
    return;
  }

  ZServerHandler *h = new FGWServerHandler(handler_id, fd, this, getBase());
  assert(h);

  // h->fd_ = fd;
  // h->setId(handler_id);
  // h->setModuleType(0); // should be Z_MODULE_FGW
  h->read_event_proxy_.registerSocket(fd, EV_READ|EV_PERSIST, h, NULL);
  // h->read_event_ =
  //   event_new(getBase(), fd, EV_READ|EV_PERSIST, ZServerHandler::SOCKET_CALLBACK, h);

  int rv = h->init();
  assert(rv == OK);

  // event_add(h->read_event_, NULL);

  // handler_map_[h->getId()] = h;
  bool result = handler_map_.insert(
    std::pair<handler_id_t, ZServerHandler*>(h->getId(), h)).second;
  if (!result) {
    Z_LOG_E("Failed to insert handler into handler map");
    h->close();
    delete h;
  }

  Z_LOG_I("%u clients", handler_map_.size());
}

handler_id_t FGWServer::genHandlerId()
{
  // static handler_id_t s_id = 1;
  // // XXX: use defined max_handler_id in module.h
  // // static const handler_id_t MAX_HANDLER_NUM = 0xFFFFF; // > 100w

  // handler_map_type_t::iterator iter;
  // handler_id_t old_id = s_id;
  // do {
  //   iter = handler_map_.find(s_id);
  //   if (iter == handler_map_.end()) {
  //     return s_id++;
  //   }
  //   ++s_id;
  //   if (s_id > MAX_HANDLER_ID) {
  //     s_id = 1;
  //   }
  // } while (s_id != old_id);
  handler_id_t first_id, final_id;

  first_id = final_id = handler_id_generator_.next();
  handler_map_type_t::iterator iter;
  do {
    iter = handler_map_.find(final_id);
    if (iter == handler_map_.end()) {
      return final_id;
    }
    final_id = handler_id_generator_.next();
  } while (final_id != first_id);

  Z_LOG_E("(FGWServer)Out of handler id:(, current client count: %u", handler_map_.size());
  return INVALID_ID;
}


