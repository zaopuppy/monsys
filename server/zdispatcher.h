
#ifndef _Z_DISPATCHER_H__
#define _Z_DISPATCHER_H__

#include <list>

#include "ztask.h"

class ZDispatcher {
	private:
		ZDispatcher() {
			session_list_.clear();
		}

	public:
		static ZDispatcher* instance() {
			static ZDispatcher dispatcher;
			return &dispatcher;
		}
	public:
		int registerModule(ZTask *task);
		int sendMsg(ZInnerMsg *msg);
		void routine();

		// typedef std::hashmap<int, ZTask*> LIST_TYPE;
		typedef std::list<ZTask*> LIST_TYPE;
	private:
		// use hash map instead of list
		LIST_TYPE session_list_;
#ifndef Z_USE_MULTI_THREAD
		std::list<ZInnerMsg*> msg_list_;
#endif // Z_USE_MULTI_THREAD
};

#endif


