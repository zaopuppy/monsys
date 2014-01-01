#ifndef _ZB_TRANSFER_LAYER_H__
#define _ZB_TRANSFER_LAYER_H__

#include <string.h>
#include <list>

#include "libbase/ztypes.h"
#include "libframework/zframework.h"

// #include "syn_byte_matcher.h"

namespace zb {

class Layer {
public:
  virtual int read(char *buf, int buf_len) = 0;
  virtual int write(const char *buf, int buf_len) = 0;
  virtual int feed(char *buf, int buf_len) = 0;
};

class LinkLayer {
public:
  virtual int read(char *buf, int buf_len) = 0;
  virtual int write(const char *buf, int buf_len) = 0;
  virtual int feed(char *buf, int buf_len) = 0;
};

class TransferLayer_ {
public:
  virtual int read(char *buf, int buf_len) = 0;
  virtual int write(const char *buf, int buf_len) = 0;
  virtual int feed(char *buf, int buf_len) = 0;
};

static void sample()
{
}

class TransferLayer {
 public: 
  TransferLayer()
    : state_(STATE_INIT)
    , buf_(4 << 10)
  {
  }
  ~TransferLayer() {};

  virtual int read(char *buf, int buf_len);
  virtual int write(const char *buf, int buf_len) {
    return 0;
  }

  int feed(char *buf, int buf_len);

  int doInit(char *buf, int buf_len);
  int doWaitingForHead(char *buf, int buf_len);
  int doWaitingForData(char *buf, int buf_len);

  int getState() { return state_; }

  const char* getStateName(int state);

 protected:
  void setState(int state);

  typedef struct {
    char *data;
    int data_len;
  } stream_data_t;

 public:
  enum {
    STATE_INIT,
    STATE_WAITING_FOR_HEAD,
    STATE_WAITING_FOR_DATA,
  };

 private:
  int state_;
  // for receiving data
  ZDataBuffer<char> buf_;
  int expect_data_len_;

  std::list<stream_data_t*> received_data_list_;
};

};

#endif // _ZB_TRANSFER_LAYER_H__

