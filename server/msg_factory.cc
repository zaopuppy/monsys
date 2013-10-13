#include "msg_factory.h"

// static
ZZigBeeMsg* MsgFactory::getZigBeeMsg(uint8_t type)
{
  ZZigBeeMsg *msg = NULL;

  switch (type) {
    case Z_ID_ZB_REG_REQ:
    {
      msg = new ZZBRegReq();
      break;
    }
    case Z_ID_ZB_REG_RSP:
    {
      msg = new ZZBRegRsp();
      break;
    }
    case Z_ID_ZB_GET_REQ:
    {
      msg = new ZZBGetReq();
      break;
    }
    case Z_ID_ZB_GET_RSP:
    {
      msg = new ZZBGetRsp();
      break;
    }
    case Z_ID_ZB_SET_REQ:
    {
      msg = new ZZBSetReq();
      break;
    }
    case Z_ID_ZB_SET_RSP:
    {
      msg = new ZZBSetRsp();
      break;
    }
    default:
    {
      break;
    }
  }

  return msg;
}

