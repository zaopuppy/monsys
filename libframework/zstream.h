#ifndef _Z_STREAM_H__
#define _Z_STREAM_H__

#include <ztypes.h>

class ZStream
{
 public:
  virtual int read(char *buf, uint32_t buf_len) = 0;
  virtual int write(const char *buf, uint32_t buf_len) = 0;
};


#endif // _ZSTREAM_H__

