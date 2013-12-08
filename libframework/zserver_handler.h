#ifndef _Z_SERVER_HANDLER_H__
#define _Z_SERVER_HANDLER_H__

#include <assert.h>

#include <event2/event.h>

#include "zhandler.h"
#include "zevent_proxy.h"

class ZServerHandler : public ZHandler, public ZTimer::TimerCallback {
 public:
  ZServerHandler(int id, evutil_socket_t fd, ZModule *module, struct event_base *base)
    : ZHandler(id, module)
    , read_event_proxy_(base, SOCKET_CALLBACK)
    // , read_event_(NULL)
    , fd_(fd)
    , timer_(base, this)
  {}
  virtual ~ZServerHandler() {}

 public:
  static void SOCKET_CALLBACK(evutil_socket_t fd, short events, void *arg) {
    assert(arg);
    ZServerHandler *h = (ZServerHandler*)arg;
    h->event(fd, events);
  }

 public:
  virtual int init() = 0;
  virtual void close();
  virtual int onRead(char *buf, uint32_t buf_len) = 0;
  virtual int onInnerMsg(ZInnerMsg *msg) = 0;

  // @override from ZTimer::TimerCallback
  virtual void onTimeout(int handler_id) {}

 public:
  virtual int event(evutil_socket_t fd, short events);

  evutil_socket_t getFd() { return fd_; }

  int setTimer(int interval, bool repeat = false) {
    return timer_.setTimer(interval, repeat);
  }

  void cancelTimer(int id) {
    timer_.cancelTimer(id);
  }

 public:
  // TODO: use EventProxy
  // struct event *read_event_;
  ZEventProxy read_event_proxy_;

 private:
  evutil_socket_t fd_;
  char buf_[1 << 10];
  ZTimer timer_;
  };

#endif // _Z_SERVER_HANDLER_H__


