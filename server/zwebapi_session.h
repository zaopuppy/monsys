#ifndef _Z_WEBAPI_SESSION_H__
#define _Z_WEBAPI_SESSION_H__

#include <map>

#include <event2/event.h>

#include <jansson.h>

#include "libframework/zframework.h"

class ZWebApiSession {
 public:
 ZWebApiSession(ZModule *module)
	 : key_(sequence_), state_(STATE_INIT)/*, module_(module)*/ {
		++sequence_;
	}

 public:
 	// save fd to fd_
	int event(evutil_socket_t fd, char *buf, uint32_t buf_len);
	void sendRsp(evutil_socket_t fd, const char *text_msg, int status);
	int processJson(evutil_socket_t fd, json_t *root);
	bool isComplete() { return state_ == STATE_FINISHED; }
	
 public:
	uint32_t key_;

 private:
	enum {
		STATE_INIT,
		STATE_WAIT_FOR_ZB_RSP,
		STATE_FINISHED,
	};
	
 private:
	int state_;
	// ZModule *module_;

 private:
	static uint32_t sequence_;
};


#endif // _Z_WEBAPI_SESSION_H__


