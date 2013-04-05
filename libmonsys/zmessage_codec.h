#ifndef _Z_MESSAGE_CODEC_H__
#define _Z_MESSAGE_CODEC_H__

#include "zmessage.h"
#include "z_codec.h"


#ifdef __cplusplus
extern "C" {
#endif // __cplusplus


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


