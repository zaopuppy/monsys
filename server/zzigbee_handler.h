#ifndef _Z_ZIGBEE_HANDLER_H__
#define _Z_ZIGBEE_HANDLER_H__

#include <stdint.h>
#include <map>
#include <assert.h>
#include <unistd.h>

#include "framework/zframework.h"

#include "zzigbee_message.h"
#include "zinner_message_ex.h"
#include "zmodule_ex.h"
#include "zlog.h"
#include "zdevice.h"

class ZZigBeeHandler : public ZClientHandler {
 public:
 	ZZigBeeHandler()
 	: addr_(Z_MODULE_SERIAL, 0, -1)
 	{
 	}

 public:
	virtual int init();
	virtual void close();
	// virtual int event(evutil_socket_t fd, short events);
	virtual int onRead(char *buf, uint32_t buf_len);
	virtual int onInnerMsg(ZInnerMsg *msg);

	virtual int send(const char *buf, uint32_t buf_len) {
		// for serial device, use write only, don't use send
		if (fd_ <= 0)
		{
			printf("Invalid fd, return\n");
			return 0;
		}

		printf("now sending:\n");
		trace_bin(buf, buf_len);
		return ::write(fd_, buf, buf_len);
	}

	virtual void routine(long delta);

 private:
	int processMsg(ZZBRegReq &msg);
	// int processMsg(ZZBGetReq &msg);
	int processMsg(ZZBGetRsp &msg);
	// int processMsg(ZZBSetReq &msg);
	int processMsg(ZZBSetRsp &msg);
	int processMsg(ZZBUpdateIdInfoReq &msg);

	int processMsg(ZInnerGetDevListReq *msg);
	int processMsg(ZInnerGetDevInfoReq *msg);
	int processMsg(ZInnerSetDevInfoReq *msg);

	// void initZBDevStruct(zb_dev_t *dev);
	// ZZBDev* findDev(uint8_t id);
	// uint8_t getDevAddr();
	void printDevInfo();

	// typedef std::list<zb_dev_t*> DEV_LIST_TYPE;
	typedef ZSessionCtrl2Key<uint32_t, uint32_t, ZSession> SESSION_CTRL_TYPE;

 private:
	char buf_[1 << 10];
	// the first one is not used, cause `0' means `all of them'
	// ZZBDev zb_dev_list_[DEV_LIST_LEN]; // 1 ~ 255
	ZInnerAddress addr_;

	ZDevManager dev_manager_;

	// TODO: they are bound, make it better for management
	SESSION_CTRL_TYPE session_ctrl_;
	// SESSION_CTRL_TYPE session_ctrl_1_;

};

#endif // _Z_ZIGBEE_HANDLER_H__

