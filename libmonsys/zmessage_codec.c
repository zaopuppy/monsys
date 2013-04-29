#include "zmessage_codec.h"

#include <arpa/inet.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>


//////////////////////////////////////////////////////////////
// header
int
z_encode_header(struct z_header *val, char *buf, uint32_t buf_len)
{
  uint32_t orig_len = buf_len;
  int rv;

  /* fixed values */
  val->syn[0] = 0xFE;
  val->syn[1] = 0xFD;
  
  ENCODE_WITH_METHOD(z_encode_byte, val->syn[0], buf, buf_len, rv);
  ENCODE_WITH_METHOD(z_encode_byte, val->syn[1], buf, buf_len, rv);
  
  ENCODE_WITH_METHOD(z_encode_integer32, val->len, buf, buf_len, rv);
  ENCODE_WITH_METHOD(z_encode_integer16, val->cmd, buf, buf_len, rv);
  ENCODE_WITH_METHOD(z_encode_integer32, val->seq, buf, buf_len, rv);
  
  return orig_len - buf_len;
}

int
z_decode_header(struct z_header *val, char *buf, uint32_t buf_len)
{
  uint32_t orig_len = buf_len;
  int rv;

  // ------------------------
  /* fixed values */
  DECODE_WITH_METHOD(z_decode_byte, (char*)&(val->syn[0]), buf, buf_len, rv);
  DECODE_WITH_METHOD(z_decode_byte, (char*)&(val->syn[1]), buf, buf_len, rv);

  if (val->syn[0] != 0xFE || val->syn[1] != 0xFD) {
    return -1;
  }
  
  DECODE_WITH_METHOD(z_decode_integer32, (uint32_t*)&val->len, buf, buf_len, rv);
  DECODE_WITH_METHOD(z_decode_integer16, &val->cmd, buf, buf_len, rv);
  DECODE_WITH_METHOD(z_decode_integer32, &val->seq, buf, buf_len, rv);
  
  return orig_len - buf_len;
}

uint32_t
z_getlen_header(const struct z_header *val)
{
  uint32_t len = 0;

  len += z_getlen_byte();
  len += z_getlen_byte();
  len += z_getlen_integer32();
  len += z_getlen_integer16();
  len += z_getlen_integer32();

  return len;
}

// device-info
int
z_encode_dev_info(const struct z_dev_info *val, char *buf, uint32_t buf_len)
{
  uint32_t orig_len = buf_len;
  int rv;

  assert(val != NULL);

  ENCODE_WITH_METHOD(z_encode_integer16, val->id,   buf, buf_len, rv);
  ENCODE_WITH_METHOD(z_encode_integer16, val->stat, buf, buf_len, rv);
  ENCODE_WITH_METHOD(z_encode_string,    val->desc, buf, buf_len, rv);

  return orig_len - buf_len;
}

int
z_decode_dev_info(struct z_dev_info *val, char *buf, uint32_t buf_len)
{
  uint32_t orig_len = buf_len;
  int rv;

  assert(val != NULL);

  DECODE_WITH_METHOD(z_decode_integer16, &val->id,   buf, buf_len, rv);
  DECODE_WITH_METHOD(z_decode_integer16, &val->stat, buf, buf_len, rv);
  DECODE_WITH_METHOD(z_decode_string,    &val->desc, buf, buf_len, rv);

  return orig_len - buf_len;
}

uint32_t
z_getlen_dev_info(const struct z_dev_info *val)
{
  assert(val != NULL);
  
  uint32_t len = 0;

  len += z_getlen_integer16();
  len += z_getlen_integer16();
  len += z_getlen_string(val->desc);

  return len;
}

int
z_encode_dev_info_list(const struct z_dev_info_list *val, char *buf, uint32_t buf_len)
{
  uint32_t orig_len = buf_len;
  int rv, i;

  assert(val != NULL);

  ENCODE_WITH_METHOD(z_encode_integer16, val->count, buf, buf_len, rv);

  for (i = 0; i < val->count; ++i) {
    ENCODE_WITH_METHOD(z_encode_dev_info, &(val->infos[i]), buf, buf_len, rv);
  }

  return orig_len - buf_len;
}

