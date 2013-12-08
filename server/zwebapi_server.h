#ifndef _Z_WEBAPI_SERVER_H__
#define _Z_WEBAPI_SERVER_H__

#include <event2/event.h>
#include <map>

#include "libframework/zframework.h"

#include "module.h"
#include "zgenerator.h"

class ZWebApiServer : public ZServer {
 public:
  ZWebApiServer(const char *ip, uint16_t port, event_base *base)
    : ZServer(ip, port, base, MODULE_WEBAPI)
    , handler_id_generator_(1, MAX_HANDLER_ID)
  {
  }

  typedef ZServer super_;

 public:
  virtual int init();
  virtual void close();
  virtual int onInnerMsg(ZInnerMsg *msg);
  virtual void routine(long delta);

 public:
  void removeHandler(ZServerHandler *h);

  typedef std::map<int, ZServerHandler*> handler_map_type_t;
  typedef std::vector<ZServerHandler*> handler_list_type_t;

 protected:
  virtual void onAccept(evutil_socket_t fd, struct sockaddr_in *addr, unsigned short port);

 private:
  int genHandlerId();
  void deleteClosedHandlers();

 private:
  handler_map_type_t handler_map_;
  handler_list_type_t delete_handler_list_;

  HandlerIdGenerator handler_id_generator_;
};

#endif // _Z_WEBAPI_SERVER_H__

