#ifndef _Z_MODULE_H__
#define _Z_MODULE_H__

enum Z_MODULE_TYPE {
	Z_MODULE_UNKNOWN = 0,
	Z_MODULE_SERVER,				// for servers
	Z_MODULE_CLIENT,
	Z_MODULE_ZIGBEE,				// for zigbee modules
	Z_MODULE_API,						// for clients
	Z_MODULE_WEBAPI,				// for http-like request
	Z_MODULE_SERIAL, 				// for serial ports
};

#endif // _Z_MODULE_H__


