#ifndef _PUSH_CODEC_H__
#define _PUSH_CODEC_H__

#include "libframework/zframework.h"

namespace push {

////////////////////////////////////////////////////////////////
// base
template<typename T>
inline int encode(const T &v, z::ZByteBuffer &buf);

template<typename T>
inline int decode(T &v, z::ZByteBuffer &buf);

template<typename T>
inline int getlen(const T &v, z::ZByteBuffer &buf);

// -------------------------------------------------------------
// uint8
template<>
inline int encode(const uint8_t &v, z::ZByteBuffer &buf)
{
  return buf.put(v);
}

template<>
inline int decode(uint8_t &v, z::ZByteBuffer &buf)
{
  v = buf.get();

  return 1;
}

template<>
inline int getlen(const uint8_t &v, z::ZByteBuffer &buf)
{
  return 1;
}

// -------------------------------------------------------------
// uint16
template<>
inline int encode(const uint16_t &v, z::ZByteBuffer &buf)
{
  return buf.putInt16(v);
}

template<>
inline int decode(uint16_t &v, z::ZByteBuffer &buf)
{
  return buf.getInt16(v);
}

template<>
inline int getlen(const uint16_t &v, z::ZByteBuffer &buf)
{
  return 2;
}

// -------------------------------------------------------------
// uint32
template<>
inline int encode(const uint32_t &v, z::ZByteBuffer &buf)
{
  return buf.putInt32(v);
}

template<>
inline int decode(uint32_t &v, z::ZByteBuffer &buf)
{
  return buf.getInt32(v);
}

template<>
inline int getlen(const uint32_t &v, z::ZByteBuffer &buf)
{
  return 4;
}

// -------------------------------------------------------------
// std::string
// 2-bytes header
template<>
inline int encode(const std::string &v, z::ZByteBuffer &buf)
{
  // return buf.putString(v);
  uint16_t len = (uint16_t)v.size();

  if (buf.remaining() < len + 2) {
    return -1;
  }

  // length, 2-bytes
  buf.putInt16(htons(len));

  // string
  if (len > 0) {
    buf.put(v.c_str(), len);
  }

  return len + 2;
}

template<>
inline int decode(std::string &v, z::ZByteBuffer &buf)
{
  if (buf.remaining() < 2) {
    return -1;
  }

  uint16_t len;

  buf.getInt16(len);

  len = ntohs(len);

  if (len > 0) {
    if (buf.remaining() < len) {
      return -1;
    }

    char *raw = buf.getArray() + buf.pos();
    int pos = buf.pos();

    v.assign(raw, pos, len);

    buf.setPos(pos + len);
  }

  return len + 2;;
}

template<>
inline int getlen(const std::string &v, z::ZByteBuffer &buf)
{
  return 2 + v.size();
}

}

#endif // _PUSH_CODEC_H__

