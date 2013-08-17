##ifndef _Z_TIMER_H__
#define _Z_TIMER_H__

#include <assert.h>

#include <event2/event.h>

class ZTimer {
 public:
 	ZTimer(): type_(ONE_SHOT), ev_(NULL), call_back_(NULL) {}

 public:
 	class CallBack {
 		void onFire() = 0;
 	};

 private:
	static void CALLBACK(evutil_socket_t fd, short events, void *arg) {
		struct StTimerInfo *info = (struct StTimerInfo*)arg;
		assert(info);
		assert(info->timer);
		assert(info->ev);

		info->timer->onFire(info);
	}

 private:
 	struct StTimerInfo {
 		ZTimer *timer;
 		struct event *ev;
 	};

 	enum enType {
 		ONE_SHOT = 1,
 		PERSIST = 2,
 	};

 public:
 	// repeat timer
 	// in second
 	int fire(long interval) {
 		cancel();
 		type_ = PERSIST;

 		const struct timeval timeout_val = { interval, 0 };
 		struct event *timeout_ev = evtimer_new(base_, TIMEOUT_CALLBACK, (void*)this);
		event_add(timeout_ev, &timeout_val);
 	}

 	// one-shot timer
 	int fireOnShot(long interval) {
 		cancel();
 		type_ = ONE_SHOT;
 	}

 	void cancel() {
 		if (ev_) {
 			event_del(ev_);
 			event_free(ev_);
 			ev_ = NULL;
 		}
 	}

 	bool isPending() {
 		return (ev_ != NULL);
 	}

 	void onFire(struct StTimerInfo *info) {
 		assert(info);
 		if (info->ev != ev_) {
 			// canceled event, ignore it
 			// XXX: should we event_del(ev)?
 			return;
 		}


 	}

 private:
 	int type_;
 	struct event *ev_;
 	CallBack call_back_;
};

#endif // _Z_TIMER_H__

