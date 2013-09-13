#ifndef _Z_ZIGBEE_MESSAGE_H__
#define _Z_ZIGBEE_MESSAGE_H__

#include <stdint.h>
#include <vector>
#include <string>

#include "libbase/zlog.h"
#include "libbase/zcodecpp.h"

#include "zbdefines.h"
#include "zzigbee_codec.h"

class ZZigBeeMsg{
 public:
	ZZigBeeMsg():	ver_(1)
	{
		// syn_.len = 8;
		// syn_.data = new char[8];
		// Z_LOG_D("constructor: &data: %p", syn_.data);
	}
	virtual ~ZZigBeeMsg()
	{
		// Z_LOG_D("destructor: &data: %p", syn_.data);
		// delete []syn_.data;
		// syn_.data = NULL;
	}
	
	virtual int encode(char *buf, uint32_t buf_len);
	virtual int decode(char *buf, uint32_t buf_len);
	
	uint16_t getHeaderLen() {
		return /* getlen(syn_) + */
					 getlen(ver_) +
					 getlen(len_) +
					 getlen(cmd_) +
					 getlen(addr_);
	}
	
 public:
 	static uint32_t getMinimumLen() {
 		return /* 8 + // synchronize bytes */
 	         1 + // ver
 	         2 + // len
 	         1 + // cmd
 	         2;
 	}
	static uint8_t getMsgType(char *buf, uint32_t buf_len) {
		if (buf_len < getMinimumLen()) {
			return 0;
		}

		return (uint8_t)buf[1];
	}

	static uint16_t getMsgLen(char *buf, uint32_t buf_len) {
		if (buf_len < getMinimumLen()) {
			return 0;
		}

		uint16_t len;
		int rv = ::decode(len, buf, buf_len);
		if (rv < 0) {
			return 0;
		}

		return len;
	}

 public:
 	// ZZBHeader hdr_;
 	// fixed_binary_t syn_;
	// uint8_t  syn_;
	uint8_t  ver_;
	uint16_t len_;
	uint8_t  cmd_;
	uint16_t addr_;
};

//////////////////////////////////////////////////////////////////
// REG
class ZZBRegReq : public ZZigBeeMsg {
 public:
	ZZBRegReq();

	typedef ZZigBeeMsg super_;

 public:
	virtual int encode(char* buf, uint32_t buf_len);
	virtual int decode(char* buf, uint32_t buf_len);

 protected:
	uint16_t getEncodeLen() {
		return getHeaderLen()
			+ getBodyLen();
	}
	
	uint16_t getBodyLen() {
		return sizeof(mac_.data)
			+ getlen(dev_type_);
		 	// + getlen(name_)
		 	// + getlen(desc_);
	}

 public:
	// const uint16_t mac_len_;
	// std::string mac_;
	zb_mac_type_t mac_;
	// uint8_t id_count_;	// 1~255
	uint16_t dev_type_;
	std::string name_;
	std::string desc_;
};

class ZZBRegRsp : public ZZigBeeMsg {
 public:
	ZZBRegRsp();

	typedef ZZigBeeMsg super_;

 public:
	virtual int encode(char* buf, uint32_t buf_len);
	virtual int decode(char* buf, uint32_t buf_len);

 protected:
	uint16_t getEncodeLen() {
		return getHeaderLen()
			+ getBodyLen();
	}
	
	uint16_t getBodyLen() {
		return getlen(status_);
	}

 public:
	// uint8_t addr_;
	uint8_t status_;
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
	std::vector<ZItemPair> items_;
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


//////////////////////////////////////////////////////////////////
// Update
class ZZBUpdateIdInfoReq : public ZZigBeeMsg {
 public:
	ZZBUpdateIdInfoReq();

	typedef ZZigBeeMsg super_;

 public:

	virtual int encode(char *buf, uint32_t buf_len);
	virtual int decode(char *buf, uint32_t buf_len);
	
	uint16_t getEncodeLen() {
		return getHeaderLen()
			+ getBodyLen();
	}
	
	uint16_t getBodyLen() {
		return getlen(id_list_);
	}

 public:
	std::vector<zb_item_id_info_t> id_list_;
};

class ZZBUpdateIdInfoRsp : public ZZigBeeMsg {
 public:
	ZZBUpdateIdInfoRsp();

	typedef ZZigBeeMsg super_;

 public:

	virtual int encode(char *buf, uint32_t buf_len);
	virtual int decode(char *buf, uint32_t buf_len);
	
	uint16_t getEncodeLen() {
		return getHeaderLen()
			+ getBodyLen();
	}
	
	uint16_t getBodyLen() {
		return getlen(status_);
	}

 public:
	uint8_t status_;
};



#endif // _Z_ZIGBEE_MESSAGE_H__


