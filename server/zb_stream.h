#ifndef _ZB_STREAM_H__
#define _ZB_STREAM_H__

#include <string.h>

#include "libbase/ztypes.h"
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

#define ZB_SYN_BYTES "ZZZZZZZZ"
const uint32_t ZB_SYN_BYTES_LEN = sizeof("ZZZZZZZZ") - 1;

class SynBytesMatcher {
 public:
  SynBytesMatcher(const char *bytes, uint32_t len)
    : syn_bytes_len_(len), idx_(0)
  {
    syn_bytes_ = new char[len];
    memcpy(syn_bytes_, bytes, len);
  }
  ~SynBytesMatcher() {
    if (syn_bytes_ != NULL) {
      delete []syn_bytes_;
      syn_bytes_ = NULL;
    }
  }

  // return the feeded length.
  // return -1 if error happened, this may caused by non-matched bytes
  int feed(const char *data, uint32_t data_len) {
    if (finished()) {
      return 0;
    }

    uint32_t cmp_len = syn_bytes_len_ - idx_;

    if (data_len < cmp_len) {
      cmp_len = data_len;
    }

    for (uint32_t i = 0; i < cmp_len; ++i) {
      if (data[i] != syn_bytes_[i]) {
        // no need to reset idx_, the data is now totally a mess now
        // just call reset()
        return -1;
      }
    }

    idx_ += cmp_len;

    return cmp_len;
  }

  bool finished() {
    return idx_ >= syn_bytes_len_;
  }

  uint32_t remaining() {
    return syn_bytes_len_ - idx_;
  }

  void reset() {
    idx_ = 0;
  }

 private:
  char *syn_bytes_;
  const uint32_t syn_bytes_len_;
  int idx_;
};

class ZBStream : public ZStream
{
 public:
	// ZBStream(): buf_wrapper_(buf_, sizeof(buf_), false) {}
  ZBStream()
    : state_(STATE_INIT), pos_(0), buf_size_(sizeof(buf_))
  {
  }
  ~ZBStream() {};

  virtual int read(char *buf, uint32_t buf_len) {
    return 0;
  }
  virtual int write(const char *buf, uint32_t buf_len) {
    return 0;
  }

  int feed(char *buf, uint32_t buf_len);

  int doInit(char *buf, uint32_t buf_len);
  int doWaitingForSync(char *buf, uint32_t buf_len);
  int doWaitingForData(char *buf, uint32_t buf_len);

 private:
  enum {
    STATE_INIT,
    STATE_WAITING_FOR_SYNC,
    STATE_WAITING_FOR_DATA,
  };

 private:
  int state_;
  char buf_[4 << 10]; // default buffer size 4-Kilobytes
  uint32_t pos_;
  const uint32_t buf_size_;
  // wrap of buf_
  // ZDataBuffer<char> buf_wrapper_;
};

#endif // _ZB_STREAM_H__

