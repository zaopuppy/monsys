
#ifndef _Z_DISPATCHER_H__
#define _Z_DISPATCHER_H__

#include <list>

#include "zmodule.h"

class ZDispatcher {
 private:
	ZDispatcher() {
			module_list_.clear();
		}

 public:
	static ZDispatcher* instance() {
		static ZDispatcher dispatcher;
		return &dispatcher;
	}
 public:
	int registerModule(ZModule *module);
	int sendMsg(ZInnerMsg *msg);
	void routine();

	// typedef std::hashmap<int, ZTask*> LIST_TYPE;
	typedef std::list<ZModule*> MODULE_LIST_TYPE;
 private:
	// use hash map instead of list
	MODULE_LIST_TYPE module_list_;
#ifndef Z_USE_MULTI_THREAD
	std::list<ZInnerMsg*> msg_list_;
#endif // Z_USE_MULTI_THREAD
};

#endif


