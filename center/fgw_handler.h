#ifndef _FGW_HANDLER_H__
#define _FGW_HANDLER_H__

#include "framework/zframework.h"

class FGWHandler : public ZServerHandler {
public:
	FGWHandler() {}
	~FGWHandler() {}

	typedef ZServerHandler super_;

	virtual int init();
	virtual void close();
	virtual int onRead(char *buf, uint32_t buf_len);
	virtual int onInnerMsg(ZInnerMsg *msg);
	virtual void routine(long delta);

	virtual int send(const char *buf, uint32_t buf_len);
};

#endif // _FGW_HANDLER_H__

