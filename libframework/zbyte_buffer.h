#ifndef _Z_BYTE_BUFFER_H__
#define _Z_BYTE_BUFFER_H__

// #include <string>
#include <arpa/inet.h>
#include "zdata_buffer.h"
#include "libbase/ztypes.h"


namespace z {

class ZByteBuffer : public ZDataBuffer<char> {
 public:
  typedef ZDataBuffer<char> super_;

 public:
  ZByteBuffer(int size): ZDataBuffer<char>(size) {}

 public:
  int putInt16(uint16_t v);
  int putInt32(uint32_t v);
  // int putString(const std::string &v);

  int getInt16(uint16_t &v);
  int getInt32(uint32_t &v);
  // int getString(std::string &v);
};

}

#endif // _Z_BYTE_BUFFER_H__

