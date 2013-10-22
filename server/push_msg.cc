#include "push_msg.h"

#include "push_codec.h"

namespace push {

// -------------------------------------------------------------
// TLV
// 2-byte tag
// 2-byte type
template<>
inline int encode(const TLV &v, z::ZByteBuffer &buf)
{
  int before_remain = buf.remaining();

  if (buf.putInt16(v.getTag()) < 0) { return -1; }
  if (buf.putInt16(v.getValue().getType()) < 0) { return -1; }
  switch (v.getValue().getType()) {
    case VData::TYPE_INT8:
      if (buf.put(v.getValue().getByte()) < 0) { return -1; }
      break;
    case VData::TYPE_INT16:
      if (buf.putInt16(v.getValue().getInt16()) < 0) { return -1; }
      break;
    case VData::TYPE_INT32:
      if (buf.put(v.getValue().getInt32()) < 0) { return -1; }
      break;
    case VData::TYPE_STR:
    // {
    //   // XXX: may doesn't work
    //   const VData &vdata = v.getValue();
    //   const char *data = vdata.getData();
    //   int data_len = vdata.getDataLen();
    //   if (buf.put(data, data_len) < 0) { return -1; }
    //   break;
    // }
    // case TLV::TYPE_CSTR:
    {
      const VData &vdata = v.getValue();
      const char *data = vdata.getData();
      uint16_t data_len = vdata.getDataLen();
      // data_len = htons(data_len);
      if (buf.putInt16(data_len) < 0) { return -1; }
      if (buf.put(data, data_len) < 0) { return -1; }
      break;
    }
    default:
      return -1;
  }

  return before_remain - buf.remaining();
}

template<>
inline int decode(TLV &v, z::ZByteBuffer &buf)
{
  int before_remain = buf.remaining();

  uint16_t tag;
  if (buf.getInt16(tag) < 0) { return -1; }
  v.setTag(tag);

  uint16_t type;
  if (buf.getInt16(type) < 0) { return -1; }
  v.getValue().setType(type);

  switch (v.getValue().getType()) {
    case VData::TYPE_INT8:
    {
      if (buf.remaining() < 1) { return -1; }
      uint8_t tmp = buf.get();
      v.getValue().putByte(tmp);
      break;
    }
    case VData::TYPE_INT16:
    {
      if (buf.remaining() < 2) { return -1; }
      uint16_t tmp;
      buf.getInt16(tmp);
      v.getValue().putInt16(tmp);
      break;
    }
    case VData::TYPE_INT32:
    {
      if (buf.remaining() < 4) { return -1; }
      uint32_t tmp;
      buf.getInt32(tmp);
      v.getValue().putInt32(tmp);
      break;
    }
    case VData::TYPE_STR:
    {
      // XXX: may doesn't work
      uint16_t len;
      if (buf.getInt16(len) < 0) { return -1; }
      // len = ntohs(len);
      if (buf.remaining() < len) { return -1; }

      v.getValue().putStr(buf.getArray() + buf.pos(), len);
      buf.setPos(buf.pos() + len);
      break;
    }
    default:
      return -1;
  }

  return before_remain - buf.remaining();
}


/////////////////////////////////////////////////
int Msg::encodeHeader(z::ZByteBuffer &buf)
{
  int remain_before = buf.remaining();

  if (push::encode(header_.seq, buf) < 0) { return -1; }
  if (push::encode(header_.type, buf) < 0) { return -1; }

  return remain_before - buf.remaining();
}

int Msg::decodeHeader(z::ZByteBuffer &buf)
{
  int remain_before = buf.remaining();

  if (push::decode(header_.seq, buf) < 0) { return -1; }
  if (push::decode(header_.type, buf) < 0) { return -1; }

  return remain_before - buf.remaining();
}

/////////////////////////////////////////////////
// Heartbeat
int Heartbeat::encode(z::ZByteBuffer &buf)
{
  int remain_before = buf.remaining();

  if (super_::encode(buf) < 0) { return -1; }

  return remain_before - buf.remaining();
}

int Heartbeat::decode(z::ZByteBuffer &buf)
{
  int remain_before = buf.remaining();

  if (super_::decode(buf) < 0) { return -1; }

  return remain_before - buf.remaining();
}

/////////////////////////////////////////////////
// Get
// request
int GetReq::encode(z::ZByteBuffer &buf)
{
  int remain_before = buf.remaining();

  if (super_::encode(buf) < 0) { return -1; }
  // if (push::encode(id_list_, buf) < 0) { return -1; }

  // id_list_
  int rv;
  uint16_t len = (uint16_t)id_list_.size();
  rv = push::encode(len, buf);
  if (rv < 0) { return -1; }

  uint32_t tmp;
  for (uint16_t i = 0; i < len; ++i) {
    tmp = (uint32_t)id_list_[i];
    rv = push::encode(tmp, buf);
    if (rv < 0) { return -1; }
  }

  return remain_before - buf.remaining();
}

int GetReq::decode(z::ZByteBuffer &buf)
{
  int remain_before = buf.remaining();

  if (super_::decode(buf) < 0) { return -1; }

  // id_list_
  int rv;
  uint16_t len;
  rv = push::decode(len, buf);
  if (rv < 0) { return -1; }

  uint32_t tmp;
  id_list_.clear();
  for (uint16_t i = 0; i < len; ++i) {
    rv = push::decode(tmp, buf);
    if (rv < 0) { return -1; }
    id_list_.push_back((int)tmp);
  }

  return remain_before - buf.remaining();
}

// response
int GetRsp::encode(z::ZByteBuffer &buf)
{
  int remain_before = buf.remaining();

  if (super_::encode(buf) < 0) { return -1; }
  // if (push::encode(id_list_, buf) < 0) { return -1; }

  // id_list_
  int rv;
  uint16_t len = (uint16_t)value_list_.size();
  rv = push::encode(len, buf);
  if (rv < 0) { return -1; }

  TLV *tlv = NULL;
  for (uint16_t i = 0; i < len; ++i) {
    tlv = value_list_[i];
    rv = push::encode(*tlv, buf);
    if (rv < 0) { return -1; }
  }

  return remain_before - buf.remaining();
}

int GetRsp::decode(z::ZByteBuffer &buf)
{
  int remain_before = buf.remaining();

  if (super_::decode(buf) < 0) { return -1; }

  // id_list_
  int rv;
  uint16_t len;
  rv = push::decode(len, buf);
  if (rv < 0) { return -1; }

  TLV *tlv = NULL;
  value_list_.clear();
  for (uint16_t i = 0; i < len; ++i) {
    tlv = new TLV();
    rv = push::decode(*tlv, buf);
    if (rv < 0) { return -1; }
    value_list_.push_back(tlv);
  }

  return remain_before - buf.remaining();
}

/////////////////////////////////////////////////
// Set


}
