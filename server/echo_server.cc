
#include "echo_server.h"


class EchoHandler : public ZServerHandler {
 public:
  EchoHandler(int id, evutil_socket_t fd, ZModule *module, struct event_base *base)
    : ZServerHandler(id, fd, module, base)
  {}

  typedef ZServerHandler super_;

public:
  virtual int init() { return 0; }
  virtual void close() {
    super_::close();
    ((EchoServer*)getModule())->removeHandler(this);
  }

  virtual int onRead(char *buf, uint32_t buf_len) {
    Z_LOG_D("onRead()");
    trace_bin(buf, buf_len);
    send(buf, buf_len);
    return 0;
  }

  virtual int onInnerMsg(ZInnerMsg *msg) { return 0; }
  virtual int onInnerMsgEx(ZInnerMsg *msg) { return 0; }
  virtual void routine(long delta) {}

  virtual int send(const char *buf, uint32_t buf_len) {
    Z_LOG_D("EchoHandler::send(%d)", getFd());
    int rv = ::send(getFd(), buf, buf_len, 0);
    Z_LOG_D("rv: %d", rv);
    return OK;
  }
};

void EchoServer::onAccept(evutil_socket_t fd, struct sockaddr_in *addr, unsigned short port)
{
  Z_LOG_D("EchoServer::onAccept");

  ZServerHandler *h = new EchoHandler(-1, fd, this, getBase());
  assert(h);

  // h->fd_ = fd;
  // h->setId(handler_id);
  // h->setModuleType(0); // should be Z_MODULE_FGW
  // h->read_event_ =
  //   event_new(getBase(), fd, EV_READ|EV_PERSIST, ZServerHandler::SOCKET_CALLBACK, h);
  h->read_event_proxy_.registerSocket(fd, EV_READ|EV_PERSIST, h, NULL);

  assert(h->init() == OK);

  // event_add(h->read_event_, NULL);
}

void EchoServer::removeHandler(ZServerHandler *h)
{
  Z_LOG_D("EchoServer::removeHandler(%p)", h);
  delete_handler_list_.push_back(h);
}


