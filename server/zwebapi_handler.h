#ifndef _ZWEBAPI_HANDLER_H__
#define _ZWEBAPI_HANDLER_H__


#include <map>

#include "ztask.h"

#include <event2/event.h>

#include <jansson.h>

#include "zhandler.h"

#include "zwebapi_session.h"

class ZWebApiHandler : public ZHandler {
public:
	ZWebApiHandler(event_base* base) {
	}

	typedef ZHandler super_;

public:
	virtual int init();
	virtual void close();
	virtual int event(char *buf, uint32_t buf_len);
	virtual int onInnerMsg(ZInnerMsg *msg);

	// XXX
	int event(evutil_socket_t fd, short events);
private:

	void onConnected(evutil_socket_t fd, short events);
	int onRead(evutil_socket_t fd, char *buf, uint32_t buf_len);
	void sendRsp(const char *text_msg, int status);
	int processGetDevInfo(json_t *root);
	int processSetDevInfo(json_t *root);

private:
	enum STATE {
		STATE_CONNECTED,
		STATE_FINISHED,
	};


private:
	// ZSocket server_;
	STATE state_;
	char buf_[512 << 10];
	char out_buf_[512 << 10];
	evutil_socket_t fd_;

	std::map<uint32_t, ZWebApiSession*> session_map_;
};

#endif // _ZWEBAPI_HANDLER_H__



