#ifndef _Z_WEBAPI_MSG_H__
#define _Z_WEBAPI_MSG_H__

#include <stdint.h>

#include <jansson.h>

class ZWebApiMsg : public ZMsg {
 public:
  ZWebApiMsg(): jobj_;

 public:
  virtual int encode(char *buf, uint32_t buf_len) = 0;
  virtual int decode(char *buf, uint32_t buf_len) = 0;

 private:
  jsong_t *jobj_;
};

#endif // _Z_WEBAPI_MSG_H__
