#ifndef _MODULE_H__
#define _MODULE_H__

#include "libframework/zframework.h"

enum MODULE_TYPE {
  MODULE_UNKNOWN = 0,
  MODULE_SERVER,
  MODULE_CLIENT,
  MODULE_ZIGBEE,        // for zigbee modules
  MODULE_API,           // for clients
  MODULE_WEBAPI,        // for http-like request
  MODULE_SERIAL,        // for serial ports
  MODULE_FGW_SERVER,
  MODULE_FGW_CLIENT,
};

const char* moduleType2string(int moduleType);

#endif // _MODULE_H__
