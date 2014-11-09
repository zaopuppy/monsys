#ifndef _FGW_CLIENT_SESSION_H__
#define _FGW_CLIENT_SESSION_H__

#include "libframework/zframework.h"

#include "module.h"
#include "zinner_message_ex.h"
#include "protobuf_convert.h"
#include "protobuf_helper.h"
#include "push.pb.h"
#include "zconfig.h"

using namespace com::letsmidi::monsys::protocol::push;

class FGWClientSession: public ZSession<uint32_t> {
public:
  FGWClientSession(ZHandler *handler, const uint32_t &key)
  : ZSession<uint32_t>(key), handler_(handler)
  {
    setTimeout(FGW_CLIENT_SESSION_TIMEOUT);
  }
  virtual ~FGWClientSession() {}

  typedef ZSession<uint32_t> super;

public:
  virtual bool init() = 0;

  virtual void event(PushMsg *msg) = 0;
  virtual void event(ZInnerMsg *msg) = 0;

protected:
  ZHandler *handler_;
};

class FGWClientForwardSession: public FGWClientSession {
public:
  FGWClientForwardSession(ZHandler *handler, const uint32_t &key)
  : FGWClientSession(handler, key), state_(INIT)
  {
  }
  virtual ~FGWClientForwardSession() {}

  // typedef FGWClientSession super;

  enum State {
    INIT = 0,
    WAIT_FOR_ZB_PROXY,
  };

public:
  virtual void onTimeout() {
    Z_LOG_D("FGWClientForwardSession::onTimeout()");
    super::close();
  }

  virtual bool init() {
    return true;
  }

  virtual void event(PushMsg *msg) {
    Z_LOG_D("FGWClientForwardSession::event()");
    if (state_ != INIT) {
      Z_LOG_D("bad state(%d), INIT is expected", state_);
      return;
    }

    ZInnerMsg *inner_msg = push2inner(*msg);
    if (NULL == inner_msg) {
      Z_LOG_E("Failed to convert push to inner");
      close();
      return;
    }

    external_key_ = msg->sequence();

    inner_msg->seq_ = getKey();

    inner_msg->src_addr_.module_type_ = handler_->getModuleType();
    inner_msg->src_addr_.handler_id_ = handler_->getId();
    inner_msg->dst_addr_.module_type_ = MODULE_SERIAL;
    inner_msg->dst_addr_.handler_id_ = ANY_ID;

    ZDispatcher::instance()->sendDirect(inner_msg);

    state_ = WAIT_FOR_ZB_PROXY;
  }

  virtual void event(ZInnerMsg *msg) {
    Z_LOG_D("FGWClientForwardSession::event()");
    if (state_ != WAIT_FOR_ZB_PROXY) {
      Z_LOG_D("bad state(%d), WAIT_FOR_ZB_PROXY is expected", state_);
      return;
    }

    PushMsg *push_msg = inner2push(*msg);
    if (push_msg == NULL) {
      close();
      return;
    }

    push_msg->set_sequence(external_key_);

    // TODO: single thread only!!!
    static char out_buf[1024];

    int rv = protobuf_encode(push_msg, out_buf, sizeof(out_buf));
    if (rv < 0) {
      Z_LOG_E("Failed to encode push message");
      close();
      return;
    }

    handler_->send(out_buf, rv);

    close();
  }

private:
  int state_;
  uint32_t external_key_;
};


#endif // _FGW_CLIENT_SESSION_H__


