#ifndef _Z_SERVER_HANDLER_H__
#define _Z_SERVER_HANDLER_H__

class ZServerHandler : public ZHandler {
 public:
	virtual int init() = 0;
	virtual int event(char *buf, uint32_t buf_len) = 0;
	virtual int onInnerMsg(ZInnerMsg *msg) = 0;

 public:
	int event(evutil_socket_t fd, short events) = 0;

 public:
	evutil_socket_t fd_;
	struct event *read_event_;
};

#endif // _Z_SERVER_HANDLER_H__


