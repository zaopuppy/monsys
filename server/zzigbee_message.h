#ifndef _Z_ZIGBEE_MESSAGE_H__
#define _Z_ZIGBEE_MESSAGE_H__

#include <stdint.h>
#include <vector>

const uint8_t Z_ID_ZB_GET_REQ = 0x02;
const uint8_t Z_ID_ZB_GET_RSP = 0x82;
const uint8_t Z_ID_ZB_SET_REQ = 0x03;
const uint8_t Z_ID_ZB_SET_RSP = 0x83;

struct ZItemPair {
	uint8_t id;
	uint16_t val;
};

class ZZigBeeMsg {
 public:
	ZZigBeeMsg();
	
	virtual int encode(char* buf, uint32_t buf_len);
	virtual int decode(char* buf, uint32_t buf_len);
	
	uint16_t getHeaderLen() {
		return 1 + 2 + 1;
	}
	
 public:
	static uint8_t getMsgType(char* buf, uint32_t buf_len) {
		return 0;
	}

 public:
	uint8_t syn_;
	uint16_t len_;
	uint8_t cmd_;
};

//////////////////////////////////////////////////////////////////
// GET
class ZZBGetReq : public ZZigBeeMsg {
 public:
	ZZBGetReq();

	typedef ZZigBeeMsg super_;

 public:
	
	virtual int encode(char* buf, uint32_t buf_len);
	virtual int decode(char* buf, uint32_t buf_len);
	
	uint16_t getEncodeLen() {
		return getHeaderLen()
			+ getBodyLen();
	}
	
	uint16_t getBodyLen() {
		return 1 + items_.size() * 1;
	}

 public:
	std::vector<uint8_t> items_;
};

class ZZBGetRsp : public ZZigBeeMsg {
 public:
	ZZBGetRsp();

	typedef ZZigBeeMsg super_;

 public:

	virtual int encode(char* buf, uint32_t buf_len);
	virtual int decode(char* buf, uint32_t buf_len);
	
	uint16_t getEncodeLen() {
		return getHeaderLen()
			+ getBodyLen();
	}
	
	uint16_t getBodyLen() {
		return 1 + items_.size() * (1 + 2);
	}

 public:
	// uint8_t itemCount_;
	std::vector<struct ZItemPair> items_;
};

//////////////////////////////////////////////////////////////////
// SET
class ZZBSetReq : public ZZigBeeMsg {
 public:
	ZZBSetReq();

	typedef ZZigBeeMsg super_;

 public:
	
	virtual int encode(char* buf, uint32_t buf_len);
	virtual int decode(char* buf, uint32_t buf_len);
	
	uint16_t getEncodeLen() {
		return getHeaderLen()
			+ getBodyLen();
	}
	
	uint16_t getBodyLen() {
		return 1 + items_.size() * (1 + 2);
	}

 public:
	// uint8_t itemCount_;
	std::vector<struct ZItemPair> items_;
};

class ZZBSetRsp : public ZZigBeeMsg {
 public:
	ZZBSetRsp();

	typedef ZZigBeeMsg super_;

 public:

	virtual int encode(char* buf, uint32_t buf_len);
	virtual int decode(char* buf, uint32_t buf_len);

	uint16_t getEncodeLen() {
		return getHeaderLen()
			+ getBodyLen();
	}
	
	uint16_t getBodyLen() {
		return 1;
	}

 public:
	uint8_t status_;
};


#endif // _Z_ZIGBEE_MESSAGE_H__


