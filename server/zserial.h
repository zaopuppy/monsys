#ifndef _Z_SERIAL_H__
#define _Z_SERIAL_H__

#include <map>

#include "framework/zframework.h"

#include "zzigbee_message.h"
#include "zzigbee_handler.h"

class ZSerial : public ZModule {
 public:
  ZSerial(event_base* base, const char *serial_dev)
    : base_(base), fd_(-1), serial_dev_(serial_dev)
  {
		handler_ = new ZZigBeeHandler();
  }

  typedef ZModule super_;

 public:
  virtual int init();
  virtual void close();
	virtual int sendMsg(ZInnerMsg *msg);
	virtual int onInnerMsg(ZInnerMsg *msg);
	virtual int getType() { return Z_MODULE_SERIAL; }
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

	// std::map<std::string, zb_dev_info_t*> dev_addr_map_;
  std::string serial_dev_;
};


#endif // _Z_SERIAL_H__

