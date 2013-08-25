#include "zzigbee_message.h"

#include <stdio.h>
#include <assert.h>

#include "libbase/zcodecpp.h"

#define ZMSG_ENCODE_BEGIN() int rv, encode_len = 0
#define ZMSG_ENCODE_END() return encode_len

#define ZMSG_DECODE_BEGIN() int rv, decode_len = 0
#define ZMSG_DECODE_END() return decode_len

#define ZMSG_ENCODE(_field) \
do { \
	rv = ::encode(_field, buf, buf_len); \
	if (rv < 0) { \
		return rv; \
	} \
	buf += rv; \
	buf_len -= rv; \
	encode_len += rv; \
} while (false)

#define ZMSG_DECODE(_field) \
do { \
	rv = ::decode(_field, buf, buf_len); \
	if (rv < 0) { \
		return rv; \
	} \
	buf += rv; \
	buf_len -= rv; \
	decode_len += rv; \
} while (false)

///////////////////////////////////////////////////////////////
ZZigBeeMsg::ZZigBeeMsg():	syn_(0xFF), ver_(1)
{
}

int ZZigBeeMsg::encode(char* buf, uint32_t buf_len)
{
	// int rv;
	// int encode_len = 0;
	ZMSG_ENCODE_BEGIN();

	// IMP
	// child class must call getEncodeLen() to update length field

	ZMSG_ENCODE(syn_);
	ZMSG_ENCODE(ver_);
	ZMSG_ENCODE(len_);
	ZMSG_ENCODE(cmd_);
	ZMSG_ENCODE(addr_);

	ZMSG_ENCODE_END();
}

int ZZigBeeMsg::decode(char* buf, uint32_t buf_len)
{
	// int rv;
	// int decode_len = 0;
	ZMSG_DECODE_BEGIN();

	ZMSG_DECODE(syn_);
	ZMSG_DECODE(ver_);
	ZMSG_DECODE(len_);
	ZMSG_DECODE(cmd_);
	ZMSG_DECODE(addr_);

	ZMSG_DECODE_END();
	// return decode_len;
}

///////////////////////////////////////////////////////////////
// REG req
ZZBRegReq::ZZBRegReq() : ZZigBeeMsg()
{
	cmd_ = Z_ID_ZB_REG_REQ;
	memset(&mac_.data, 0x00, sizeof(mac_.data));
}

int ZZBRegReq::encode(char* buf, uint32_t buf_len)
{
	// update length field
	len_ = getBodyLen();

	int rv = super_::encode(buf, buf_len);
	if (rv < 0) {
		return rv;
	}

	buf += rv;
	buf_len -= rv;

	int encode_len = rv;

	if (sizeof(mac_.data) > buf_len) {
		return -1;
	}

	// encode
	memcpy(buf, mac_.data, sizeof(mac_.data));
	buf += sizeof(mac_.data);
	buf_len -= sizeof(mac_.data);

	encode_len += sizeof(mac_.data);

	ZMSG_ENCODE(dev_type_);
	// ZMSG_ENCODE(name_);
	// ZMSG_ENCODE(desc_);

	return encode_len;
}

int ZZBRegReq::decode(char* buf, uint32_t buf_len)
{
	int decode_len = 0;

	int rv = super_::decode(buf, buf_len);
	if (rv < 0) {
		return rv;
	}

	buf += rv;
	buf_len -= rv;
	decode_len += rv;

	if (buf_len < len_) {
		return -1;
	}

	if (len_ < sizeof(mac_.data)) {
		return -1;
	}

	// mac_.assign(buf, mac_len_);
	memcpy(&mac_.data, buf, sizeof(mac_.data));
	buf += sizeof(mac_.data);
	buf_len -= sizeof(mac_.data);
	decode_len += sizeof(mac_.data);

	ZMSG_DECODE(dev_type_);
	// ZMSG_DECODE(name_);
	// ZMSG_DECODE(desc_);

	return decode_len;
}

///////////////////////////////////////////////////////////////
// REG rsp
ZZBRegRsp::ZZBRegRsp() : ZZigBeeMsg()
{
	cmd_ = Z_ID_ZB_REG_RSP;
}

int ZZBRegRsp::encode(char* buf, uint32_t buf_len)
{
	uint16_t enc_len = getEncodeLen();
	if (buf_len < enc_len) {
		return -1;
	}

	len_ = getBodyLen();

	int rv = super_::encode(buf, buf_len);
	if (rv < 0) {
		return rv;
	}

	buf += rv;
	buf_len -= rv;

	// status_
	rv = z_encode_byte((char)status_, buf, buf_len);
	if (rv < 0) {
		return rv;
	}
	buf += rv;
	buf_len -= rv;

	return enc_len;
}

