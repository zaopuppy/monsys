#ifndef _Z_MSG_H__
#define _Z_MSG_H__

#include <stdint.h>

#include "zmessage_codec.h"

class ZMsg {
 public:
	virtual int encode(char *buf, uint32_t buf_len) = 0;
	virtual int decode(char *buf, uint32_t buf_len) = 0;
};

// template
template<typename T>
inline int encode(const T &v, char *buf, uint32_t buf_len);
template<typename T>
inline int decode(T &v, char *buf, uint32_t buf_len);

// uint8_t
template<>
inline int encode(const uint8_t &v, char *buf, uint32_t buf_len)
{
	return z_encode_byte(v, buf, buf_len);
}

template<>
inline int decode(uint8_t &v, char *buf, uint32_t buf_len)
{
	return z_decode_byte((char*)&v, buf, buf_len);
}

// uint16_t
template<>
inline int encode(const uint16_t &v, char *buf, uint32_t buf_len)
{
	return z_encode_integer16(v, buf, buf_len);
}

template<>
inline int decode(uint16_t &v, char *buf, uint32_t buf_len)
{
	return z_decode_integer16(&v, buf, buf_len);
}

// uint32_t
template<>
inline int encode(const uint32_t &v, char *buf, uint32_t buf_len)
{
	return z_encode_integer32(v, buf, buf_len);
}

template<>
inline int decode(uint32_t &v, char *buf, uint32_t buf_len)
{
	return z_decode_integer32(&v, buf, buf_len);
}

#endif // _Z_MSG_H__


