#include "module.h"

const char* moduleType2string(module_type_t type) {
  switch (type) {
    case MODULE_SERVER:
      return "SERVER";
    case MODULE_CLIENT:
      return "CLIENT";
    case MODULE_ZIGBEE:
      return "ZIGBEE";
    case MODULE_API:
      return "API";
    case MODULE_WEBAPI:
      return "WEBAPI";
    case MODULE_SERIAL:
      return "SERIAL";
    default:
      return "UNKNOWN";
  }
}


