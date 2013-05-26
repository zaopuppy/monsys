#ifndef _Z_ZIGBEE_HANDLER_H__
#define _Z_ZIGBEE_HANDLER_H__

#include <stdint.h>
#include <map>

#include "zclient_handler.h"

#include "zzigbee_message.h"
#include "zsession_ctrl.h"
#include "zinner_message.h"
#include "zmodule.h"

typedef struct {
	char mac[8];
	// uint8_t id;
	int state;
	// id-val pair map
	std::map<uint8_t, uint16_t> id_val_map;
} zb_dev_t;

enum zb_dev_state {
	zb_dev_state_invalid = 0,
	zb_dev_state_active,
	zb_dev_state_inactive,
};

const uint8_t DEV_LIST_LEN = 255;

// class ZZBSession {
//  public:
// 	 virtual int event(ZMsg *msg) = 0;
// 
//  public:
// 	ZInnerAddress src_addr_;
// };

// class ZZBInnerGetDevSession : public ZZBSession {
//  public:
// 	virtual int event(ZMsg *msg);
// 
//  public:
// 	void setHandler(ZHandler *handler) {
// 		handler_ = handler;
// 	}
// 
//  private:
// 	ZHandler *handler_;
// };

// class ZZigBeeHandler : public ZHandler {
class ZZigBeeHandler : public ZClientHandler {
 public:
	ZZigBeeHandler(): addr_(Z_MODULE_SERIAL, 0, -1)
	{
		for (uint8_t i = 0; i < DEV_LIST_LEN; ++i) {
			initZBDevStruct(&zb_dev_list_[i]);
		}
	}

 public:
	virtual int init();
	virtual void close();
	// virtual int event(evutil_socket_t fd, short events);
	virtual int onRead(char *buf, uint32_t buf_len);
	virtual int onInnerMsg(ZInnerMsg *msg);

	virtual int send(const char *buf, uint32_t buf_len) {
		// for serial device, use write only, don't use send
		return ::write(fd_, buf, buf_len);
	}

 private:
	int processMsg(ZZBRegReq &msg);
	int processMsg(ZZBGetReq &msg);
	int processMsg(ZZBGetRsp &msg);
	int processMsg(ZZBSetReq &msg);
	int processMsg(ZZBSetRsp &msg);

	int processMsg(ZInnerGetDevInfoReq *msg);

	void initZBDevStruct(zb_dev_t *dev);
	zb_dev_t* findDev(uint8_t id);
	uint8_t genDevAddr();
	void printDevInfo();

	// typedef std::list<zb_dev_t*> DEV_LIST_TYPE;

 private:
	char buf_[1 << 10];
	// the first one is not used, cause `0' means `all of them'
	zb_dev_t zb_dev_list_[DEV_LIST_LEN];	// 1 ~ 255
	// DEV_LIST_TYPE zb_dev_list_;
	// uint8_t zb_dev_id_;
	// ZSessionCtrl<uint8_t, ZZBSession> session_ctrl_;
	ZInnerAddress addr_;
};

#endif // _Z_ZIGBEE_HANDLER_H__


