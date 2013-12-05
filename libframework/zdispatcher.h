
#ifndef _Z_DISPATCHER_H__
#define _Z_DISPATCHER_H__

#include <list>

#include "zsession.h"
#include "zsession_ctrl.h"
#include "zmodule.h"

class ZDispatcher : public ZTimer::TimerCallback {
private:
  ZDispatcher(struct event_base *base)
  : routine_interval_(1000)
  , timer_(base, this), timer_routine_id_(-1)
  {
    module_list_.clear();
    timer_routine_id_ = timer_.setTimer(routine_interval_, true);
  }

public:
  static void init(struct event_base *base) {
    static ZDispatcher instance(base);
    instance_ = &instance;
  }
  static ZDispatcher* instance() {
    return instance_;
  }

private:
  static ZDispatcher *instance_;

public:
  int registerModule(ZModule *module);
  int sendMsg(ZInnerMsg *msg);
  int sendDirect(ZInnerMsg *msg);
  void routine(time_t delta);

  // @override from TimerCallback
  virtual void onTimeout(int id) {
    if (timer_routine_id_ == id) {
      routine(routine_interval_);
    }
  }

private:
  ZModule* findModule(int moduleType);

  // void consumeMsg();
  void checkTimeout(long delta);
  void moduleRoutine(long delta);

private:
  typedef std::list<ZModule*> MODULE_LIST_TYPE;
  // typedef ZSessionCtrl<uint32_t, ZSession> SESSION_CTRL_TYPE;

private:
  // TODO: use hash map instead of list
  MODULE_LIST_TYPE module_list_;
  std::list<ZInnerMsg*> msg_list_;
  // SESSION_CTRL_TYPE session_ctrl_;

  int routine_interval_;
  ZTimer timer_;
  int timer_routine_id_;
};

#endif


