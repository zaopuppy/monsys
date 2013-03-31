#include "zzigbee_message.h"

#include <stdio.h>
#include <assert.h>

#include "zmessage_codec.h"

///////////////////////////////////////////////////////////////
ZZigBeeMsg::ZZigBeeMsg():
	syn_(0xFF)
{
}

int ZZigBeeMsg::encode(char* buf, uint32_t buf_len)
{
	int enc_len = getHeaderLen();
	if ((int)buf_len < enc_len) {
		printf("No enough buffer length: %u, %u\n", enc_len, buf_len);
		return -1;
	}

	int rv;
	rv = z_encode_byte(syn_, buf, buf_len);
	if (rv < 0) {
		return rv;
	}
	buf += rv;
	buf_len -= rv;

	rv = z_encode_integer16(len_, buf, buf_len);
	if (rv < 0) {
		return rv;
	}
	buf += rv;
	buf_len -= rv;

	rv = z_encode_byte(cmd_, buf, buf_len);
	if (rv < 0) {
		return rv;
	}
	buf += rv;
	buf_len -= rv;

	return enc_len;
}

int ZZigBeeMsg::decode(char* buf, uint32_t buf_len)
{
	// XXX: assert(buf_len > 3)
	int rv;
	int len = 0;

	rv = z_decode_byte((char*)&syn_, buf, buf_len);
	if (rv < 0) {
		return rv;
	}
	buf += rv;
	buf_len -= rv;
	len += rv;

	rv = z_decode_integer16(&len_, buf, buf_len);
	if (rv < 0) {
		return rv;
	}
	buf += rv;
	buf_len -= rv;
	len += rv;

	rv = z_decode_byte((char*)&cmd_, buf, buf_len);
	buf += rv;
	buf_len -= rv;
	len += rv;

	return len;
}


///////////////////////////////////////////////////////////////

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

