#ifndef _Z_HANDLER_H__
#define _Z_HANDLER_H__

#include <assert.h>

#include <event2/event.h>

#include "zmodule.h"
#include "zinner_message.h"

class ZHandler {
 public:
 	ZHandler(int id, ZModule *module)
 		: module_(module), id_(id)
 	{}

 public:
	virtual int init() = 0;
	virtual void close() = 0;
	virtual int onRead(char *buf, uint32_t buf_len) = 0;
	virtual int onInnerMsg(ZInnerMsg *msg) = 0;
	virtual void routine(long delta) = 0;

	virtual int send(const char *buf, uint32_t buf_len) = 0;

 public:
 	int getId() { return id_; }
 	ZModule* getModule() { return module_; }
 	int getModuleType() { return module_->getType(); }

 private:
 	ZModule *module_;
 	const int id_;
};


#endif // _Z_HANDLER_H__


