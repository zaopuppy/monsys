#ifndef _WEBAPI_MSG_H__
#define _WEBAPI_MSG_H__

#include <stdint.h>

#include <jansson.h>

#include "libframework/zframework.h"

#include "zinner_message_ex.h"

json_t* inner2Json(ZInnerMsg *innerMsg);

json_t* inner2Json(ZInnerGetDevListReq *msg);
json_t* inner2Json(ZInnerGetDevListRsp *msg);

json_t* inner2Json(ZInnerGetDevInfoReq *msg);
json_t* inner2Json(ZInnerGetDevInfoRsp *msg);

json_t* inner2Json(ZInnerSetDevInfoRsp *msg);
json_t* inner2Json(ZInnerSetDevInfoReq *msg);

json_t* inner2Json(ZInnerPreBindReq *msg);
json_t* inner2Json(ZInnerPreBindRsp *msg);

json_t* inner2Json(ZInnerBindReq *msg);
json_t* inner2Json(ZInnerBindRsp *msg);

// deprecated
ZInnerMsg* decodePushMsg(char *buf, uint32_t buf_len);
json_t* decodeWebApiMsg(char *buf, uint32_t buf_len);
ZInnerMsg* json2Inner(json_t *jroot);

#endif // _WEBAPI_MSG_H__