int ZZBRegRsp::decode(char* buf, uint32_t buf_len)
{
	int len = 0;

	int rv = super_::decode(buf, buf_len);
	if (rv < 0) {
		printf("failed to call super_::decode()\n");
		return rv;
	}

	buf += rv;
	buf_len -= rv;
	len += rv;

	if (buf_len < len_) {
		printf("No enough buffer: %u, %u\n", len_, buf_len);
		return -1;
	}

	// status_
	rv = z_decode_byte((char*)&status_, buf, buf_len);
	if (rv < 0) {
		return rv;
	}
	buf += rv;
	buf_len -= rv;
	len += rv;

	return len;
}


///////////////////////////////////////////////////////////////
// GET
ZZBGetReq::ZZBGetReq():
	ZZigBeeMsg()
{
	cmd_ = Z_ID_ZB_GET_REQ;
}

int ZZBGetReq::encode(char* buf, uint32_t buf_len) {
	printf("ZZBGetReq::encode()\n");

	// check buf length
	int enc_len = getEncodeLen();
	if ((int)buf_len < enc_len) {
		printf("No enough buffer length: %u, %u\n", enc_len, buf_len);
		return -1;
	}

	// update len_ first
	len_ = getBodyLen();
	
	// super::encode()
	int rv = super_::encode(buf, buf_len);
	if (rv < 0) {
		printf("failed to call super_::encode()\n");
		return rv;
	}

	buf += rv;
	buf_len -= rv;

	// itemCount_
	rv = z_encode_byte((char)(items_.size()), buf, buf_len);
	if (rv < 0) {
		return rv;
	}
	buf += rv;
	buf_len -= rv;

	size_t item_count = items_.size();
	for (size_t i = 0; i < item_count; ++i) {
		rv = z_encode_byte((char)items_[i], buf, buf_len);
		if (rv < 0) {
			return rv;
		}
		buf += rv;
		buf_len -= rv;
	}

	return enc_len;
}

int ZZBGetReq::decode(char* buf, uint32_t buf_len) {
	printf("ZZBGetReq::decode()\n");

	// check buf length
	// int enc_len = getEncodeLen();
	// if (buf_len < enc_len) {
	// 	printf("No enough buffer length\n");
	// 	return -1;
	// }

	// super::decode()
	int len = 0;
	
	int rv = super_::decode(buf, buf_len);
	if (rv < 0) {
		printf("failed to call super_::decode()\n");
		return rv;
	}

	buf += rv;
	buf_len -= rv;
	len += rv;

	if (buf_len < len_) {
		printf("No enough buffer: %u, %u\n", len_, buf_len);
		return -1;
	}

	// itemCount_
	uint8_t item_count;
	rv = z_decode_byte((char*)(&item_count), buf, buf_len);
	if (rv < 0) {
		return rv;
	}

	buf += rv;
	buf_len -= rv;
	len += rv;

	for (size_t i = 0; i < item_count; ++i) {
		items_.push_back(*((uint8_t*)buf));
		buf += 1;
		buf_len -= 1;
		len += 1;
	}

	return len;
}

///////////////////////////////////////////////////////////////
// GET rsp
ZZBGetRsp::ZZBGetRsp():
	ZZigBeeMsg()
{
	cmd_ = Z_ID_ZB_GET_RSP;
}

int ZZBGetRsp::encode(char* buf, uint32_t buf_len) {
	printf("ZZBGetRsp::encode()\n");

	// check buf length
	int enc_len = getEncodeLen();
	if ((int)buf_len < enc_len) {
		printf("No enough buffer length: %u, %u\n", enc_len, buf_len);
		return -1;
	}

	// update len_ first
	len_ = getBodyLen();
	
	// super::encode()
	int rv = super_::encode(buf, buf_len);
	if (rv < 0) {
		printf("failed to call super_::encode()\n");
		return rv;
	}

	buf += rv;
	buf_len -= rv;

	// itemCount_
	rv = z_encode_byte((char)(items_.size()), buf, buf_len);
	if (rv < 0) {
		return rv;
	}
	buf += rv;
	buf_len -= rv;

	size_t item_count = items_.size();
	struct ZItemPair itemPair;
	for (size_t i = 0; i < item_count; ++i) {
		itemPair = items_[i];
		// id
		rv = z_encode_byte((char)itemPair.id, buf, buf_len);
		if (rv < 0) {
			return rv;
		}
		buf += rv;
		buf_len -= rv;
		// value
		rv = z_encode_integer16(itemPair.val, buf, buf_len);
		if (rv < 0) {
			return rv;
		}
		buf += rv;
		buf_len -= rv;
	}

	return enc_len;
}

