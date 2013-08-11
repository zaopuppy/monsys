#ifndef _Z_ZIGBEE_SESSION_H__
#define _Z_ZIGBEE_SESSION_H__

#include <event2/event.h>

#include "module.h"

class ZZigBeeModule : public ZModule {
public:
	ZZigBeeModule(event_base* base): base_(base) {
	}

	typedef ZModule super_;
public:
	virtual int init();
	virtual void close();
	virtual int sendMsg(ZInnerMsg *msg) = 0;
	virtual int onInnerMsg(ZInnerMsg *msg);
	virtual int getType() { return Z_MODULE_ZIGBEE; }

	void event(evutil_socket_t fd, short events);
	virtual int onTimer();

private:
	void onConnected(evutil_socket_t fd, short events);
	int onRead(evutil_socket_t fd, char *buf, uint32_t buf_len);
	
	void processReq(char* buf, uint32_t buf_len);
	void processRsp(char* buf, uint32_t buf_len);
	
private:
	enum STATE {
		STATE_CONNECTED,
		STATE_FINISHED,
	};

// public:
// 	struct event* read_event_;

private:
	event_base *base_;
	STATE state_;
	char buf_[512 << 10];
	char out_buf_[512 << 10];
};


#endif // _Z_ZIGBEE_SESSION_H__


