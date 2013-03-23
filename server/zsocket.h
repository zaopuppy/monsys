#ifndef _ZSOCKET_H__
#define _ZSOCKET_H__

#include <assert.h>

#include <event2/event.h>

class ZSocket {
public:
  struct event* read_event;
  struct event* write_event;
	struct event_base* base_event;

public:
	ZSocket(event_base* base): base_event(base) {
	}

public:
  virtual bool init() = 0;
  virtual void close() = 0;
	// XXX: arg is unneccessary
  virtual void doRead(evutil_socket_t fd, short events, void* arg) = 0;
  virtual void doWrite(evutil_socket_t fd, short events, void* arg) = 0;
};



#endif // _ZSOCKET_H__

