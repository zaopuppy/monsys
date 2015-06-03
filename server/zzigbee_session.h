#ifndef _Z_ZIGBEE_SESSION_H__
#define _Z_ZIGBEE_SESSION_H__

#include "libframework/zframework.h"
#include "zconfig.h"

class ZZigBeeSession : public ZSession<uint32_t> {
 public:
  ZZigBeeSession(const uint32_t &key): ZSession(key) {
    setTimeout(ZIGBEE_CLIENT_SESSION_TIMEOUT);
  }
  virtual ~ZZigBeeSession() {}

  typedef ZSession super;

  virtual void onTimeout() {
    close();
  }

  // virtual void event(ZInnerMsg *msg) {}
  // virtual void event(ZZigBeeMsg *msg) {}

 public:
  ZInnerAddress src_addr_;
  ZInnerAddress dst_addr_;
};


#endif // _Z_ZIGBEE_SESSION_H__

