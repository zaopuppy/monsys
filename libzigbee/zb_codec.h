#ifndef _ZB_CODEC_H__
#define _ZB_CODEC_H__

#ifndef WIN
#include <stdint.h>
#else
typedef signed char int8_t;
typedef unsigned int uint8_t;

typedef signed short int16_t;
typedef unsigned short uint16_t;

typedef signed int int32_t;
typedef unsigned int uint32_t;

typedef signed long long int32_t;
typedef unsigned long long uint32_t;
#endif

///////////////////////////////////////////////////////////////
#define ENCODE_WITH_METHOD(_method, _val, _buf, _buf_len, _tmp) \
  do {                \
    _tmp = (_method)((_val), (_buf), (_buf_len));   \
    if (_tmp < 0) {           \
      return _tmp;            \
    }               \
    _buf     += _tmp;           \
    _buf_len -= _tmp;           \
  } while (0)

#define DECODE_WITH_METHOD(_method, _val, _buf, _buf_len, _tmp) \
  do {                \
    _tmp = (_method)((_val), (_buf), (_buf_len));   \
    if (_tmp < 0) {           \
      return _tmp;            \
    }               \
    _buf     += _tmp;           \
    _buf_len -= _tmp;           \
  } while (0)


///////////////////////////////////////////////////////////////
// common encoding functions
// string (c style, ending with '\0')
int z_encode_string(const char *val, char *buf, uint32_t buf_len);
int z_decode_string(char **val, char *buf, uint32_t buf_len);
uint32_t z_getlen_string(const char *val);

// binary string
// int z_encode_bstring(const char *val, char *buf, uint32_t buf_len);
// int z_decode_bstring(char **val, char *buf, uint32_t buf_len);
// uint32_t z_getlen_bstring(const char *val);

// byte(integer8)
int z_encode_byte(char val, char *buf, uint32_t buf_len);
int z_decode_byte(char *val, char *buf, uint32_t buf_len);
uint32_t z_getlen_byte();

// integer16
int z_encode_integer16(uint16_t val, char *buf, uint32_t buf_len);
int z_decode_integer16(uint16_t *val, char *buf, uint32_t buf_len);
uint32_t z_getlen_integer16();

// integer32
int z_encode_integer32(uint32_t val, char *buf, uint32_t buf_len);
int z_decode_integer32(uint32_t *val, char *buf, uint32_t buf_len);
uint32_t z_getlen_integer32();

///////////////////////////////////////////////////////////////
#if 0
int platform_is_big_endian();
uint32_t htonl(uint32_t hostlong);
uint16_t htons(uint16_t hostshort);
uint32_t ntohl(uint32_t netlong);
uint16_t ntohs(uint16_t netshort);
#endif

#endif // _ZB_CODEC_H__

