#include "zmodule.h"

const char* moduleType2string(int moduleType) {
	switch (moduleType) {
		case Z_MODULE_SERVER:
			return "SERVER";
		case Z_MODULE_CLIENT:
			return "CLIENT";
		case Z_MODULE_ZIGBEE:
			return "ZIGBEE";
		case Z_MODULE_API:
			return "API";
		case Z_MODULE_WEBAPI:
			return "WEBAPI";
		case Z_MODULE_SERIAL:
			return "SERIAL";
		default:
			return "UNKNOWN";
	}
}


