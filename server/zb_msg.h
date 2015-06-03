#ifndef _ZB_MSG_H__
#define _ZB_MSG_H__

#include "zb_codec.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ver. 1.0 */
#define ZB_VERSION    (1000)

typedef struct {
  char data[8];
} zb_mac_type_t;

/* header */
typedef struct {
  uint8_t ver;
  uint16_t len;
  uint8_t  cmd;
  uint16_t addr;
} zb_header_t;

/* register */
typedef struct {
  zb_header_t hdr;   /* header */
  zb_mac_type_t mac;
  uint16_t dev_type;
  char *name;
  char *desc;
} zb_reg_req_t;

typedef struct {
  zb_header_t hdr;   /* header */
  uint8_t status;
} zb_reg_rsp_t;

/* get */
typedef struct {
  zb_header_t hdr;   /* header */
} zb_get_req_t;

/* set */

#ifdef __cplusplus
}
#endif

#endif // _ZB_MSG_H__

