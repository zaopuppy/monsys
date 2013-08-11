#ifndef _MODULE_H__
#define _MODULE_H__

#include "libframework/zframework.h"

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

#endif // _MODULE_H__
