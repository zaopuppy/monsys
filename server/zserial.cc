#include "zserial.h"

#include <assert.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
// #include <unistd.h>
#include <sys/ioctl.h>
#include <signal.h>

#include "libframework/zframework.h"



static const struct timeval RETRY_INTERVAL = { 5, 0 };

static void SOCKET_CALLBACK(evutil_socket_t fd, short events, void *arg)
{
  // Z_LOG_D("SOCKET_CALLBACK");
  assert(arg);
  ZSerial *h = (ZSerial*)arg;
  h->event(fd, events);
}

int ZSerial::init()
{
  int rv;

  rv = ZDispatcher::instance()->registerModule(this);
  if (rv != OK) {
    return FAIL;
  }

  rv = onDisconnected(-1, 0);
  if (rv != OK && rv != ERR_IO_PENDING) {
    return FAIL;
  }
  
  return OK;
}

void ZSerial::close()
{
  ::close(fd_);
  fd_ = -1;
}

// int ZSerial::sendMsg(ZInnerMsg *msg)
// {
//   Z_LOG_D("ZSerial::sendMsg");

//   return onInnerMsg(msg);
// }

int ZSerial::onInnerMsg(ZInnerMsg *msg)
{
  Z_LOG_D("ZSerial::onInnerMsg()");

  return handler_->onInnerMsg(msg);
}

int ZSerial::event(evutil_socket_t fd, short events)
{
  Z_LOG_D("ZSerial::event()");

  int rv = 0;

  switch (state_) {
  case STATE_CONNECTED:
    onConnected(fd, events);
    break;
  case STATE_DISCONNECTED:
    onDisconnected(fd, events);
    break;
  case STATE_FINISHED:
    // should never happen
    assert(false);
    break;
  default:
    close();
    break;
  }

  return rv;
}

void ZSerial::onConnected(evutil_socket_t fd, short events)
{
  Z_LOG_D("ZSerial::onConnected(%d)", fd);

  int rv;
  int offset = 0;
  size_t buf_len = sizeof(buf_);

  do {
    rv = read(fd, buf_ + offset, buf_len);
    Z_LOG_D("read %d bytes", rv);
    if (rv > 0) {
      buf_len -= rv;
      offset += rv;
    }
  } while (rv > 0 && buf_len > 0);

  if (buf_len <= 0) {
    Z_LOG_D("Receved too many bytes...no.");
  }

  if (offset > 0) {
    Z_LOG_D("Received:");
    trace_bin(buf_, offset);
    onRead(fd, buf_, offset);
  }

  if (rv == 0) {
    Z_LOG_D("peer closed");
    close();
    state_ = STATE_DISCONNECTED;
    scheduleReconnect();
    return;
  } else if (rv < 0 && errno != EAGAIN) {
    perror("read");
    Z_LOG_D("failed to read from serial port, errno: [%d]", errno);
    close();
    state_ = STATE_DISCONNECTED;
    scheduleReconnect();
    return;
  } 
}

static void signal_handler_IO(int status)
{
  Z_LOG_D("signal_handler_IO(%d)\n", status);
}

int ZSerial::connect()
{
  Z_LOG_D("Openning serial device: [%s]", serial_dev_.c_str());

  fd_ = open(serial_dev_.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK | O_NDELAY);
  if (fd_ < 0) {
    perror(serial_dev_.c_str());
    return FAIL;
  }

  // this signal handler must be set, or program will be abort
  // by signal SIGIO
  struct sigaction saio;
  saio.sa_handler = signal_handler_IO;
  // saio.sa_mask = 0;
  sigemptyset(&saio.sa_mask);
  // sigaddset(&new_action.sa_mask, SIGINT);
  saio.sa_flags = 0;
  // saio.sa_restorer = NULL;
  sigaction(SIGIO, &saio, NULL);

  fcntl(fd_, F_SETOWN, getpid());

  fcntl(fd_, F_SETFL, FASYNC | FNDELAY);

  // setting
  {
    struct termios opts;

    tcgetattr(fd_, &opts);

    cfsetispeed(&opts, B38400);
    cfsetospeed(&opts, B38400);

    // control
    // with hardware flow control
    // opts.c_cflag |= (CLOCAL | CREAD | CRTSCTS);
    // without hardware flow control
    opts.c_cflag |= (CLOCAL | CREAD);
    // 8N1
    // set character size
    opts.c_cflag &= ~PARENB;
    opts.c_cflag &= ~CSTOPB;
    opts.c_cflag &= ~CSIZE;
    opts.c_cflag |= CS8;

    // local options
    opts.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

    opts.c_iflag = IGNPAR;
    opts.c_oflag = 0;
    opts.c_cc[VMIN] = 1;
    opts.c_cc[VTIME] = 0;

    tcsetattr(fd_, TCSANOW, &opts);
  }

  Z_LOG_D("serial port initialized.");

  read_event_ =
    event_new(base_, fd_, EV_READ|EV_PERSIST, SOCKET_CALLBACK, (void*)this);
  if (read_event_ == NULL) {
    Z_LOG_D("failed to new event");
    return FAIL;
  }
  
  int rv = event_add(read_event_, NULL);
  if (rv != 0) {
    Z_LOG_D("failed to add new event");
    return FAIL;
  }

  state_ = STATE_CONNECTED;

  handler_->fd_  = fd_;

  handler_->onConnected();

  return OK;
}

int ZSerial::onDisconnected(evutil_socket_t fd, short events)
{
  int rv = connect();
  switch (rv) {
  case OK:
    {
      state_ = STATE_CONNECTED;
      
      return OK;
    }
  case FAIL:
    {
      state_ = STATE_DISCONNECTED;
      // XXX
      scheduleReconnect();
      return ERR_IO_PENDING;
    }
  default:
    break;
  }

  return FAIL;

}
void ZSerial::onRead(evutil_socket_t fd, char *buf, uint32_t buf_len)
{
  Z_LOG_D("ZSerial::onRead");

  // TODO: should be a loop...
  // handler_->onRead(buf, buf_len);

  int rv;
  rv = stream_.feed(buf, buf_len);

  // should not use assert
  assert(rv == (int)buf_len);

  while (true) {
    rv = stream_.read(buf_, sizeof(buf_));
    Z_LOG_D("%d bytes read", rv);
    if (rv <= 0) {
      break;
    }

    handler_->onRead(buf_, rv);
  }
}

void ZSerial::scheduleReconnect()
{
  return;
  Z_LOG_D("ZSerial::scheduleReconnect()");
  close();
  struct event* ev = evtimer_new(base_, SOCKET_CALLBACK, this);
  event_add(ev, &RETRY_INTERVAL);
}


