#ifndef _Z_MSG_H__
#define _Z_MSG_H__

#include <stdint.h>
#include <iostream>

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
inline int decode(      T &v, char *buf, uint32_t buf_len);
template<typename T>
inline int getlen(const T &v);

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

template<>
inline int getlen(const uint8_t &v)
{
	return 1;
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

template<>
inline int getlen(const uint16_t &v)
{
	return 2;
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

template<>
inline int getlen(const uint32_t &v)
{
	return 4;
}

// c-style string
// 2-bytes length field
// bytes stream
template<>
inline int encode(const std::string &v, char *buf, uint32_t buf_len)
{
	// length header
	uint16_t len = v.size();

	// no enough buffer
	if (buf_len - 1 < len) {
		return -1;
	}

	// string
	memcpy(buf, v.c_str(), len);
	buf[len] = 0x00;

	return len + 1;
}

template<>
inline int decode(std::string &v, char *buf, uint32_t buf_len)
{
	uint16_t len = 0;

	const char *p = buf;
	while (*p && len <= buf_len) {
		++p;
		++len;
	}

	// actually, '=' is enough
	if (len >= buf_len) {
		return -1;
	}

	if (buf_len - 1 < len) {
		return -1;
	}

	v.assign(buf, len);

	return len + 1;
}

template<>
inline int getlen(const std::string &v)
{
	return 1 + v.size();
}

//////////////////////////////////////////////////////////////
// here is a different template
// std::vector
template<typename T>
inline int encode(const std::vector<T> &v, char *buf, uint32_t buf_len)
{
	int rv;
	int len = 0;

	rv = encode((uint8_t)v.size(), buf, buf_len);
	if (rv < 0) {
		return rv;
	}
	buf += rv;
	buf_len -= rv;
	len += rv;

	for (size_t i = 0; i < v.size(); ++i) {
		rv = encode(v[i], buf, buf_len);
		if (rv < 0) {
			return rv;
		}
		buf += rv;
		buf_len -= rv;
		len += rv;
	}

	return len;
}

template<typename T>
inline int decode(std::vector<T> &v, char *buf, uint32_t buf_len)
{
	int rv = 0;
	int len = 0;

	uint8_t list_len;
	rv = decode(list_len, buf, buf_len);
	if (rv < 0) {
		return rv;
	}

	T t;
	v.clear();
	for (uint8_t i = 0; i < list_len; ++i)	{
		rv = decode(t, buf, buf_len);
		if (rv < 0) {
			return rv;
		}
		v.push_back(t);
		buf += rv;
		buf_len -= rv;
		len += rv;
	}

	return 1 + len;
}

template<typename T>
inline int getlen(const std::vector<T> &v)
{
	if (v.size() <= 0) {
		return 1;
	} else {
		return 1 + ((int)v.size() * getlen(v[0]));
	}
}

#endif // _Z_MSG_H__


