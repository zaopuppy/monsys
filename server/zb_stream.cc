#include "zb_stream.h"

#include "libzigbee/zzigbee_message.h"

int ZBStream::feed(char *buf, uint32_t buf_len)
{
  int rv;
  switch (state_) {
    case STATE_INIT:
    {
      rv = doInit(buf, buf_len);
      break;
    }
    case STATE_WAITING_FOR_SYNC:
    {
      rv = doWaitingForSync(buf, buf_len);
      break;
    }
    case STATE_WAITING_FOR_DATA:
    {
      rv = doWaitingForData(buf, buf_len);
      break;
    }
    default:
    {
      // should never happen!
      assert(false);
      rv = -1;
      break;
    }
  }
  return 0;
}

static ZZigBeeMsg* getMsgByType(uint8_t type)
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

// <init>
// 1. syn bytes is not enough
//    --> <wait_for_syn_bytes>
// 2. length is not enough, and not too big(very long length will block stream for a while)
//    --> <wait_for_data>
// 3. length is not enough, and very big(very long length will block stream for a while)
//    --> discard received bytes, <init>
// 3. length is enough but failed to decode
//    --> skip this message, <init>
int ZBStream::doInit(char *buf, uint32_t buf_len)
{
  Z_LOG_D("ZBStream::doInit()");

  // check syn bytes

  // skip sync bytes
  buf += ZB_SYN_BYTES_LEN;
  buf_len -= ZB_SYN_BYTES_LEN;

  uint8_t type = ZZigBeeMsg::getMsgType(buf, buf_len);
  ZZigBeeMsg *msg = getMsgByType(type);
  if (msg == NULL) {
    Z_LOG_E("Unknown message type: %u, skip", type);
    // skip
    // uint16_t skip_len = ZZigBeeMsg
  } else {
    int rv = msg->decode(buf, buf_len);
    if (rv == NO_ENOUGH_BUFFER) {
      state_ = STATE_WAITING_FOR_DATA;
      return 0;
    } else {
      // skip
    }
  }

  return 0;
}

int ZBStream::doWaitingForSync(char *buf, uint32_t buf_len)
{
  Z_LOG_D("ZBStream::doWaitingForSync()");

  return 0;
}

int ZBStream::doWaitingForData(char *buf, uint32_t buf_len)
{
  Z_LOG_D("ZBStream::doWaitingForData()");

  return 0;
}


