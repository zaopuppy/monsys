#ifndef _PUSH_MESSAGE_H__
#define _PUSH_MESSAGE_H__

#include <stdint.h>

#include <jansson.h>

#include "libframework/zframework.h"

namespace push {

// message types
const uint32_t INVALID    = 0xFFFFFFFF;
const uint32_t HEARTBEAT  = 0x00FF;
const uint32_t REG_REQ    = 0x0001;
const uint32_t REG_RSP    = 0x8001;
const uint32_t GET_REQ    = 0x0002;
const uint32_t GET_RSP    = 0x8002;
const uint32_t SET_REQ    = 0x0003;
const uint32_t SET_RSP    = 0x8003;

typedef struct {
  uint32_t seq;
  uint32_t type;
} header_t;


class VData {
 public:
  VData(): data_(NULL), data_len_(0), alloc_len_(0), type_(TYPE_UNKNOWN) {}
  VData(const VData &other) {
    copy(other);
  }

  ~VData() {
    if (data_) { delete []data_; }
  }

  void copy(const VData &other) {
    if (this == &other) {
      return;
    }

    resize(other.getDataLen());
    memcpy(data_, other.data_, other.getDataLen());
  }

  int8_t getByte() const { return *data_; }
  int16_t getInt16() const { return *(int16_t*)data_; }
  int32_t getInt32() const { return *(int32_t*)data_; }
  // never assume string will be endup with '\0'!
  const char* getData() const { return data_; }
  int32_t getDataLen() const { return data_len_; }
  int32_t getAllocLen() const { return alloc_len_; }

  void putByte(int8_t v) {
    resize(1);

    type_ = TYPE_INT8;

    *data_ = v;
  }

  void putInt16(int16_t v) {
    resize(2);

    type_ = TYPE_INT16;

    *(int16_t*)data_ = v;
  }

  void putInt32(int32_t v) {
    resize(4);

    type_ = TYPE_INT32;

    *(int32_t*)data_ = v;
  }

  // binary string, no '\0' will be appended
  void putStr(const char *str, int32_t len) {
    resize(len);

    type_ = TYPE_STR;

    memcpy(data_, str, len);
  }

  // // deprecated 
  // // set C-Style string, append '\0' automatically
  // void putCStr(const char *str, int32_t len) {
  //   resize(len + 1);

  //   memcpy(data_, str, len);
  //   data_[len] = 0x00;
  // }
  int getType() const { return type_; }
  void setType(int type) { type_ = type; }

  enum {
    TYPE_UNKNOWN = 0,
    TYPE_INT8,
    TYPE_INT16,
    TYPE_INT32,
    TYPE_STR,
    // TYPE_CSTR,
  };

 protected:
  // forbid it
  VData& operator=(const VData &other) {
    assert(false);
  }

  // re-allocate memory only, no data copy will occur
  void resize(int size) {
    // update data_len first
    data_len_ = size;

    if (data_ && alloc_len_ >= size) {
      return;
    }

    if (data_) {
      delete []data_;
      data_ = NULL;
    }

    // adjust size align to 4 bytes, for better memory management
    // F = fun(X) -> ((X + 3) bsr 2) bsl 2 end.
    // io:format("0x~2.16.0B~n", [ F(3) ]).
    alloc_len_ = ((size + 3) >> 2) << 2;
    data_ = new char[alloc_len_];
  }

  // int32_t align(int num, int size) {
  //   (num + (size - 1)) >> 2 << 2
  // }

 private:
  char *data_;
  int32_t data_len_;
  int32_t alloc_len_;
  int type_;
};

// template <typename T>
class TLV {
 public:
  TLV(): tag_(0x00) {}

  uint16_t getTag() const { return tag_; }
  void setTag(uint16_t tag) { tag_ = tag; }

  VData& getValue() { return value_; }
  const VData& getValue() const { return value_; }
  void setValue(VData &value) { value_.copy(value); }

 private:
  uint16_t tag_;
  VData value_;
};

class Msg {
 public:
  Msg(uint32_t type) {
    header_.type = type;
  }

  virtual int encode(z::ZByteBuffer &buf) { return encodeHeader(buf); }
  virtual int decode(z::ZByteBuffer &buf) { return decodeHeader(buf); }

  int encodeHeader(z::ZByteBuffer &buf);
  int decodeHeader(z::ZByteBuffer &buf);

 public:
  header_t header_;
};

class Heartbeat : public Msg {
 public:
  Heartbeat(): Msg(HEARTBEAT) {
  }

  typedef Msg super_;

  virtual int encode(z::ZByteBuffer &buf);
  virtual int decode(z::ZByteBuffer &buf);

 public:
};

class GetReq : public Msg {
 public:
  GetReq(): Msg(GET_REQ) {
  }

  typedef Msg super_;

  virtual int encode(z::ZByteBuffer &buf);
  virtual int decode(z::ZByteBuffer &buf);

 public:
  std::vector<int> id_list_;
};

class GetRsp : public Msg {
 public:
  GetRsp(): Msg(GET_RSP) {
  }

  typedef Msg super_;

  virtual int encode(z::ZByteBuffer &buf);
  virtual int decode(z::ZByteBuffer &buf);

 public:
  std::vector<TLV*> value_list_;
};

class SetReq : public Msg {
 public:
  SetReq(): Msg(GET_REQ) {
  }

  typedef Msg super_;

  virtual int encode(z::ZByteBuffer &buf);
  virtual int decode(z::ZByteBuffer &buf);

 public:
  std::vector<TLV*> value_list_;
};

class SetRsp : public Msg {
 public:
  SetRsp(): Msg(GET_RSP) {
  }

  typedef Msg super_;

  virtual int encode(z::ZByteBuffer &buf);
  virtual int decode(z::ZByteBuffer &buf);

 public:
  uint16_t status_;
  // TODO: error for every single field
};


}

#endif // _PUSH_MESSAGE_H__

