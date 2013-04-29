#ifndef _Z_SERIAL_H__
#define _Z_SERIAL_H__

#include "ztask.h"

#include "zmodule.h"

#include "zzigbee_message.h"

class ZSerial : public ZTask {
 public:
 ZSerial(event_base* base):
  ZTask(base, Z_MODULE_SERIAL), fd_(-1)
    {
    }

  typedef ZTask super_;

 public:
  virtual int init();
  virtual void close();
  virtual void event(evutil_socket_t fd, short events);
  // XXX: use `timeout event' instead of freqent timeout check
  virtual void doTimeout();
  virtual bool isComplete();
  virtual int onInnerMsg(ZInnerMsg *msg);

 private:
  // int onWaitingForConnect(evutil_socket_t fd, short events);
  void onConnected(evutil_socket_t fd, short events);
  int onDisconnected(evutil_socket_t fd, short events);
	void onRead(evutil_socket_t fd, char *buf, uint32_t buf_len);

  int connect();
  void scheduleReconnect();

 private:
  enum STATE {
    // STATE_WAITING_FOR_CONNECT,
    STATE_CONNECTED,
    STATE_DISCONNECTED,
    STATE_FINISHED,
  };

 private:
  evutil_socket_t fd_;
  struct event* read_event_;
  // struct event* write_event_;
  STATE state_;
  char buf_[1 << 10];
	char buf_out_[1 << 10];
};


#endif // _Z_SERIAL_H__

