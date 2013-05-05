#ifndef _ZWEBAPI_MODULE_H__
#define _ZWEBAPI_MODULE_H__


#include <map>

#include "ztask.h"

#include <event2/event.h>

#include <jansson.h>

#include "zmodule.h"

#include "zwebapi_session.h"

class ZWebApiModule : public ZTask {
public:
	ZWebApiModule(event_base* base): ZTask(base, Z_MODULE_API) {
	}

	typedef ZTask super_;

public:
	virtual int init();
	virtual void close();
	virtual void event(evutil_socket_t fd, short events);
	virtual void doTimeout();
	virtual bool isComplete();
	virtual int onInnerMsg(ZInnerMsg *msg);

private:
	void onConnected(evutil_socket_t fd, short events);
	int onRead(evutil_socket_t fd, char *buf, uint32_t buf_len);
	void sendRsp(const char *text_msg, int status);
	int processGetDevInfo(json_t *root);

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

// class ZWebApiModule : public ZServerModule {
// public:
// 	ZWebApiModule(const char* ip, uint16_t port, event_base* base)
// 		: ZServerModule(ip, port, base) {
// 	}
// 
// 	typedef ZServerModule super_;
// 
// public:
// 	virtual int init();
// 	// virtual void close();
// 	// virtual void event(evutil_socket_t fd, short events);
// 	// virtual void doTimeout();
// 	// virtual bool isComplete();
// 	virtual int onInnerMsg(ZInnerMsg *msg);
// 
// protected:
// 	virtual void onAccept(evutil_socket_t fd, short events);
// 
// };

#endif // _ZWEBAPI_MODULE_H__



