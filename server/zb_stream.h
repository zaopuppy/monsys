#ifndef _ZB_STREAM_H__
#define _ZB_STREAM_H__

#include <string.h>
#include <list>

#include "libframework/zframework.h"

#include "syn_byte_matcher.h"

// class ZBStream : public ZStream
// TODO: add timeout cleaning, for auto-recovering
class ZBStream
{
 public:
  ZBStream()
    : state_(STATE_INIT)
    , buf_(4 << 10)
    , syn_matcher_(ZB_SYN_BYTES, ZB_SYN_BYTES_LEN)
    , last_feed_time_(0)
  {
  }
  ~ZBStream() {};

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

  void checkTime() {
    // TODO:
    // if didn't receive anything more than 1 second
    // reset it
  }

  void resetTime() {
    // TODO:
  }

  typedef struct {
    char *data;
    int data_len;
  } stream_data_t;

 public:
  typedef enum {
    STATE_INIT,
    STATE_WAITING_FOR_HEAD,
    STATE_WAITING_FOR_DATA,
  } state_t;

 private:
  int state_;
  // for receiving data
  ZDataBuffer<char> buf_;
  SynBytesMatcher syn_matcher_;
  int expect_data_len_;

  // TODO:
  long last_feed_time_;

  std::list<stream_data_t*> received_data_list_;
};

#endif // _ZB_STREAM_H__

