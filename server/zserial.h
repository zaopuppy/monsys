#ifndef _Z_SERIAL_H__
#define _Z_SERIAL_H__

#include <map>

#include "ztask.h"

#include "zmodule.h"

#include "zzigbee_message.h"
#include "zzigbee_handler.h"

typedef struct {
	uint8_t addr;
} zb_dev_info_t;

// class ZSerial : public ZTask {
class ZSerial : public ZModule {
 public:
  ZSerial(event_base* base): base_(base), fd_(-1)
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

  int event(evutil_socket_t fd, short events);

  // XXX: use `timeout event' instead of freqent timeout check
  // virtual void doTimeout();
  // virtual bool isComplete();
  // virtual int onInnerMsg(ZInnerMsg *msg);

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
	event_base *base_;
  evutil_socket_t fd_;
  struct event* read_event_;
  // struct event* write_event_;
  STATE state_;
  char buf_[1 << 10];
	char buf_out_[1 << 10];
	ZZigBeeHandler *handler_;

	std::map<std::string, zb_dev_info_t*> dev_addr_map_;
};


#endif // _Z_SERIAL_H__

