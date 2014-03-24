#ifndef _ZWEBAPI_HANDLER_H__
#define _ZWEBAPI_HANDLER_H__

#include <map>

#include <event2/event.h>
#include <jansson.h>

#include "libframework/zframework.h"

// #include "zwebapi_session.h"
#include "zinner_message_ex.h"
#include "zwebapi_session.h"

// class ZWebApiHandler : public ZHandler {
class ZWebApiHandler : public ZServerHandler {
 public:
  ZWebApiHandler(event_base* base, int id, evutil_socket_t fd, ZModule *module)
    : ZServerHandler(id, fd, module, base)
  {
  }

  typedef ZServerHandler super_;

 public:
  virtual int init();
  virtual void close();
  virtual int onRead(char *buf, uint32_t buf_len);
  virtual int onInnerMsg(ZInnerMsg *msg);
  virtual void routine(long delta);

  virtual int send(const char *buf, uint32_t buf_len) {
    Z_LOG_D("ZWebApiHandler::send(fd=%d)", getFd());
    return ::send(getFd(), buf, buf_len, 0);
  }

public:
  int send(json_t *msg);

  // typedef ZSessionCtrl<uint32_t, ZWebApiSession*> SESSIONCTRL_TYPE;
private:
  int processFGWGetListReq(json_t *jmsg);

  void sendRsp(const char *text_msg, int status);

  ZWebApiSession* createSession(ZInnerMsg *inner_msg);

  void destroySession(ZSession<uint32_t> *session);

 private:
  // ZSocket server_;
  // char buf_[512 << 10];
  // char out_buf_[512 << 10];
  // evutil_socket_t fd_;

  // std::map<uint32_t, ZWebApiSession*> session_map_;

  // XXX:
  // NO, we shouldn't use sessionctrl, cause HTTP is usually short connecting
  // handler will be destroied very quickly
  // SESSIONCTRL_TYPE session_ctrl_;

  ZWebApiSession *session_;

  // XXX
  ZInnerAddress addr_;
};

#endif // _ZWEBAPI_HANDLER_H__


