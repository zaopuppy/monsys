#ifndef _Z_SERIAL_H__
#define _Z_SERIAL_H__

#include <map>

#include "libframework/zframework.h"

#include "libzigbee/zzigbee_message.h"
#include "zzigbee_handler.h"
#include "zb_stream.h"

class ZSerial : public ZModule {
 public:
  ZSerial(event_base* base, const char *serial_dev)
    : ZModule(MODULE_SERIAL)
    , base_(base), fd_(-1), serial_dev_(serial_dev)
  {
		handler_ = new ZZigBeeHandler(1, this);
  }

  typedef ZModule super_;

 public:
  virtual int init();
  virtual void close();
	virtual int sendMsg(ZInnerMsg *msg);
	virtual int onInnerMsg(ZInnerMsg *msg);
  virtual void routine(long delta) { handler_->routine(delta); }

  int event(evutil_socket_t fd, short events);

 private:
  // int onWaitingForConnect(evutil_socket_t fd, short events);
  void onConnected(evutil_socket_t fd, short events);
  int onDisconnected(evutil_socket_t fd, short events);
	void onRead(evutil_socket_t fd, char *buf, uint32_t buf_len);

  int connect();
  void scheduleReconnect();

 private:
  enum STATE {
    STATE_CONNECTED,
    STATE_DISCONNECTED,
    STATE_FINISHED,
  };

 private:
	event_base *base_;
  evutil_socket_t fd_;
  struct event* read_event_;
  STATE state_;
  char buf_[1 << 10];
	char buf_out_[1 << 10];
	ZZigBeeHandler *handler_;

  std::string serial_dev_;

  ZBStream stream_;
};


#endif // _Z_SERIAL_H__

