#ifndef _MODULE_H__
#define _MODULE_H__

#include "libframework/zframework.h"

const handler_id_t INVALID_ID = -1;
const handler_id_t ANY_ID = 0;
const handler_id_t BROADCAST_ID = 0xFFFFFF;
const handler_id_t MIN_HANDLER_ID = 0;
const handler_id_t MAX_HANDLER_ID = 0xFFFFF; // don't let it overflow

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

const char* moduleType2string(module_type_t type);

#endif // _MODULE_H__
