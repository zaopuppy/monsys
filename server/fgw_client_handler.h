#ifndef _FGW_CLIENT_HANDLER_H__
#define _FGW_CLIENT_HANDLER_H__

#include <jansson.h>

#include "libframework/zframework.h"

#include "zgenerator.h"
#include "module.h"
#include "fgw_client_session.h"
#include "push.pb.h"

using namespace com::letsmidi::monsys::protocol::push;

class FGWClientHandler : public ZClientHandler {
public:
  FGWClientHandler(int id, ZModule *module, struct event_base *base)
    : ZClientHandler(id, module, base), state_(STATE_UNREGISTERED)
    , timer_(base, this)
    , login_timer_id(-1)
    , id_generator_(1, 0xFFFFFF)
  {}

public:
  virtual int init();
  virtual void close();
  virtual int onRead(char *buf, uint32_t buf_len);
  virtual int onInnerMsg(ZInnerMsg *msg);
  virtual void routine(long delta);

  virtual int send(const char *buf, uint32_t buf_len);
  virtual int sendJson(json_t *jmsg);

  virtual void onConnected();

  void fgwLogin();

  // override from ZTimer::TimerCallback
  virtual void onTimeout(int handler);

protected:
  int onRead_Unregistered(char *buf, uint32_t buf_len);
  int onRead_WaitForServer(char *buf, uint32_t buf_len);
  int onRead_Registered(char *buf, uint32_t buf_len);
  int processLoginRsp(PushMsg *push_msg);

  void sendRsp(const char *text_msg, int status);

  // FGWClientSession* createSession(ZInnerMsg *inner_msg);

  typedef ZSessionCtrl2Key<uint32_t, uint32_t, FGWClientSession> SESSION_CTRL_TYPE;

  // state-machine
  enum {
    STATE_UNREGISTERED,
    STATE_WAIT_FOR_SERVER,
    STATE_REGISTERED,
  };

  void setState(int new_state);

private:
  ZInnerAddress addr_;

  SESSION_CTRL_TYPE session_ctrl_;


  // state-machine
  int state_;

  ZTimer timer_;
  int login_timer_id;

  char out_buf_[2048];

  HandlerIdGenerator id_generator_;

};

#endif // _FGW_CLIENT_HANDLER_H__

