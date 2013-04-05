#ifndef _Z_MESSAGE_H__
#define _Z_MESSAGE_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

enum {
	ID_HEARTBEAT = 0xFFFF,
	ID_AUTH_REQ = 0x0001,
	ID_AUTH_RSP = 0x8001,
	ID_GET_ACCOUNT_REQ = 0x0002,
	ID_GET_ACCOUNT_RSP = 0x8002,
	ID_GET_DEV_REQ = 0x0003,
	ID_GET_DEV_RSP = 0x8003,
	ID_GET_DEV_DETAIL_REQ = 0x0004,
	ID_GET_DEV_DETAIL_RSP = 0x8004,
	ID_SET_DEV_REQ = 0x0005,
	ID_SET_DEV_RSP = 0x8005,
};

///////////////////////////////////////
// header
struct z_header {
  uint8_t syn[2];
  int32_t len; 			/* not including header */
  uint16_t cmd;
  uint32_t seq;
};

// device-info
struct z_dev_info {
  uint16_t id;
  uint16_t stat;
  char* desc;
};

struct z_dev_info_list {
  uint16_t count;
  struct z_dev_info *infos;
};

///////////////////////////////////////
struct z_query_dev_req {
  struct z_header hdr;
  
  char* uid;
  uint16_t dev_id;
};

struct z_query_dev_rsp {
  struct z_header hdr;
  
  uint16_t code;
  char* reason;
  struct z_dev_info_list info_list;
};

#ifdef __cplusplus
}
#endif // __cplusplus


#endif // _Z_MESSAGE_H__

