#include "zb_stream.h"

#include "zzigbee_message.h"
#include "msg_factory.h"

static const int MIN_HEAD_LEN = 1   // ver_(1)
                              + 2;  // len_(2);
static const int HEAD_LEN = 1   // ver_(1)
                          + 2   // len_(2)
                          + 1   // cmd_(1)
                          + 2;  // addr_(2)

static const int RESET_INTERVAL = 1; // 1 second

int ZBStream::read(char *buf, int buf_len) {

  if (received_data_list_.size() <= 0) {
    return 0;
  }

  std::list<stream_data_t*>::iterator iter = received_data_list_.begin();
  stream_data_t *data = *iter;
  assert(data);
  assert(data->data_len > 0);

  int read_len;

  if (data->data_len > buf_len) {
    Z_LOG_D("buffer is not big enough:(, data_len=%d, buf_len=%d",
      data->data_len, buf_len);

    read_len = buf_len;

    memcpy(buf, data->data, buf_len);

    // update data struct
    {
      // update data_len first
      data->data_len = data->data_len - buf_len;

      // don't use memcpy, cause memory may collasping
      // don't new memory again, use data->data_len
      for (int i = 0; i < data->data_len; ++i) {
        data->data[i] = data->data[buf_len + i];
      }
    }

  } else {

    read_len = data->data_len;

    // buffer is big enough
    memcpy(buf, data->data, data->data_len);

    // don't forget to free memory
    delete []data->data;
    delete data;
    received_data_list_.erase(iter);

  }

  return read_len;
}


int ZBStream::feed(char *buf, int buf_len)
{
  int orig_len = buf_len;
  int rv;

  do {
    switch (state_) {
      case STATE_INIT:
      {
        rv = doInit(buf, buf_len);
        break;
      }
      case STATE_WAITING_FOR_HEAD:
      {
        rv = doWaitingForHead(buf, buf_len);
        break;
      }
      case STATE_WAITING_FOR_DATA:
      {
        rv = doWaitingForData(buf, buf_len);
        break;
      }
      default:
      {
        // should never happen!
        assert(false);
        rv = -1;
        break;
      }
    }

    if (rv < 0) {
      break;
    }

    buf += rv;
    buf_len -= rv;

  } while (buf_len > 0);

  return orig_len - buf_len;
}

int ZBStream::doInit(char *buf, int buf_len)
{
  Z_LOG_D("ZBStream::doInit()");

  int orig_len = buf_len;
  int rv;

  // check synchronize bytes byte by bytes
  {
    int i = 0;
    for (i = 0; i < buf_len; ++i) {
      rv = syn_matcher_.feed(buf[i]);
      if (rv < 0) {
        // Z_LOG_D("Not match at %d:0x%02X", i, buf[i]);
        syn_matcher_.reset();
        return i + 1;
      } else if (rv == 0) {
        assert(syn_matcher_.finished());
        break;
      }
    }
    buf += i;
    buf_len -= i;
  }

  if (syn_matcher_.finished()) {
    expect_data_len_ = MIN_HEAD_LEN;
    setState(STATE_WAITING_FOR_HEAD);
  } else {
    assert(buf_len == 0);
  }

  return orig_len - buf_len;
}

int ZBStream::doWaitingForHead(char *buf, int buf_len)
{
  Z_LOG_D("ZBStream::doWaitingForHead(%d)", expect_data_len_);

  if (buf_len < expect_data_len_) {
    buf_.put(buf, buf_len);
    expect_data_len_ -= buf_len;

    return buf_len;
  }

  int used_len = expect_data_len_;

  // read all expect bytes first
  buf_.put(buf, expect_data_len_);

  uint16_t msg_len;
  {
    int rv1;
    uint8_t zb_ver;

    rv1 = decode(zb_ver, buf_.getArray(), buf_.remaining());
    assert(rv1 == 1);

    rv1 = decode(msg_len, buf_.getArray()+1, buf_.remaining()-1);
    assert(rv1 == 2);

    Z_LOG_D("decoded version: %u", zb_ver);
    Z_LOG_D("decoded message length: %u", msg_len);

    // head length must be included
    msg_len += HEAD_LEN;
  }

  // we already read 3 bytes, so don't forget to exclude them
  expect_data_len_ = msg_len - MIN_HEAD_LEN;

  Z_LOG_D("expect_data_len_: %d", expect_data_len_);

  setState(STATE_WAITING_FOR_DATA);

  return used_len;
}

int ZBStream::doWaitingForData(char *buf, int buf_len)
{
  Z_LOG_D("ZBStream::doWaitingForData(%d)", expect_data_len_);

  if (buf_len < expect_data_len_) {
    buf_.put(buf, buf_len);
    expect_data_len_ -= buf_len;

    return buf_len;
  } else {
    buf_.put(buf, expect_data_len_);

    // save current recived data to received_data_list_;
    {
      buf_.flip();

      Z_LOG_D("Complete message, len=%d", buf_.remaining());

      stream_data_t *data = new stream_data_t;
      data->data_len = buf_.remaining();
      data->data = new char[data->data_len];

      buf_.get(data->data, data->data_len);
      assert(buf_.remaining() == 0);

      received_data_list_.push_back(data);

    }

    syn_matcher_.reset();
    buf_.clear();

    setState(STATE_INIT);

    return expect_data_len_;
  }

  // TODO: decode it!
}

void ZBStream::setState(int state)
{
  Z_LOG_D("ZBStream::setState([%s] -> [%s])",
    getStateName(state_),
    getStateName(state));
  state_ = state;
}

const char* ZBStream::getStateName(int state)
{
  switch (state) {
    case STATE_INIT:
      return "INIT";
    case STATE_WAITING_FOR_HEAD:
      return "WAITING_FOR_HEAD";
    case STATE_WAITING_FOR_DATA:
      return "WAITING_FOR_DATA";
    default:
      return "UNKNOWN";
  }
}



