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
      if (buf.putInt32(v.getValue().getInt32()) < 0) { return -1; }
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
// dev_info_t
// typedef struct {
//   int32_t     addr;
//   std::string name;
//   int32_t     state;
//   int32_t     type;
// } dev_info_t;
template<>
inline int encode(const dev_info_t &v, z::ZByteBuffer &buf)
{
  int before_remain = buf.remaining();

  // address
  if (buf.putInt32(v.addr) < 0) { return -1; }
  // string
  // head
  uint16_t len = v.name.size();
  if (buf.putInt16(len) < 0) { return -1; }
  if (buf.put(v.name.c_str(), len) < 0) { return -1; }
  // state
  if (buf.putInt32(v.state) < 0) { return -1; }
  // type
  if (buf.putInt32(v.type) < 0) { return -1; }

  return before_remain - buf.remaining();
}

template<>
inline int decode(dev_info_t &v, z::ZByteBuffer &buf)
{
  int before_remain = buf.remaining();

  // address
  if (buf.getInt32(v.addr) < 0) { return -1; }
  // string
  // head
  uint16_t len;
  if (buf.getInt16(len) < 0) { return -1; }
  if (buf.remaining() < len) { return -1; }
  v.name.assign(buf.getArray() + buf.pos(), len);
  buf.setPos(buf.pos() + len);
  // state
  if (buf.getInt32(v.state) < 0) { return -1; }
  // type
  if (buf.getInt32(v.type) < 0) { return -1; }

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
// GetDevList
int GetDevListReq::encode(z::ZByteBuffer &buf)
{
  int remain_before = buf.remaining();

  if (super_::encode(buf) < 0) { return -1; }

  return remain_before - buf.remaining();
}

int GetDevListReq::decode(z::ZByteBuffer &buf)
{
  int remain_before = buf.remaining();

  if (super_::decode(buf) < 0) { return -1; }

  return remain_before - buf.remaining();
}

int GetDevListRsp::encode(z::ZByteBuffer &buf)
{
  int remain_before = buf.remaining();

  if (super_::encode(buf) < 0) { return -1; }

  // info_list_
  uint16_t len = (uint16_t)info_list_.size();
  if (push::encode(len, buf) < 0) { return -1; }

  dev_info_t *dev_info;
  for (uint16_t i = 0; i < len; ++i) {
    dev_info = info_list_[i];
    if (push::encode(*dev_info, buf) < 0) { return -1; }
  }

  return remain_before - buf.remaining();
}

int GetDevListRsp::decode(z::ZByteBuffer &buf)
{
  int remain_before = buf.remaining();

  if (super_::decode(buf) < 0) { return -1; }

  // info_list_
  uint16_t len;
  if (buf.getInt16(len) < 0) { return -1; }

  // TODO: memory leak
  dev_info_t *dev_info = NULL;
  for (uint16_t i = 0; i < len; ++i) {
    dev_info = new dev_info_t;
    if (push::decode(*dev_info, buf) < 0) {
      delete dev_info;
      dev_info = NULL;
      return -1;
    }
    info_list_.push_back(dev_info);
  }

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
// request
int SetReq::encode(z::ZByteBuffer &buf)
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

int SetReq::decode(z::ZByteBuffer &buf)
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

// response
int SetRsp::encode(z::ZByteBuffer &buf)
{
  int remain_before = buf.remaining();

  if (super_::encode(buf) < 0) { return -1; }
  if (push::encode(status_, buf) < 0) { return -1; }

  return remain_before - buf.remaining();
}

int SetRsp::decode(z::ZByteBuffer &buf)
{
  int remain_before = buf.remaining();

  if (super_::decode(buf) < 0) { return -1; }
  if (push::decode(status_, buf) < 0) { return -1; }

  return remain_before - buf.remaining();
}

}
