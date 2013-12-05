#ifndef _Z_TIMER_H__
#define _Z_TIMER_H__

#include <assert.h>

#include <event2/event.h>

class ZTimer {
public:
  class TimerCallback {
  public:
    virtual void onTimeout(int id) = 0;
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
  // set up a timer and return an timer id
  int setTimer(int interval, bool repeat = false) {
    int id = getAvailableIndex();
    if (id < 0) {
      return -1;
    }

    assert(ev_list_[id] == NULL);

    int sec = interval / 1000;
    int usec = interval % 1000 * 1000;
    struct timeval timeout = {sec, usec};

    TimerData *timer_data = new TimerData;
    timer_data->timer = this;
    timer_data->repeat = repeat;
    timer_data->id = id;
    struct event *ev = event_new(
      base_, -1, (repeat ? EV_PERSIST : 0), timer_callback, timer_data);
    assert(ev != NULL);
    timer_data->ev = ev;

    event_add(ev, &timeout);

    ev_list_[id] = timer_data;

    return id;
  }

  void cancelTimer(int id) {
    // check parameter first
    if (id < 0 || id >= MAX_TIMER_COUNT) {
      return;
    }

    if (ev_list_[id] == NULL) {
      return;
    }

    event_free(ev_list_[id]->ev);
    delete ev_list_[id];
    ev_list_[id] = NULL;

  }

  void onTimeout(int id) {
    callback_->onTimeout(id);
  }

protected:
  struct TimerData {
    ZTimer *timer;
    bool repeat;
    int id;
    struct event *ev;
  };

protected:
  int getAvailableIndex() {
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

