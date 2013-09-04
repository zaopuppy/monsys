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

class ZMsg {
 public:
	virtual int encode(char* buf, uint32_t buf_len) = 0;
	virtual int decode(char* buf, uint32_t buf_len) = 0;
};

class PushMsg : ZMsg {
public:
	PushMsg(arguments);
	~PushMsg();

private:
	uint32_t
};

#endif // _PUSH_MESSAGE_H__

