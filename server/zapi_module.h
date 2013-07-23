#ifndef _ZAPI_MODULE_H__
#define _ZAPI_MODULE_H__

#include <event2/event.h>


#include "zmodule_ex.h"

class ZApiModule : public ZModule {
public:
	ZApiModule(event_base* base): base_(base) {
	}

	typedef ZModule super_;

public:
	virtual int init();
	virtual void close();
	virtual int sendMsg(ZInnerMsg *msg);
	virtual int onInnerMsg(ZInnerMsg *msg);
	virtual int getType() { return Z_MODULE_API; }
	virtual void routine(long delta) {}

	void event(evutil_socket_t fd, short events);

private:
	void onConnected(evutil_socket_t fd, short events);
	int onRead(evutil_socket_t fd, char *buf, uint32_t buf_len);

	void processMsg(struct z_query_dev_req &msg);
	
	void printMsg(struct z_header &msg);
	void printMsg(struct z_dev_info &msg);
	void printMsg(struct z_dev_info_list &msg);
	void printMsg(struct z_query_dev_req &msg);
	void printMsg(struct z_query_dev_rsp &msg);

	void processCmd(evutil_socket_t fd, char* buf, uint32_t buf_len);

private:
	enum STATE {
		STATE_CONNECTED,
		STATE_FINISHED,
	};


public:
	struct event* read_event_;

private:
	event_base *base_;
	STATE state_;
	char buf_[512 << 10];
	char out_buf_[512 << 10];
	evutil_socket_t fd_;
};

#endif // _ZAPI_MODULE_H__


