#ifndef _Z_TIMER_H__
#define _Z_TIMER_H__

#include <assert.h>

#include <event2/event.h>

class ZTimer {
public:
  class TimerCallback {
  public:
    virtual void onTimeout(int handler_id) = 0;
  };

public:
  ZTimer(struct event_base *base, TimerCallback *callback)
  : base_(base), callback_(callback)
  {
    for (int i = 0; i < MAX_TIMER_COUNT; ++i) {
      ev_list_[i] = NULL;
    }
  }
  ~ZTimer() {
    for (int i = 0; i < MAX_TIMER_COUNT; ++i) {
      if (ev_list_[i] != NULL) {
        event_free(ev_list_[i]->ev);
        delete ev_list_[i];
        ev_list_[i] = NULL;
      }
    }
  }

public:
  // set up a timer and return an timer handler
  int setTimer(struct timeval *timeout, bool repeat = false) {
    int handler = getUsedIndex();
    if (handler < 0) {
      return -1;
    }

    assert(ev_list_[handler] == NULL);

    TimerData *timer_data = new TimerData;
    timer_data->timer = this;
    timer_data->repeat = repeat;
    timer_data->id = handler;
    struct event *ev = event_new(base_, -1, 0, timer_callback, timer_data);
    assert(ev != NULL);
    timer_data->ev = ev;

    event_add(ev, timeout);

    ev_list_[handler] = timer_data;

    return handler;
  }

  void cancelTimer(int handler) {
    // check parameter first
    if (handler < 0 || handler >= MAX_TIMER_COUNT) {
      return;
    }

    if (ev_list_[handler] == NULL) {
      return;
    }

    event_free(ev_list_[handler]->ev);
    delete ev_list_[handler];
    ev_list_[handler] = NULL;

  }

  void onTimeout(int handler) {
    callback_->onTimeout(handler);
  }

protected:
  struct TimerData {
    ZTimer *timer;
    bool repeat;
    int id;
    struct event *ev;
  };

protected:
  int getUsedIndex() {
    for (int i = 0; i < MAX_TIMER_COUNT; ++i) {
      if (ev_list_[i] == NULL) {
        return i;
      }
    }

    return -1;
  }

  static void timer_callback(evutil_socket_t fd, short events, void* arg) {
    TimerData *timer_data = (TimerData*)arg;
    ZTimer *timer = timer_data->timer;

    timer->onTimeout(timer_data->id);

    if (!timer_data->repeat) {
      timer->cancelTimer(timer_data->id);
    }

  }

private:
  enum {
    MAX_TIMER_COUNT = 16,
  };

private:
  struct event_base *base_;
  TimerData *ev_list_[MAX_TIMER_COUNT];
  TimerCallback *callback_;
};

#endif // _Z_TIMER_H__

