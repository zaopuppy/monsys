#ifndef _SYN_BYTE_MATCHER_H__
#define _SYN_BYTE_MATCHER_H__

#include "string.h"

#include "libbase/ztypes.h"


/**
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

// synchronize word: 'ZZZZZZZZ' ('Z' * 8)
#define ZB_SYN_BYTES "ZZZZZZZZ"
const int ZB_SYN_BYTES_LEN = (int)(sizeof(ZB_SYN_BYTES) - 1);

class SynBytesMatcher {
 public:
  SynBytesMatcher(const char *bytes, int len)
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
  int feed(const char *data, int data_len) {
    if (finished()) {
      return 0;
    }

    int cmp_len = syn_bytes_len_ - idx_;

    if (data_len < cmp_len) {
      cmp_len = data_len;
    }

    for (int i = 0; i < cmp_len; ++i) {
      if (data[i] != syn_bytes_[idx_ + i]) {
        // no need to reset idx_, the data is now totally a mess now
        // just call reset()
        return -1;
      }
    }

    idx_ += cmp_len;

    return cmp_len;
  }

  int feed(char c) {
    if (finished()) {
      return 0;
    }

    if (c != syn_bytes_[idx_]) {
      return -1;
    }

    ++idx_;

    return 1;
  }

  bool finished() {
    return idx_ >= syn_bytes_len_;
  }

  int remaining() {
    return syn_bytes_len_ - idx_;
  }

  int feeded() { return idx_; }

  void reset() {
    idx_ = 0;
  }

  // char charAtIdx(int idx) {
  //   if (idx < 0 || idx >= syn_bytes_len_) {
  //     return 0x00;
  //   }

  //   return syn_bytes_[idx];
  // }

 private:
  char *syn_bytes_;
  const int syn_bytes_len_;
  int idx_;
};


#endif // _SYN_BYTE_MATCHER_H__

