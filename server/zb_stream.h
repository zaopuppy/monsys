#ifndef _ZB_STREAM_H__
#define _ZB_STREAM_H__

#include "libframework/zframework.h"

/**
 *
 * synchronize word: 'ZZZZZZZZ' ('Z' * 8)
 *
 *
 * example:
 *
 * ZBStream stream_;
 * stream_.feed(buf, buf_len);
 * int rv = stream_.read(buf, buf_len);
 * if (rv > 0) {
 *   // do whatever you want
 * }
**/
class ZBStream : public ZStream
{
 public:
	ZBStream();
  ~ZBStream();

  virtual int read(char *buf, uint32_t buf_len) {
    return 0;
  }
  virtual int write(const char *buf, uint32_t buf_len) {
    return 0;
  }

  int feed(char *buf, uint32_t buf_len) {
    return 0;
  }

 private:
  char buf_[4 << 10]; // default buffer size 4-Kilobytes
};

#endif // _ZB_STREAM_H__

