#ifndef _FGW_SERVER_H__
#define _FGW_SERVER_H__

#include "libframework/zframework.h"
#include "module.h"
#include "fgw_server_handler.h"
#include "zgenerator.h"

// FGW is short for "Family gateway"
class FGWServer : public ZServer {
 public:
  FGWServer(const char *ip, uint16_t port, event_base *base)
  : ZServer(ip, port, base, MODULE_FGW_SERVER)
  , handler_id_generator_(1, MAX_HANDLER_ID)
  {}
  virtual ~FGWServer() {}

 public:
  virtual int onInnerMsg(ZInnerMsg *msg);
  virtual void routine(long delta);

 public:
  void removeHandler(ZServerHandler *h);

 protected:
  virtual void onAccept(evutil_socket_t fd, struct sockaddr_in *addr, unsigned short port);

 protected:
  handler_id_t genHandlerId();
  void deleteClosedHandlers();

  typedef std::map<handler_id_t, ZServerHandler*> handler_map_type_t;
  typedef std::vector<ZServerHandler*> handler_list_type_t;

 private:
  handler_map_type_t handler_map_;
  handler_list_type_t delete_handler_list_;
  HandlerIdGenerator handler_id_generator_;
};

#endif // _FGW_SERVER_H__