int
z_decode_dev_info_list(struct z_dev_info_list *val, char *buf, uint32_t buf_len)
{
  uint32_t orig_len = buf_len;
  int rv, i;

  assert(val != NULL);

  ENCODE_WITH_METHOD(z_decode_integer16, &val->count, buf, buf_len, rv);

  val->infos = malloc(sizeof(struct z_dev_info) * val->count);
  for (i = 0; i < val->count; ++i) {
    ENCODE_WITH_METHOD(z_decode_dev_info, &(val->infos[i]), buf, buf_len, rv);
  }

  return orig_len - buf_len;
}

uint32_t
z_getlen_dev_info_list(const struct z_dev_info_list *val)
{
  assert(val != NULL);
  
  uint32_t len = 0;
  int i;

  len += z_getlen_integer16();
  for (i = 0; i < val->count; ++i) {
    len += z_getlen_dev_info(&(val->infos[i]));
  }

  return len;
}

//////////////////////////////////////////////////////////////////////
// z_query_dev_req
int
z_encode_query_dev_req(struct z_query_dev_req *val, char *buf, uint32_t buf_len)
{
  uint32_t orig_len = buf_len;
  int rv;

  assert(val != NULL);
  
  val->hdr.len = 0;
  val->hdr.len += z_getlen_string(val->uid);
  val->hdr.len += z_getlen_integer16(val->dev_id);

	val->hdr.cmd = (uint16_t)ID_GET_DEV_REQ;

  ENCODE_WITH_METHOD(z_encode_header,    &(val->hdr), buf, buf_len, rv);
  
  ENCODE_WITH_METHOD(z_encode_string,    val->uid,    buf, buf_len, rv);
  ENCODE_WITH_METHOD(z_encode_integer16, val->dev_id, buf, buf_len, rv);

  return (orig_len - buf_len);
}

int
z_decode_query_dev_req(struct z_query_dev_req *val, char *buf, uint32_t buf_len)
{
  uint32_t orig_len = buf_len;
  int rv;

  assert(val != NULL);

  // val->hdr.len = 0;
  // val->hdr.len += z_getlen_string(val->uid);
  // val->hdr.len += z_getlen_integer16(val->dev_id);

  DECODE_WITH_METHOD(z_decode_header, &(val->hdr), buf, buf_len, rv);
  
  DECODE_WITH_METHOD(z_decode_string, &val->uid, buf, buf_len, rv);
  DECODE_WITH_METHOD(z_decode_integer16, &val->dev_id, buf, buf_len, rv);

  return (orig_len - buf_len);
}

//////////////////////////////////////////////////////////////////////
// z_query_dev_rsp
int
z_encode_query_dev_rsp(struct z_query_dev_rsp *val, char *buf, uint32_t buf_len)
{
  uint32_t orig_len = buf_len;
  int rv;

  // val->hdr.len = 0;
  // val->hdr.len += z_getlen_integer16(val->code);
  // val->hdr.len += z_getlen_string(val->reason);
  // val->hdr.len += z_getlen_dev_info_list(&(val->info_list));

  ENCODE_WITH_METHOD(z_encode_header, &(val->hdr), buf, buf_len, rv);
  
  ENCODE_WITH_METHOD(z_encode_integer16, val->code, buf, buf_len, rv);
  ENCODE_WITH_METHOD(z_encode_string, val->reason, buf, buf_len, rv);
  ENCODE_WITH_METHOD(z_encode_dev_info_list, &(val->info_list), buf, buf_len, rv);

  return (orig_len - buf_len);
}

int
z_decode_query_dev_rsp(struct z_query_dev_rsp *val, char *buf, uint32_t buf_len)
{
  uint32_t orig_len = buf_len;
  int rv;

  // val->hdr.len = 0;
  // val->hdr.len += z_getlen_integer16(val->code);
  // val->hdr.len += z_getlen_string(val->reason);
  // val->hdr.len += z_getlen_dev_info_list(&(val->info_list));

  DECODE_WITH_METHOD(z_decode_header, &(val->hdr), buf, buf_len, rv);
  
  DECODE_WITH_METHOD(z_decode_integer16, &val->code, buf, buf_len, rv);
  DECODE_WITH_METHOD(z_decode_string, &val->reason, buf, buf_len, rv);
  DECODE_WITH_METHOD(z_decode_dev_info_list, &(val->info_list), buf, buf_len, rv);

  return (orig_len - buf_len);
}



