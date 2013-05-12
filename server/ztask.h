#ifndef _ZSESSION_H__
#define _ZSESSION_H__

// #include <list>
// 
// #include <event2/event.h>
// 
// #include "zinner_message.h"
// 
// class ZTask {
// public:
// 	ZTask(event_base* base, int type): base_(base) {
// #ifdef Z_USE_MULTI_THREAD
// 		msg_list_.clear();
// #endif // Z_USE_MULTI_THREAD
// 		addr_.moduleType = type;
// 		addr_.moduleId = 1;	// TODO: should be generated automatically.
// 	}
// 	virtual ~ZTask() {
// 	}
// 
// public:
// 	virtual int init();
// 	virtual void close() = 0;
// 	virtual void event(evutil_socket_t fd, short events) = 0;
// 	virtual void doTimeout() = 0;
// 	virtual bool isComplete() = 0;
// 	virtual int onInnerMsg(ZInnerMsg *msg) = 0;
// 
// public:
// 	ZInnerAddress getAddr() { return addr_; }
// 	int sendMsg(ZInnerMsg *msg);
// 
// public:
// 	struct event* read_event_;
// 
// protected:
// 
// #ifdef Z_USE_MULTI_THREAD
// 	std::list<ZInnerMsg*> msg_list_;
// #endif
// 
// 	event_base* base_;
// 	ZInnerAddress addr_;
// };

#endif // _ZSESSION_H__

