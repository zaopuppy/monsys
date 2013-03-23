#ifndef _Z_ZIGBEE_MESSAGE_H__
#define _Z_ZIGBEE_MESSAGE_H__

#include <stdint.h>

class CZJCMessage {
 public:
	int encode(char* buf, uint32_t buf_len) = 0;
	int decode(char* buf, uint32_t buf_len) = 0;
	
 public:
	static uint32_t getMsgType(char* buf, uint32_t buf_len) {
	}
};

class CZJCGetFunctionReq : public CZJCMessage {
 public:
	int encode(char* buf, uint32_t buf_len) {
	}
	int decode(char* buf, uint32_t buf_len) {
	}
};

#endif // _Z_ZIGBEE_MESSAGE_H__