int ZZBGetRsp::decode(char* buf, uint32_t buf_len) {
	printf("ZZBGetRsp::decode()\n");

	// check buf length
	// int enc_len = getEncodeLen();
	// if (buf_len < enc_len) {
	// 	printf("No enough buffer length\n");
	// 	return -1;
	// }

	// super::decode()
	int len = 0;
	
	int rv = super_::decode(buf, buf_len);
	if (rv < 0) {
		printf("failed to call super_::decode()\n");
		return rv;
	}

	buf += rv;
	buf_len -= rv;
	len += rv;

	if (buf_len < len_) {
		printf("No enough buffer: %u, %u\n", len_, buf_len);
		return -1;
	}

	// itemCount_
	uint8_t item_count;
	rv = z_decode_byte((char*)(&item_count), buf, buf_len);
	if (rv < 0) {
		return rv;
	}

	buf += rv;
	buf_len -= rv;
	len += rv;

	struct ZItemPair itemPair;
	for (size_t i = 0; i < item_count; ++i) {
		// id
		rv = z_decode_byte((char*)(&itemPair.id), buf, buf_len);
		if (rv < 0) {
			return rv;
		}
		buf += rv;
		buf_len -= rv;
		len += rv;
		// val
		rv = z_decode_integer16(&itemPair.val, buf, buf_len);
		if (rv < 0) {
			return rv;
		}
		buf += rv;
		buf_len -= rv;
		len += rv;
		// push
		items_.push_back(itemPair);
	}

	return len;
}

///////////////////////////////////////////////////////////////
// SET
ZZBSetReq::ZZBSetReq():
	ZZigBeeMsg()
{
	cmd_ = Z_ID_ZB_SET_REQ;
}

int ZZBSetReq::encode(char* buf, uint32_t buf_len) {
	printf("ZZBSetReq::encode()\n");

	// check buf length
	int enc_len = getEncodeLen();
	if ((int)buf_len < enc_len) {
		printf("No enough buffer length: %u, %u\n", enc_len, buf_len);
		return -1;
	}

	// update len_ first
	len_ = getBodyLen();

	// super::encode()
	int rv = super_::encode(buf, buf_len);
	if (rv < 0) {
		printf("failed to call super_::encode()\n");
		return rv;
	}

	buf += rv;
	buf_len -= rv;

	// itemCount_
	rv = z_encode_byte((char)(items_.size()), buf, buf_len);
	if (rv < 0) {
		return rv;
	}
	buf += rv;
	buf_len -= rv;

	size_t item_count = items_.size();
	struct ZItemPair itemPair;
	for (size_t i = 0; i < item_count; ++i) {
		itemPair = items_[i];
		rv = z_encode_byte((char)itemPair.id, buf, buf_len);
		if (rv < 0) {
			return rv;
		}
		buf += rv;
		buf_len -= rv;
		rv = z_encode_integer16((uint16_t)itemPair.val, buf, buf_len);
		if (rv < 0) {
			return rv;
		}
		buf += rv;
		buf_len -= rv;
	}

	return enc_len;
}

int ZZBSetReq::decode(char* buf, uint32_t buf_len) {
	printf("ZZBSetReq::decode()\n");

	// check buf length
	// int enc_len = getEncodeLen();
	// if (buf_len < enc_len) {
	// 	printf("No enough buffer length\n");
	// 	return -1;
	// }

	// super::decode()
	int len = 0;
	
	int rv = super_::decode(buf, buf_len);
	if (rv < 0) {
		printf("failed to call super_::decode()\n");
		return rv;
	}

	buf += rv;
	buf_len -= rv;
	len += rv;

	if (buf_len < len_) {
		printf("No enough buffer: %u, %u\n", len_, buf_len);
		return -1;
	}

	// itemCount_
	uint8_t item_count;
	rv = z_decode_byte((char*)(&item_count), buf, buf_len);
	if (rv < 0) {
		return rv;
	}

	buf += rv;
	buf_len -= rv;
	len += rv;

	struct ZItemPair itemPair;
	for (size_t i = 0; i < item_count; ++i) {
		// id
		rv = z_decode_byte((char*)(&itemPair.id), buf, buf_len);
		if (rv < 0) {
			return rv;
		}
		buf += rv;
		buf_len -= rv;
		len += rv;
		// val
		rv = z_decode_integer16(&itemPair.val, buf, buf_len);
		if (rv < 0) {
			return rv;
		}
		buf += rv;
		buf_len -= rv;
		len += rv;
		// push
		items_.push_back(itemPair);
	}
	return len;
}

