#ifndef _Z_EVENT_PROXY_H__
#define _Z_EVENT_PROXY_H__

#include <assert.h>

#include <event2/event.h>


/////////////////////////////////////////////////
// an encapsulation of libevent event
class ZEventProxy {
  typedef void (*callback_t)(evutil_socket_t, short, void*);

 public:
  ZEventProxy(event_base *base, callback_t callback)
    : callback_(callback)
    , base_(base)
    , ev_(NULL)
  {}
  ~ZEventProxy() {
    cancel();
  }

 public:
  void registerSocket(evutil_socket_t fd, short events, void* arg, const struct timeval *timeout) {
    cancel();

    assert(ev_ == NULL);
    ev_ = event_new(base_, fd, events, callback_, arg);
    assert(ev_ != NULL);
    event_add(ev_, timeout);
  }

  // one shot timer
  void registerTimeout(void* arg, const struct timeval *timeout) {
    registerSocket(-1, 0, arg, timeout);
  }

  // repeating timer
  void registerPersistTimeout(void* arg, const struct timeval *timeout) {
    registerSocket(-1, EV_PERSIST, arg, timeout);
  }

  void cancel() {
    if (ev_) {
      event_free(ev_);
      ev_ = NULL;
    }
  }

 private:
  callback_t callback_;
  struct event_base *base_;
  struct event *ev_;
};

#endif // _Z_EVENT_PROXY_H__

