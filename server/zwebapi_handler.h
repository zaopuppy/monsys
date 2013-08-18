#ifndef _ZWEBAPI_HANDLER_H__
#define _ZWEBAPI_HANDLER_H__

#include <map>

#include <event2/event.h>
#include <jansson.h>

#include "libframework/zframework.h"

// #include "zwebapi_session.h"
#include "zinner_message_ex.h"

// class ZWebApiHandler : public ZHandler {
class ZWebApiHandler : public ZServerHandler {
 public:
	ZWebApiHandler(event_base* base, int id, ZModule *module)
		: ZServerHandler(id, module)
	{}

	typedef ZHandler super_;

 public:
	virtual int init();
	virtual void close();
	virtual int onRead(char *buf, uint32_t buf_len);
	virtual int onInnerMsg(ZInnerMsg *msg);
	virtual void routine(long delta);

	virtual int send(const char *buf, uint32_t buf_len) {
		return ::send(fd_, buf, buf_len, 0);
	}

 private:

	void sendRsp(const char *text_msg, int status);

 private:
	// ZSocket server_;
	char buf_[512 << 10];
	char out_buf_[512 << 10];
	// evutil_socket_t fd_;

	// std::map<uint32_t, ZWebApiSession*> session_map_;

	// XXX
	ZInnerAddress addr_;
};

#endif // _ZWEBAPI_HANDLER_H__


