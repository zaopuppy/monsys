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
	ZWebApiHandler(event_base* base/*, int moduleType, int handlerId*/)
	// 	: addr_(moduleType, 0, handlerId)
	{
	}

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

	// XXX
	// int event(evutil_socket_t fd, short events);
 private:

	// void onConnected(evutil_socket_t fd, short events);
	// int onRead(evutil_socket_t fd, char *buf, uint32_t buf_len);
	void sendRsp(const char *text_msg, int status);

	// int processGetDevListReq(json_t *root);
	// int processGetDevInfoReq(json_t *root);
	// int processSetDevInfoReq(json_t *root);

	// ZInnerMsg* webMsg2InnerMsg(json_t *web_msg);
	// ZInnerMsg* convertGetDevListReq(json_t *jobj);
	// ZInnerMsg* convertGetDevInfoReq(json_t *jobj);
	// ZInnerMsg* convertSetDevInfoReq(json_t *jobj);

	// int processMsg(ZInnerGetDevListRsp *msg);
	// int processMsg(ZInnerGetDevInfoRsp *msg);
	// int processMsg(ZInnerSetDevInfoRsp *msg);

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


