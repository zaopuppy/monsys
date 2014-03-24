#ifndef _FGW_CLIENT_SESSION_H__
#define _FGW_CLIENT_SESSION_H__

#include "libframework/zframework.h"

#include "module.h"
#include "zinner_message_ex.h"
#include "webapi_msg.h"

class FGWClientSession : public ZSession<uint32_t> {
public:
  FGWClientSession(ZHandler* handler)
  : handler_(handler), state_(INIT) {
    setTimeout(1000);
  }

  typedef ZSession super;

  virtual void event(ZInnerMsg *msg) {
    Z_LOG_D("event()");
    if (state_ == INIT) {
      if (msg->msg_type_ == Z_ZB_PRE_BIND_REQ) {
        ZInnerPreBindRsp *rsp = new ZInnerPreBindRsp();
        rsp->seq_ = msg->seq_;
        rsp->result_ = 0x00;

        json_t *jrsp = inner2Json(rsp);

        send(jrsp);

        this->close();
      } else if (msg->msg_type_ == Z_ZB_BIND_REQ) {
        ZInnerPreBindRsp *rsp = new ZInnerPreBindRsp();
        rsp->seq_ = msg->seq_;
        rsp->result_ = 0x00;

        json_t *jrsp = inner2Json(rsp);

        send(jrsp);

        this->close();
      } else {
        msg->src_addr_.module_type_ = handler_->getModuleType();
        msg->src_addr_.handler_id_ = handler_->getId();
        msg->dst_addr_.module_type_ = MODULE_SERIAL;
        msg->dst_addr_.handler_id_ = ANY_ID;
        ZDispatcher::instance()->sendDirect(msg);
        state_ = WAIT_FOR_RSP;
      }
    } else {
      json_t *jmsg = NULL;

      switch (msg->msg_type_) {
        case Z_ZB_GET_DEV_LIST_RSP:
          {
            jmsg = inner2Json((ZInnerGetDevListRsp*)msg);
            break;
          }
        case Z_ZB_GET_DEV_RSP:
          {
            jmsg = inner2Json((ZInnerGetDevInfoRsp*)msg);
            break;
          }
        case Z_ZB_SET_DEV_RSP:
          {
            jmsg = inner2Json((ZInnerSetDevInfoRsp*)msg);
            break;
          }
        default:
          Z_LOG_E("Unknown message type: %d", msg->msg_type_);
      }

      if (!jmsg) {
        Z_LOG_E("Failed to convert ");
      } else {
        uint32_t seq = msg->seq_;
        json_t *jseq = json_integer(seq);
        json_object_set_new(jmsg, "seq", jseq);

        send(jmsg);
      }

      this->close();
    }
  }

  enum {
    INIT = 1,
    WAIT_FOR_RSP,
  };

  int send(json_t *jmsg)
  {
    char *str_dump = json_dumps(jmsg, 0);

    int rv = handler_->send(str_dump, strlen(str_dump));

    Z_LOG_D("Message sent");
    trace_bin(str_dump, strlen(str_dump));

    free(str_dump);

    return rv;
  }
public:
  // ZInnerAddress src_addr_;
  // ZInnerAddress dst_addr_;
  uint32_t webapi_seq;

  ZHandler *handler_;
  int state_;
};

#endif // _FGW_CLIENT_SESSION_H__
