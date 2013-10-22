#include "zbyte_buffer.h"

namespace z {

int ZByteBuffer::putInt16(uint16_t v)
{
  if (remaining() < 2) {
    return -1;
  }

  v = htons(v);

  put((const char*)&v, 2);

  return 2;
}

int ZByteBuffer::putInt32(uint32_t v)
{
  if (remaining() < 4) {
    return -1;
  }

  v = htonl(v);

  put((const char*)&v, 4);

  return 4;
}

// // c-style string
// // 2-bytes length field
// int ZByteBuffer::putString(const std::string &v)
// {
//   uint16_t len = (uint16_t)v.size();

//   if (remaining() < len + 2) {
//     return -1;
//   }

//   // length
//   {
//     uint16_t tmp = htons(len);
//     put((const char*)&tmp, 2);
//   }

//   // string
//   put(v.c_str(), len);

//   return len + 2;
// }

int ZByteBuffer::getInt16(uint16_t &v)
{
  if (remaining() < 2) {
    return -1;
  }

  get((char*)&v, 2);

  v = ntohs(v);

  return 2;
}

int ZByteBuffer::getInt32(uint32_t &v)
{
  if (remaining() < 4) {
    return -1;
  }

  get((char*)&v, 4);

  v = ntohl(v);

  return 4;
}

// // c-style string
// // 2-bytes length field
// int ZByteBuffer::getString(std::string &v)
// {
//   if (remaining() < 2) {
//     return -1;
//   }

//   uint16_t len;

//   get((char*)&len, 2);

//   len = ntohs(len);

//   if (remaining() < len) {
//     return -1;
//   }

//   char *raw = getArray();
//   int pos = this->pos();

//   v.assign(raw, pos, len);

//   this->setPos(pos + len);

//   return len + 2;;
// }


}
