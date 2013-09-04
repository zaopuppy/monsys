#ifndef _WEBAPI_MSG_H__
#define _WEBAPI_MSG_H__

#include <stdint.h>

#include <jansson.h>

#include "libframework/zframework.h"

#include "zinner_message_ex.h"

// enum emPUSH_MSG_TYPE {
// 	PUSH_INVALID = -1,
// 	PUSH_HB = 0,
// };

// class ZPushHBMsg : public ZInnerMsg {
//  public:
//  	ZPushHBMsg(): ZInnerMsg(PUSH_HB) {
//  		//
//  	}
//  };

json_t* inner2Json(ZInnerGetDevListRsp *msg);
json_t* inner2Json(ZInnerGetDevInfoRsp *msg);
json_t* inner2Json(ZInnerSetDevInfoRsp *msg);

// deprecated
ZInnerMsg* decodePushMsg(char *buf, uint32_t buf_len);
json_t* decodeWebApiMsg(char *buf, uint32_t buf_len);
ZInnerMsg* json2Inner(json_t *jroot);

#endif // _WEBAPI_MSG_H__

