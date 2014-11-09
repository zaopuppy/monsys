#ifndef _Z_MODULE_H__
#define _Z_MODULE_H__

#include <queue>

#include "zinner_message.h"
#include "ztimer.h"

typedef int module_type_t;
typedef int handler_id_t;

class ZModule {
public:
  ZModule(module_type_t type): type_(type) {}

public:
  virtual int init() = 0;
  virtual void close() = 0;
  virtual int onInnerMsg(ZInnerMsg *msg) = 0;
  virtual void routine(long delta) = 0;

  // // @override from ZTimer::TimerCallback
  // virtual void onTimeout(int handler_id) {}

public:
  int sendMsg(ZInnerMsg *msg) {
    if (msg == NULL) {
      return -1;
    }

    inner_msg_queue_.push(msg);

    return 0;
  }
  void checkMsgQueue() {
    if (inner_msg_queue_.empty()) {
      return;
    }

    // TODO: only check certain number of msg once
    ZInnerMsg *msg = NULL;
    while (!inner_msg_queue_.empty()) {
      msg = inner_msg_queue_.front();
      onInnerMsg(msg);
      delete msg;
      inner_msg_queue_.pop();
    }
  }
  module_type_t getType() { return type_; }
  // void setType(int type) { type_ = type; }

  typedef std::queue<ZInnerMsg*> MSG_QUEUE_TYPE;

private:
  const module_type_t type_;

  MSG_QUEUE_TYPE inner_msg_queue_;
};

#endif // _Z_MODULE_H__


