#ifndef _Z_MODULE_H__
#define _Z_MODULE_H__

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
  virtual int sendMsg(ZInnerMsg *msg) = 0;
  virtual int onInnerMsg(ZInnerMsg *msg) = 0;
  virtual void routine(long delta) = 0;

  // // @override from ZTimer::TimerCallback
  // virtual void onTimeout(int handler_id) {}

public:
  module_type_t getType() { return type_; }
  // void setType(int type) { type_ = type; }

private:
  const module_type_t type_;
};

#endif // _Z_MODULE_H__


