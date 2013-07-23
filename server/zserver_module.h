#ifndef _ZSERVER_SESSION_H__
#define _ZSERVER_SESSION_H__

#include <iostream>

#include "zframework.h"

class ZServerModule : public ZTask {
public:
	enum SESSION_TYPE {
		TYPE_UNKNOWN,
		TYPE_ZIGBEE,
		TYPE_WEBCLIENT,
	};

public:
	ZServerModule(const char* ip, uint16_t port,
			event_base* base, int type)
		: ZTask(base, Z_MODULE_SERVER), ip_(ip), port_(port), type_(type) {
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
	void onAccepting(evutil_socket_t fd, short events);

private:
	enum STATE {
		STATE_ACCEPTING,
		STATE_FINISHED,
	};

private:
	std::string ip_;
	uint16_t port_;

	int type_;
	evutil_socket_t fd_;
	STATE state_;
};

// class ZServerModule : public ZTask {
// public:
// 	enum SESSION_TYPE {
// 		TYPE_UNKNOWN,
// 		TYPE_ZIGBEE,
// 		TYPE_WEBCLIENT,
// 	};
// 
// public:
// 	ZServerModule(const char* ip, uint16_t port,
// 			event_base* base, int type)
// 		: ZTask(base, Z_MODULE_SERVER), ip_(ip), port_(port), type_(type) {
// 	}
// 
// 	typedef ZTask super_;
// public:
// 	virtual int init();
// 	virtual void close();
// 	virtual void event(evutil_socket_t fd, short events);
// 	virtual void doTimeout();
// 	virtual bool isComplete();
// 	virtual int onInnerMsg(ZInnerMsg *msg);
// 
// private:
// 	void onAccepting(evutil_socket_t fd, short events);
// 
// private:
// 	enum STATE {
// 		STATE_ACCEPTING,
// 		STATE_FINISHED,
// 	};
// 
// private:
// 	std::string ip_;
// 	uint16_t port_;
// 
// 	int type_;
// 	evutil_socket_t fd_;
// 	STATE state_;
// };


#endif // _ZSERVER_SESSION_H__


