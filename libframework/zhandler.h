#ifndef _Z_HANDLER_H__
#define _Z_HANDLER_H__

#include <event2/event.h>

#include "zinner_message.h"

class ZHandler {
 public:
	virtual int init() = 0;
	virtual void close() = 0;
	virtual int onRead(char *buf, uint32_t buf_len) = 0;
	virtual int onInnerMsg(ZInnerMsg *msg) = 0;
	virtual void routine(long delta) = 0;

	virtual int send(const char *buf, uint32_t buf_len) = 0;

 public:
 	int getId() { return id_; }
 	void setId(int id) { id_ = id; }
 	int getModuleType() { return module_type_; }
 	void setModuleType(int type) { module_type_ = type; }

 private:
 	int id_;
 	int module_type_;
};


#endif // _Z_HANDLER_H__


