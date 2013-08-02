#ifndef _Z_SESSION_H__
#define _Z_SESSION_H__

#include "zinner_message.h"
#include "ztime.h"
#include "zlog.h"

#include <stdio.h>

class ZSession {
 public:
 	ZSession(): key_(0), touch_time_(0), timeout_(0) {}
	virtual ~ZSession() {}

 public:
	virtual void doTimeout(long delta) {
		// printf("touch_time_: [%ld]\n", touch_time_);
		// printf("timeout_: [%ld]\n", timeout_);
		// printf("delta: [%ld]\n", delta);
		touch_time_ += delta;
	}

	virtual bool isComplete() {
		if (touch_time_ < timeout_) {
			return false;
		}

		printf("session complete: %p\n", this);
		printf("touch_time_: [%ld], timeout_: [%ld]\n", touch_time_, timeout_);

		return true;
	}

 public:
 	// void touch() { touch_time_ = ZTime::getInMillisecond(); }
 	void touch() { touch_time_ = 0; }
	uint32_t getKey() { return key_; }
	void setKey(uint32_t key) { key_ = key; }
	long getTimeout() { return timeout_; }
	void setTimeout(long timeout) { timeout_ = timeout; }

 private:
	uint32_t key_;
	long touch_time_;
	long timeout_;
};

class ZInnerForwardSession : public ZSession {
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
	ZInnerAddress src_addr_;
};

#endif // _Z_SESSION_H__

