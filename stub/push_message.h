#ifndef _PUSH_MESSAGE_H__
#define _PUSH_MESSAGE_H__

#include <stdint.h>

#include "libframework/zframework.h"

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

ZInnerMsg* decodePushMsg(char *buf, uint32_t buf_len);

#endif // _PUSH_MESSAGE_H__