///////////////////////////////////////////////////////////////
// SET rsp
ZZBSetRsp::ZZBSetRsp():
	ZZigBeeMsg()
{
	cmd_ = Z_ID_ZB_SET_RSP;
}

int ZZBSetRsp::encode(char* buf, uint32_t buf_len) {
	printf("ZZBSetRsp::encode()\n");

	// check buf length
	int enc_len = getEncodeLen();
	if ((int)buf_len < enc_len) {
		printf("No enough buffer length: %u, %u\n", enc_len, buf_len);
		return -1;
	}

	// update len_ first
	len_ = getBodyLen();
	
	// super::encode()
	int rv = super_::encode(buf, buf_len);
	if (rv < 0) {
		printf("failed to call super_::encode()\n");
		return rv;
	}

	buf += rv;
	buf_len -= rv;

	// status_
	rv = z_encode_byte((char)status_, buf, buf_len);
	if (rv < 0) {
		return rv;
	}
	buf += rv;
	buf_len -= rv;

	return enc_len;
}

int ZZBSetRsp::decode(char* buf, uint32_t buf_len) {
	printf("ZZBSetRsp::decode()\n");

	// check buf length
	// int enc_len = getEncodeLen();
	// if (buf_len < enc_len) {
	// 	printf("No enough buffer length\n");
	// 	return -1;
	// }

	// super::decode()
	int len = 0;
	
	int rv = super_::decode(buf, buf_len);
	if (rv < 0) {
		printf("failed to call super_::decode()\n");
		return rv;
	}

	buf += rv;
	buf_len -= rv;
	len += rv;

	if (buf_len < len_) {
		printf("No enough buffer: %u, %u\n", len_, buf_len);
		return -1;
	}

	// status_
	rv = z_decode_byte((char*)(&status_), buf, buf_len);
	if (rv < 0) {
		return rv;
	}

	buf += rv;
	buf_len -= rv;
	len += rv;

	return len;
}

//////////////////////////////////////////////////////////////////
// Update
ZZBUpdateIdInfoReq::ZZBUpdateIdInfoReq()
: ZZigBeeMsg()
{
	cmd_ = Z_ID_ZB_UPDATE_ID_REQ;
}

int ZZBUpdateIdInfoReq::encode(char *buf, uint32_t buf_len)
{
	ZMSG_ENCODE_BEGIN();

	// TODO: clean it up
	rv = super_::encode(buf, buf_len);
	if (rv < 0) return rv;
	buf += rv;
	buf_len -= rv;
	encode_len += rv;

	ZMSG_ENCODE(id_list_);

	ZMSG_ENCODE_END();
}

int ZZBUpdateIdInfoReq::decode(char *buf, uint32_t buf_len)
{
	ZMSG_DECODE_BEGIN();

	len_ = getBodyLen();

	// TODO: clean it up
	rv = super_::decode(buf, buf_len);
	if (rv < 0) return rv;
	buf += rv;
	buf_len -= rv;
	decode_len += rv;

	ZMSG_DECODE(id_list_);

	ZMSG_DECODE_END();
}

ZZBUpdateIdInfoRsp::ZZBUpdateIdInfoRsp()
: ZZigBeeMsg()
{
	cmd_ = Z_ID_ZB_UPDATE_ID_RSP;
}

int ZZBUpdateIdInfoRsp::encode(char *buf, uint32_t buf_len)
{
	ZMSG_ENCODE_BEGIN();

	// TODO: clean it up
	rv = super_::encode(buf, buf_len);
	if (rv < 0) return rv;
	buf += rv;
	buf_len -= rv;
	encode_len += rv;
	ZMSG_ENCODE(status_);

	ZMSG_ENCODE_END();
}

int ZZBUpdateIdInfoRsp::decode(char *buf, uint32_t buf_len)
{
	ZMSG_DECODE_BEGIN();

	len_ = getBodyLen();

	// TODO: clean it up
	rv = super_::decode(buf, buf_len);
	if (rv < 0) return rv;
	buf += rv;
	buf_len -= rv;
	decode_len += rv;

	ZMSG_DECODE(status_);

	ZMSG_DECODE_END();
}

