#ifndef _Z_MESSAGE_CODEC_H__
#define _Z_MESSAGE_CODEC_H__

#include "zmessage.h"

#define ENCODE_WITH_METHOD(_method, _val, _buf, _buf_len, _tmp)	\
  do {								\
    _tmp = (_method)((_val), (_buf), (_buf_len));		\
    if (_tmp < 0) {						\
      return _tmp;						\
    }								\
    _buf     += _tmp;						\
    _buf_len -= _tmp;						\
  } while (0)

#define DECODE_WITH_METHOD(_method, _val, _buf, _buf_len, _tmp)	\
  do {								\
    _tmp = (_method)((_val), (_buf), (_buf_len));		\
    if (_tmp < 0) {						\
      return _tmp;						\
    }								\
    _buf     += _tmp;						\
    _buf_len -= _tmp;						\
  } while (0)

///////////////////////////////////////////////////////////////
// int z_destroy_query_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

///////////////////////////////////////////////////////////////
// common encoding functions
// string
int z_encode_string(const char *val, char *buf, uint32_t buf_len);
int z_decode_string(char **val, char *buf, uint32_t buf_len);
uint32_t z_getlen_string(const char *val);

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


//////////////////////////////////////////////////////////////
// basic struct
int z_encode_header(struct z_header *val, char *buf, uint32_t buf_len);
int z_decode_header(struct z_header *val, char *buf, uint32_t buf_len);
uint32_t z_getlen_header(const struct z_header *val);

int z_encode_dev_info(const struct z_dev_info *val, char *buf, uint32_t buf_len);
int z_decode_dev_info(struct z_dev_info *val, char *buf, uint32_t buf_len);
uint32_t z_getlen_dev_info(const struct z_dev_info *val);

int z_encode_dev_info_list(const struct z_dev_info_list *val, char *buf, uint32_t buf_len);
int z_decode_dev_info_list(struct z_dev_info_list *val, char *buf, uint32_t buf_len);
uint32_t z_getlen_dev_info_list(const struct z_dev_info_list *val);


//////////////////////////////////////////////////////////////////////
// z_query_dev_req
int z_encode_query_dev_req(struct z_query_dev_req *val, char *buf, uint32_t buf_len);
int z_decode_query_dev_req(struct z_query_dev_req *val, char *buf, uint32_t buf_len);

//////////////////////////////////////////////////////////////////////
// z_query_dev_rsp
int z_encode_query_dev_rsp(struct z_query_dev_rsp *val, char *buf, uint32_t buf_len);
int z_decode_query_dev_rsp(struct z_query_dev_rsp *val, char *buf, uint32_t buf_len);

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

#ifdef __cplusplus
}
#endif // __cplusplus



#endif // _Z_MESSAGE_CODEC_H__


