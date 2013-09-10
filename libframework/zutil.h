#ifndef _Z_UTIL_H__
#define _Z_UTIL_H__

#include <stdint.h>
#include <string.h>
#include <assert.h>

#include <event2/event.h>

#include "libbase/ztypes.h"

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

/////////////////////////////////////////////////

bool isBlank(const char *str, uint32_t str_len);

class ZStringUtil
{
 public:
 	static int atoi(const char *str, int def, int begin = 0, int end = -1/*, int base = 10 */);
};

// a,b,c,d
// a=1, b=2, c=3
inline int val(const char *str, int str_len, int &v)
{
	if (str_len <= 0) {
		return -1;
	}

	int idx = 0;

	while (str[idx] >= '0' && str[idx] <= '9' && idx < (int)str_len) {
		++idx;
	}

	--idx;

	v = ZStringUtil::atoi(str, -1, 0, idx);

	return idx + 1;
}

template<typename T_List>
inline bool str2list(const char *str, T_List &list)
{
	int str_len = (int)strlen(str);
	int len = str_len;
	int v;

	int lastpos;

	do {
		lastpos = val(str, str_len, v);
		list.push_back(v);
		lastpos += 1;	// skip ','
		str += lastpos;
		len -= lastpos;
	} while (lastpos > 0 && len > 0);

	if (len > 0) {
		return false;
	}

	return true;
}

///////////////////////////////////////////////////////////////////////
// initial:
//  (pos=0)         
// 		+-------------------------+
//   pos = 0;
//	 limit = capability = size
//   remaining = limit - pos = size;
//
//  put 1 data
//    (pos = 1)      (limit = capability = size)
// 		+-+-----------------------+
//   pos = 1;
//	 limit = capability = size
//   remaining = limit - pos = size - 1;
//
template <typename T>
class ZDataBuffer {
 public:
 	ZDataBuffer(int size)
    : buf_size_(size), release_(true), pos_(0), limit_(size)
  {
 		buf_ = new T[size];
 	}
  ZDataBuffer(const T *data, int size)
    : buf_size_(size), release_(false), pos_(0), limit_(size)
  {
    memcpy(buf_, data, size);
  }
  ZDataBuffer(T *data, int size, bool release)
    : buf_size_(size), release_(true), pos_(0), limit_(size)
  {
    memcpy(buf_, data, size);
  }
  ~ZDataBuffer() {
    if (buf_ != NULL && release_) {
      delete []buf_;
      buf_ = NULL;
    }
  }

 public:
 	int pos() { return pos_; }
 	void setPos(int pos) { pos_ = pos; }

 	int limit() { return limit_; }
 	void setLimit(int limit) { limit_ = limit; }

  int capability() { return buf_size_; }

 	bool hasRemaining() { return limit_ > pos_; }
 	int remaining() { return limit_ - pos_; }

  int put(const T &v) {
  	if (!hasRemaining()) {
  		return 0;
  	}

  	buf_[pos_] = v;
  	++pos_;

  	return 1;
  }
  int put(const T *v, int len) {
  	int remain = remaining();
  	if (remain <= 0) {
  		// completely empty
  		return 0;
  	}

  	int put_len = remain < len ? remain : len;

  	// TODO: POD only!!
  	memcpy(buf_ + pos_, v, put_len);
  	pos_ += put_len;

  	return put_len;
  }
  int put(const T *v, int start, int len) {
  	return put(v + start, len);
  }

  T get() {
  	if (!hasRemaining()) {
  		// XXX: damn...I hate exception
  		// just give him the last one...
  		return buf_[pos_];
  	}

  	return buf_[pos_++];
  }
  // in Java version of ByteBuffer, get() return 'this'
  int get(T *v, int len) {
  	int remain = remaining();
  	if (remaining() < 0) {
  		return 0;
  	}

  	int get_len = remain < len ? remain : len;

  	memcpy(v, buf_ + pos_, get_len);
  	pos_ += get_len;

  	return get_len;
  }
  int get(T *v, int start, int len) {
  	return get(v + start, len);
  }

  void clear() {
    pos_ = 0;
    limit_ = buf_size_;
  }

  void flip() {
    limit_ = pos_;
    pos_ = 0;
  }

 private:
 	T *buf_;
 	const int buf_size_;
  const bool release_;
 	int pos_;
 	int limit_;
};



#endif // _Z_UTIL_H__


