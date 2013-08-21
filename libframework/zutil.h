#ifndef _Z_UTIL_H__
#define _Z_UTIL_H__

#include <stdint.h>
#include <string.h>
#include <assert.h>

#include <event2/event.h>

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

class CharBuffer {
 public:
 	CharBuffer(uint32_t len): data_() {
 		data_ = new char[len];
 		capacity_ = len;
 		clear();
 	}
 	// wrap
 	CharBuffer(char *data, uint32_t data_len) {
 		data_ = data;
 		capacity_ = data_len;
 		clear();
 	}
 	~CharBuffer() {
 		delete []data_;
 	}

 public:
 	void put(char *data, uint32_t data_len) {
 	}
 	void get(char *data, uint32_t data_len) {
 	}
 	void flip() {
 		limit_ = pos_;
 		pos_ = 0;
 	}
 	void clear() {
 		pos_ = 0;
 		limit_ = capacity_;
 	}
 	bool hasRemaining() {
 		return limit_ > pos_;
 	}

 private:
 	char *data_;

 	uint32_t pos_;
 	uint32_t limit_;
 	uint32_t capacity_;
};




#endif // _Z_UTIL_H__


