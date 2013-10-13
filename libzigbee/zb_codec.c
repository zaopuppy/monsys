#include "zb_codec.h"

#include <string.h>
#include <stdlib.h>

// string
int
z_encode_string(const char *val, char *buf, uint32_t buf_len)
{
  uint32_t str_len = (val == NULL) ? 0 : strlen(val);
  if (buf_len < (str_len + 1)) {
    return -1;      /* TODO: return ((str_len + 1) - buf_len); */
  }

  memcpy(buf, val, str_len);
  buf += str_len;
  *buf = 0x00;

  return str_len + 1;
}

int
z_decode_string(char **val, char *buf, uint32_t buf_len)
{
  uint32_t str_len;
  const char* p;

  for (p = buf, str_len = 0; (*p != 0x00) && (buf_len > 0); ++p, --buf_len) {
    ++str_len;
  }

  if (*p != 0x00) {
    // reach the end of buffer
    return -1;
  }

  *val = malloc(str_len + 1); // ends with '\0'

  memcpy(*val, buf, str_len);
  
  (*val)[str_len] = 0x00;

  return str_len + 1;
}

uint32_t
z_getlen_string(const char *val)
{
  return (val == NULL) ? 1 : (strlen(val) + 1);
}

// byte(integer8)
int
z_encode_byte(char val, char *buf, uint32_t buf_len)
{
  if (buf_len < 1) {
    return -1;
  }

  *buf = val;

  return 1;
}

int
z_decode_byte(char *val, char *buf, uint32_t buf_len)
{
  if (buf_len < 1) {
    return -1;
  }

  *val = *buf;

  return 1;
}

uint32_t
z_getlen_byte()
{
  return 1;
}

// integer16
int
z_encode_integer16(uint16_t val, char *buf, uint32_t buf_len)
{
  if (buf_len < 2) {
    return -1;
  }

  val = htons(val);

  memcpy(buf, &val, 2);

  return 2;
}

int
z_decode_integer16(uint16_t *val, char *buf, uint32_t buf_len)
{
  if (buf_len < 2) {
    return -1;
  }

  memcpy(val, buf, 2);
  
  *val = ntohs(*val);

  return 2;
}

uint32_t
z_getlen_integer16()
{
  return 2;
}

// integer32
int
z_encode_integer32(uint32_t val, char *buf, uint32_t buf_len)
{
  if (buf_len < 4) {
    return -1;
  }

  val = htonl(val);

  memcpy(buf, &val, 4);

  return 4;
}

int
z_decode_integer32(uint32_t *val, char *buf, uint32_t buf_len)
{
  if (buf_len < 4) {
    return -1;
  }

  memcpy(val, buf, 4);
  
  *val = ntohl(*val);

  return 4;
}

uint32_t
z_getlen_integer32()
{
  return 4;
}

//////////////////////////////////////////////////////////////
#if 0
int
platform_is_big_endian()
{
  unsigned short v = 0x01;

  if (v == *(unsigned char*)(&v)) {
    return 0;
  } else {
    return 1;
  }
}

uint32_t
htonl(uint32_t hostlong)
{
  uint32_t tmpl;
  char *p;
  char tmpc;

  if (platform_is_big_endian()) {
    return hostlong;
  } else {
    tmpl = hostlong;
    char *p = (char*)&tmpl;

    // 0 <--> 3
    tmpc = p[0];
    p[0] = p[3];
    p[3] = tmpc;

    // 1 <-->
    tmpc = p[1];
    p[1] = p[2];
    p[2] = tmpc;

    return tmpl;
  }
}

uint16_t
htons(uint16_t hostshort)
{
  uint16_t tmpl;
  char *p;
  char tmpc;

  if (platform_is_big_endian()) {
    return hostshort;
  } else {
    tmpl = hostshort;
    char *p = (char*)&tmpl;

    // 0 <--> 3
    tmpc = p[0];
    p[0] = p[1];
    p[1] = tmpc;
    return tmpl;
  }
}

uint32_t
ntohl(uint32_t netlong)
{
  return htonl(netlong);
}

uint16_t
ntohs(uint16_t netshort)
{
  return htons(netshort);
}

#endif

