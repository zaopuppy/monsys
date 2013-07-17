#ifndef _Z_ZIGBEE_MESSAGE_H__
#define _Z_ZIGBEE_MESSAGE_H__

#include <stdint.h>
#include <vector>
#include <string>

#include "zmsg.h"
#include "zdevice.h"

const uint8_t Z_ID_ZB_REG_REQ = 0x01;
const uint8_t Z_ID_ZB_REG_RSP = 0x81;

const uint8_t Z_ID_ZB_GET_REQ = 0x02;
const uint8_t Z_ID_ZB_GET_RSP = 0x82;

const uint8_t Z_ID_ZB_SET_REQ = 0x03;
const uint8_t Z_ID_ZB_SET_RSP = 0x83;

const uint8_t Z_ID_ZB_UPDATE_ID_REQ = 0x06;
const uint8_t Z_ID_ZB_UPDATE_ID_RSP = 0x86;

struct ZZBHeader {
	uint8_t  syn_;
	uint8_t  ver_;
	uint16_t len_;
	uint8_t  cmd_;
	uint16_t addr_;
};

struct ZItemPair {
	uint8_t id;
	uint16_t val;
};

enum ITEM_ID {
	ITEM_ID_ALL = 0,
	ITEM_ID_DEV_TYPE = 1,
};

typedef union {
	uint16_t int_val;
	char str_val[1];
} zb_dev_item_val_t;

// struct zb_item_id_info_t {
// 	uint8_t id;
// 	std::string name;
// 	std::string desc;
// 	uint8_t type;		// 0: integer
// 	std::string formatter;
// };

///////////////////////////////////////////////////////////////
// zb_item_id_info_t
template<>
inline int encode(const zb_item_id_info_t &v, char *buf, uint32_t buf_len)
{
	int rv;
	int len = 0;

	// id
	rv = encode(v.id_, buf, buf_len);
	if (rv < 0) return rv;
	buf += rv;
	buf_len -= rv;
	len += rv;

	// name
	rv = encode(v.name_, buf, buf_len);
	if (rv < 0) return rv;
	buf += rv;
	buf_len -= rv;
	len += rv;

	// desc
	rv = encode(v.desc_, buf, buf_len);
	if (rv < 0) return rv;
	buf += rv;
	buf_len -= rv;
	len += rv;

	// type
	rv = encode(v.type_, buf, buf_len);
	if (rv < 0) return rv;
	buf += rv;
	buf_len -= rv;
	len += rv;

	// formatter
	rv = encode(v.formatter_, buf, buf_len);
	if (rv < 0) return rv;
	buf += rv;
	buf_len -= rv;
	len += rv;

	return len;
}

template<>
inline int decode(zb_item_id_info_t &v, char *buf, uint32_t buf_len)
{
	int rv;
	int len = 0;

	// id
	rv = decode(v.id_, buf, buf_len);
	if (rv < 0) return rv;
	buf += rv;
	buf_len -= rv;
	len += rv;

	// name
	rv = decode(v.name_, buf, buf_len);
	if (rv < 0) return rv;
	buf += rv;
	buf_len -= rv;
	len += rv;

	// desc
	rv = decode(v.desc_, buf, buf_len);
	if (rv < 0) return rv;
	buf += rv;
	buf_len -= rv;
	len += rv;

	// type
	rv = decode(v.type_, buf, buf_len);
	if (rv < 0) return rv;
	buf += rv;
	buf_len -= rv;
	len += rv;

	// formatter
	rv = decode(v.formatter_, buf, buf_len);
	if (rv < 0) return rv;
	buf += rv;
	buf_len -= rv;
	len += rv;

	return len;
}

template<>
inline int getlen(const zb_item_id_info_t &v)
{
	return getlen(v.id_)
		+ getlen(v.name_)
		+ getlen(v.desc_)
		+ getlen(v.type_)
		+ getlen(v.formatter_);
}

///////////////////////////////////////////////////////////////
// ZZBHeader
template<>
inline int encode(const ZZBHeader &v, char *buf, uint32_t buf_len)
{
	int rv, len = 0;

	rv = encode(v.syn_, buf, buf_len);
	if (rv < 0) return rv;
	buf += rv;
	buf_len -= rv;
	len += rv;

	rv = encode(v.ver_, buf, buf_len);
	if (rv < 0) return rv;
	buf += rv;
	buf_len -= rv;
	len += rv;

	rv = encode(v.len_, buf, buf_len);
	if (rv < 0) return rv;
	buf += rv;
	buf_len -= rv;
	len += rv;

	rv = encode(v.cmd_, buf, buf_len);
	if (rv < 0) return rv;
	buf += rv;
	buf_len -= rv;
	len += rv;

	rv = encode(v.addr_, buf, buf_len);
	if (rv < 0) return rv;
	buf += rv;
	buf_len -= rv;
	len += rv;

	return len;
}

template<>
inline int decode(ZZBHeader &v, char *buf, uint32_t buf_len)
{
	int rv, len = 0;

	rv = decode(v.syn_, buf, buf_len);
	if (rv < 0) return rv;
	buf += rv;
	buf_len -= rv;
	len += rv;

	rv = decode(v.ver_, buf, buf_len);
	if (rv < 0) return rv;
	buf += rv;
	buf_len -= rv;
	len += rv;

	rv = decode(v.len_, buf, buf_len);
	if (rv < 0) return rv;
	buf += rv;
	buf_len -= rv;
	len += rv;

	rv = decode(v.cmd_, buf, buf_len);
	if (rv < 0) return rv;
	buf += rv;
	buf_len -= rv;
	len += rv;

	rv = decode(v.addr_, buf, buf_len);
	if (rv < 0) return rv;
	buf += rv;
	buf_len -= rv;
	len += rv;

	return len;
}

template<>
inline int getlen(const ZZBHeader &v)
{
	return getlen(v.syn_)
		+ getlen(v.ver_)
		+ getlen(v.len_)
		+ getlen(v.cmd_)
		+ getlen(v.addr_);
}

class ZZigBeeMsg : public ZMsg {
 public:
	ZZigBeeMsg();
	
	virtual int encode(char* buf, uint32_t buf_len);
	virtual int decode(char* buf, uint32_t buf_len);
	
	uint16_t getHeaderLen() {
		return getlen(syn_) +
					 getlen(ver_) +
					 getlen(len_) +
					 getlen(cmd_) +
					 getlen(addr_);
	}
	
 public:
	static uint8_t getMsgType(char* buf, uint32_t buf_len) {
		if (buf_len < 4) {
			return 0;
		}

		return (uint8_t)buf[3];
	}

 public:
 	// ZZBHeader hdr_;
	uint8_t  syn_;
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
			+ getlen(dev_type_)
		 	+ getlen(name_)
		 	+ getlen(desc_);
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


