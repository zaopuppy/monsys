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
  if (handler_id < MIN_HANDLER_ID || handler_id > MAX_HANDLER_ID) {
   Z_LOG_D("Bad handler id: %d", handler_id);
   return FAIL;
  }

  if (ANY_ID == handler_id) {
    // TODO:
    // XXX: should do load-balancing, current just use the first one
    handler_map_type_t::iterator iter = handler_map_.begin();
    if (iter == handler_map_.end()) {
      Z_LOG_D("Empty handler map...:(");
      return FAIL;
    }
    iter->second->onInnerMsg(msg);
  // } else if (BROADCAST_ID = handler_id) {
  //  // TODO:
  } else {
    handler_map_type_t::iterator iter = handler_map_.find(handler_id);
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
  // Z_LOG_D("ZWebApiServer::routine()");
  handler_map_type_t::iterator iter = handler_map_.begin();
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

  h->read_event_proxy_.registerSocket(fd, EV_READ|EV_PERSIST, h, NULL);

  int rv = h->init();
  assert(rv == OK);

  // add to handler map
  handler_map_[h->getId()] = h;
}

int ZWebApiServer::genHandlerId()
{
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

  Z_LOG_E("(WebApiServer)Out of handler id:(, current client count: %u", handler_map_.size());
  return INVALID_ID;
}

