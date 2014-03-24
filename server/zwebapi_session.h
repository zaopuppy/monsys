#ifndef _Z_WEBAPI_SESSION_H__
#define _Z_WEBAPI_SESSION_H__

#include <map>

#include <event2/event.h>

#include <jansson.h>

#include "libframework/zframework.h"

#include "fgw_manager.h"
#include "module.h"
#include "zinner_message_ex.h"
#include "webapi_msg.h"

typedef ZSession<uint32_t> ZWebApiSession;

class ZWebApiForwardSession : public ZSession<uint32_t> {
public:
  ZWebApiForwardSession(ZHandler *handler)
  : handler_(handler), state_(INIT) {}

public:
  virtual void event(ZInnerMsg *msg) {
    Z_LOG_D("ZWebApiForwardSession::event()");

    if (state_ == INIT) {
      int handler_id = getHandlerId(msg);
      if (handler_id == INVALID_ID) {
        this->close();
        return;
      }

      msg->seq_ = handler_->getId();
      msg->src_addr_.module_type_ = handler_->getModuleType();
      msg->src_addr_.handler_id_ = handler_->getId();
      msg->dst_addr_.module_type_ = MODULE_FGW_SERVER;
      msg->dst_addr_.handler_id_ = handler_id;

      state_ = WAIT_FOR_RSP;

      ZDispatcher::instance()->sendDirect(msg);
    } else {
      json_t *web_msg = inner2Json(msg);
      if (web_msg == NULL) {
        Z_LOG_E("inner2Json fail");
        return;
      }

      send(web_msg);
      json_decref(web_msg);

      this->close();
    }
  }

private:
  int send(json_t *jmsg)
  {
    char *str_dump = json_dumps(jmsg, 0);

    int rv = handler_->send(str_dump, strlen(str_dump));

    Z_LOG_D("Message sent");
    trace_bin(str_dump, strlen(str_dump));

    free(str_dump);

    return rv;
  }

  const int getHandlerId(ZInnerMsg *msg) {
    switch (msg->msg_type_) {
      case Z_ZB_GET_DEV_LIST_REQ:
      {
        ZInnerGetDevListReq *m = (ZInnerGetDevListReq*)msg;
        return FGWManager::instance()->find_handler(m->fgw_);
      }
      case Z_ZB_GET_DEV_REQ:
      {
        ZInnerGetDevInfoReq *m = (ZInnerGetDevInfoReq*)msg;
        return FGWManager::instance()->find_handler(m->fgw_);
      }
      case Z_ZB_SET_DEV_REQ:
      {
        ZInnerSetDevInfoReq *m = (ZInnerSetDevInfoReq*)msg;
        return FGWManager::instance()->find_handler(m->fgw_);
      }
      case Z_ZB_PRE_BIND_REQ:
      {
        ZInnerPreBindReq *m = (ZInnerPreBindReq*)msg;
        return FGWManager::instance()->find_handler(m->fgw_);
      }
      default:
      {
        return INVALID_ID;
      }
    }
  }

  enum {
    INIT = 1,
    WAIT_FOR_RSP,
  };

private:
  ZHandler *handler_;
  int state_;
};


#endif // _Z_WEBAPI_SESSION_H__


