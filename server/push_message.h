#ifndef _PUSH_MESSAGE_H__
#define _PUSH_MESSAGE_H__

#include <stdint.h>

#include <jansson.h>

#include "libframework/zframework.h"

#include "zinner_message_ex.h"

enum emPUSH_MSG_TYPE {
	PUSH_INVALID = -1,
	PUSH_HB = 0,
};

class ZPushHBMsg : public ZInnerMsg {
 public:
 	ZPushHBMsg(): ZInnerMsg(PUSH_HB) {
 		//
 	}
 };

json_t* inner2Json(ZInnerGetDevListRsp *msg);
json_t* inner2Json(ZInnerGetDevInfoRsp *msg);
json_t* inner2Json(ZInnerSetDevInfoRsp *msg);

ZInnerMsg* decodePushMsg(char *buf, uint32_t buf_len);

#endif // _PUSH_MESSAGE_H__

