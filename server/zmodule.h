#ifndef _Z_MODULE_H__
#define _Z_MODULE_H__

#include "zinner_message.h"

typedef int module_type_t;
typedef int handler_id_t;

const handler_id_t INVALID_ID = -1;
const handler_id_t ANY_ID = 0;
const handler_id_t BROADCAST_ID = 0xFFFFFF;
const handler_id_t MIN_HANDLER_ID = 1;
const handler_id_t MAX_HANDLER_ID = 0xFFFFF; // don't let it overflow

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
	virtual void routine(long delta) = 0;
};

#endif // _Z_MODULE_H__


