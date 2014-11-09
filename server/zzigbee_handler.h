#ifndef _Z_ZIGBEE_HANDLER_H__
#define _Z_ZIGBEE_HANDLER_H__

#include <stdint.h>
#include <map>
#include <assert.h>
#include <unistd.h>

#include "libbase/zlog.h"
#include "libframework/zframework.h"
#include "libzigbee/zzigbee_message.h"
#include "libzigbee/zbdefines.h"
#include "libzigbee/zbdevice_manager.h"

#include "zinner_message_ex.h"
#include "module.h"
#include "zzigbee_session.h"
#include "zgenerator.h"


class ZZigBeeHandler : public ZClientHandler {
 public:
  ZZigBeeHandler(int id, ZModule *module, struct event_base *base)
    : ZClientHandler(id, module, base)
    , addr_(MODULE_SERIAL, 0, -1)
    , id_generator_(1, 0xFFFF)
  {
  }

 public:
  virtual int init();
  virtual void close();
  // virtual int event(evutil_socket_t fd, short events);
  virtual int onRead(char *buf, uint32_t buf_len);
  virtual int onInnerMsg(ZInnerMsg *msg);

  virtual int send(const char *buf, uint32_t buf_len) {
    // for serial device, use write only, don't use send
    if (fd_ <= 0) {
      Z_LOG_D("Invalid fd, return");
      return 0;
    }

    Z_LOG_D("now sending:");
    trace_bin(buf, buf_len);
    return ::write(fd_, buf, buf_len);
  }

  virtual void routine(long delta);

  virtual void onConnected();

 private:
  int processMsg(ZZBRegReq &msg);
  // int processMsg(ZZBGetReq &msg);
  int processMsg(ZZBGetRsp &msg);
  // int processMsg(ZZBSetReq &msg);
  int processMsg(ZZBSetRsp &msg);
  int processMsg(ZZBBindRsp &msg);
  int processMsg(ZZBUpdateIdInfoReq &msg);

  int processMsg(ZInnerGetDevListReq *msg);
  int processMsg(ZInnerGetDevInfoReq *msg);
  int processMsg(ZInnerSetDevInfoReq *msg);
  int processMsg(ZInnerBindReq *msg);

  // <senquence, addr>
  typedef ZSessionCtrl2Key<uint32_t, uint32_t, ZZigBeeSession> SESSION_CTRL_TYPE;

 private:
  char buf_[1 << 10];
  // the first one is not used, cause `0' means `all of them'
  // ZZBDev zb_dev_list_[DEV_LIST_LEN]; // 1 ~ 255
  ZInnerAddress addr_;

  ZZBDevManager dev_manager_;

  // TODO: they are bound, make it better for management
  SESSION_CTRL_TYPE session_ctrl_;
  // SESSION_CTRL_TYPE session_ctrl_1_;

  HandlerIdGenerator id_generator_;

};

#endif // _Z_ZIGBEE_HANDLER_H__

