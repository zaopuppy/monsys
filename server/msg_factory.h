#ifndef _MSG_FACTORY_H__
#define _MSG_FACTORY_H__

#include "zzigbee_message.h"

class MsgFactory
{
 public:
  static ZZigBeeMsg* getZigBeeMsg(uint8_t type);
};

#endif // _MSG_FACTORY_H__

