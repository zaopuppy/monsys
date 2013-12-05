#ifndef _Z_CLIENT_HANDLER_H__
#define _Z_CLIENT_HANDLER_H__

#include "zhandler.h"

class ZClientHandler : public ZHandler, public ZTimer::TimerCallback {
public:
  ZClientHandler(int id, ZModule *module, struct event_base *base)
    : ZHandler(id, module)
    , fd_(-1), read_event_(NULL)
    , timer_(base, this)
  {}

public:
  virtual void onConnected() = 0;
  virtual void onTimeout(int handler_id) { }

public:
  int setTimer(int interval, bool repeat = false) {
    return timer_.setTimer(interval, repeat);
  }

  void cancelTimer(int id) {
    timer_.cancelTimer(id);
  }

public:
  evutil_socket_t fd_;
  struct event *read_event_;
  ZTimer timer_;
};

#endif // _Z_CLIENT_HANDLER_H__

