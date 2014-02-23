#ifndef _Z_SESSION_H__
#define _Z_SESSION_H__

#include "zinner_message.h"
#include "ztime.h"
#include "libbase/zlog.h"

#include <stdio.h>

template <typename T_Key>
class ZSession {
 public:
  ZSession(): key_(0), touch_time_(0), timeout_(0) {}
  virtual ~ZSession() {}

 public:
  virtual void doTimeout(long delta) {
    Z_LOG_D("touch_time_: [%ld]", touch_time_);
    Z_LOG_D("timeout_: [%ld]", timeout_);
    Z_LOG_D("delta: [%ld]", delta);
    touch_time_ += delta;
  }

  virtual bool isComplete() {
    if (touch_time_ < timeout_) {
      return false;
    }

    Z_LOG_D("session complete: %p", this);
    Z_LOG_D("touch_time_: [%ld], timeout_: [%ld]", touch_time_, timeout_);

    return true;
  }

  virtual void event(ZInnerMsg *msg) {
    Z_LOG_D("enter event()");
  }

 public:
  // void touch() { touch_time_ = ZTime::getInMillisecond(); }
  void touch() { touch_time_ = 0; }
  T_Key getKey() { return key_; }
  void setKey(T_Key key) { key_ = key; }
  long getTimeout() { return timeout_; }
  void setTimeout(long timeout) { timeout_ = timeout; }

 private:
  T_Key key_;
  long touch_time_;
  long timeout_;
};

class ZInnerForwardSession : public ZSession<uint32_t> {
 public:
  ZInnerForwardSession() {
    setTimeout(1000);
  }

 public:
  virtual void doTimeout(long delta) {
    super::doTimeout(delta);
  }

  typedef ZSession super;

 public:
  int module_type_;
  int module_id_;
  // ZInnerAddress src_addr_;
};

#endif // _Z_SESSION_H__

