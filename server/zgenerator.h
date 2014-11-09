#ifndef _Z_GENERATOR_H__
#define _Z_GENERATOR_H__

#include "libframework/zframework.h"

namespace z {

template <typename ReturnType, typename InputType>
class Generator {
public:
  virtual void send(InputType &v) = 0;
  virtual ReturnType next() = 0;
  virtual void close() = 0;
  virtual bool closed() = 0;
};

};

// simple number generator, output number from 1 ~ 0xFFFF
// NumGenerator generator_;
// int = generator_.next();
class HandlerIdGenerator : public z::Generator<handler_id_t, handler_id_t> {
public:
  HandlerIdGenerator(handler_id_t min, handler_id_t max)
  : min_(min), max_(max), closed_(false), last_(min)
  {}

public:
  virtual void send(handler_id_t &v) {}
  virtual handler_id_t next() {
    if (closed_) {
      return last_;
    }

    if (last_ > max_) {
      last_ = min_;
    }

    return last_++;
  }
  virtual void close() { closed_ = true; }
  virtual bool closed() { return closed_; }
  // virtual void throwException() = 0;

private:
  const handler_id_t min_;
  const handler_id_t max_;
  bool closed_;
  handler_id_t last_;
};

#endif // _Z_GENERATOR_H__

