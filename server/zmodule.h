#ifndef _Z_MODULE_H__
#define _Z_MODULE_H__

#include "zinner_message.h"

enum Z_MODULE_TYPE {
	Z_MODULE_UNKNOWN = 0,
	Z_MODULE_SERVER,				// for servers
	Z_MODULE_CLIENT,
	Z_MODULE_ZIGBEE,				// for zigbee modules
	Z_MODULE_API,						// for clients
	Z_MODULE_WEBAPI,				// for http-like request
	Z_MODULE_SERIAL, 				// for serial ports
};

const char* moduleType2string(int moduleType);

class ZModule {
 public:
	virtual int init() = 0;
	virtual void close() = 0;
	virtual int sendMsg(ZInnerMsg *msg) = 0;
	virtual int onInnerMsg(ZInnerMsg *msg) = 0;
	virtual int getType() = 0;
};

#endif // _Z_MODULE_H__


