#ifndef _Z_DATA_BUFFER_H__
#define _Z_DATA_BUFFER_H__

#include <stdlib.h>
#include <string.h>

///////////////////////////////////////////////////////////////////////
// initial:
//  (pos=0)         
//    +-------------------------+
//   pos = 0;
//   limit = capability = size
//   remaining = limit - pos = size;
//
//  put 1 data
//    (pos = 1)      (limit = capability = size)
//    +-+-----------------------+
//   pos = 1;
//   limit = capability = size
//   remaining = limit - pos = size - 1;
//
template <typename T>
class ZDataBuffer {
 public:
  ZDataBuffer(int size)
    : buf_size_(size), pos_(0), limit_(size)
  {
    buf_ = new T[size];
  }
  ZDataBuffer(const T *data, int size)
    : buf_size_(size), pos_(0), limit_(size)
  {
    buf_ = new T[size];
    memcpy(buf_, data, size);
  }
  ~ZDataBuffer() {
    if (buf_ != NULL) {
      delete []buf_;
      buf_ = NULL;
    }
  }

 public:
  // --------------------------------------------
  T* getArray() { return buf_; }

  int pos() { return pos_; }
  void setPos(int pos) { pos_ = pos; }

  int limit() { return limit_; }
  void setLimit(int limit) { limit_ = limit; }

  int capability() { return buf_size_; }

  bool hasRemaining() { return limit_ > pos_; }
  int remaining() { return limit_ - pos_; }

  // --------------------------------------------
  int put(const T &v) {
    if (!hasRemaining()) {
      return 0;
    }

    buf_[pos_] = v;
    ++pos_;

    return 1;
  }
  int put(const T *v, int len) {
    int remain = remaining();
    if (remain <= 0) {
      // completely empty
      return 0;
    }

    int put_len = remain < len ? remain : len;

    // TODO: POD only!!
    memcpy(buf_ + pos_, v, put_len);
    pos_ += put_len;

    return put_len;
  }
  int put(const T *v, int start, int len) {
    return put(v + start, len);
  }

  // --------------------------------------------
  T get() {
    if (!hasRemaining()) {
      // XXX: damn...I hate exception
      // just give him the last one...
      return buf_[pos_];
    }

    return buf_[pos_++];
  }
  // in Java version of ByteBuffer, get() return 'this'
  int get(T *v, int len) {
    int remain = remaining();
    if (remaining() < 0) {
      return 0;
    }

    int get_len = remain < len ? remain : len;

    memcpy(v, buf_ + pos_, get_len);
    pos_ += get_len;

    return get_len;
  }
  int get(T *v, int start, int len) {
    return get(v + start, len);
  }

  void clear() {
    pos_ = 0;
    limit_ = buf_size_;
  }

  void flip() {
    limit_ = pos_;
    pos_ = 0;
  }

 private:
  T *buf_;
  const int buf_size_;
  int pos_;
  int limit_;
};

#endif // _Z_DATA_BUFFER_H__

