#ifndef _Z_WEBAPI_MSG_H__
#define _Z_WEBAPI_MSG_H__

#include <stdint.h>

#include <jansson.h>

class ZWebApiMsg : public ZMsg {
 public:
	virtual int encode(char *buf, uint32_t buf_len) = 0;
	virtual int decode(char *buf, uint32_t buf_len) = 0;

	static json_t* decodeJson(char *buf, uint32_t buf_len);

 private:
};

class ZWAGetDevListReq : public ZWebApiMsg {
 public:
	virtual int encode(char *buf, uint32_t buf_len);
	virtual int decode(char *buf, uint32_t buf_len);

 private:
 	uint32_t uid_;
};

#endif // _Z_WEBAPI_MSG_H__
