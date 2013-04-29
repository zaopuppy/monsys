#ifndef _ZAPI_MODULE_H__
#define _ZAPI_MODULE_H__

#include "ztask.h"

#include <event2/event.h>


#include "zmodule.h"

class ZApiModule : public ZTask {
public:
	ZApiModule(event_base* base): ZTask(base, Z_MODULE_TERM) {
	}

	typedef ZTask super_;

public:
	virtual int init();
	virtual void close();
	virtual void event(evutil_socket_t fd, short events);
	virtual void doTimeout();
	virtual bool isComplete();
	virtual int onInnerMsg(ZInnerMsg *msg);

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


// public:
// 	struct event* read_event_;

private:
	// ZSocket server_;
	STATE state_;
	char buf_[512 << 10];
	char out_buf_[512 << 10];
	evutil_socket_t fd_;
};

#endif // _ZAPI_MODULE_H__


