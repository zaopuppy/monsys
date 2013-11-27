#ifndef _echo_SERVER_H__
#define _echo_SERVER_H__

#include "libframework/zframework.h"
#include "module.h"

// FGW is short for "Family gateway"
class EchoServer : public ZServer {
 public:
  EchoServer(const char *ip, uint16_t port, event_base *base)
  : ZServer(ip, port, base, -1)
  {}

 public:
  // virtual int onInnerMsg(ZInnerMsg *msg) { return 0; }
  virtual void routine(long delta) { }

 public:
  void removeHandler(ZServerHandler *h);

 protected:
  virtual void onAccept(evutil_socket_t fd, struct sockaddr_in *addr, unsigned short port);

 protected:
  void deleteClosedHandlers();

 private:
  std::vector<ZServerHandler*> delete_handler_list_;
};

#endif // _echo_SERVER_H__

