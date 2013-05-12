#ifndef _Z_ZIGBEE_HANDLER_H__
#define _Z_ZIGBEE_HANDLER_H__

#include "zhandler.h"

#include "zzigbee_message.h"

class ZZigBeeHandler : public ZHandler {
 public:
	ZZigBeeHandler() {}

 public:
	virtual int init();
	// virtual int event(evutil_socket_t fd, short events);
	virtual int event(char *buf, uint32_t buf_len);
	virtual int onInnerMsg(ZInnerMsg *msg);

 private:
	int processMsg(ZZBRegReq &msg);
	int processMsg(ZZBGetRsp &msg);
	int processMsg(ZZBSetRsp &msg);

 private:
	char buf_[1 << 10];

 public:
	// XXX!
	evutil_socket_t fd_;
};

#endif // _Z_ZIGBEE_HANDLER_H__


