#include "zdispatcher.h"

#include "zerrno.h"

// we handle the memory
int ZDispatcher::sendMsg(ZInnerMsg *msg)
{
#ifdef Z_USE_MULTI_THREAD
#error "not support"
	// LIST_TYPE::iterator iter;
	// ZTask *session = NULL;
	// 
	// for (iter = module_list_.begin(); iter != module_list_.end(); ++iter) {
	// 	if ((*iter)->getId() == moduleId) {
	// 		m = *iter;
	// 		break;
	// 	}
	// }

	// if (m == NULL) {
	// 	return -1;
	// }

	// return m->sendMsg(msg);
#else // not define Z_USE_MULTI_THREAD
	if (!msg->addr.isValid()) {
		delete msg;
		return -1;
	}

	msg_list_.push_back(msg);
	printf("add new inner msg\n");

	return 0;
#endif // Z_USE_MULTI_THREAD
}

int ZDispatcher::registerModule(ZModule *m)
{
	module_list_.push_back(m);

	printf("Module(%d:%s), added\n",
			m->getType(), moduleType2string(m->getType()));

	return OK;
}

void ZDispatcher::routine()
{
	// printf("ZDispatcher::routine\n");

#ifdef Z_USE_MULTI_THREAD
	// do nothing, actually, this is not neccessary if multi-thread is using
#else // Z_USE_MULTI_THREAD

	if (msg_list_.size() <= 0) {
		return;
	}

	printf("msg_list size: %ld\n", msg_list_.size());

	std::list<ZInnerMsg*>::iterator iter1;
	MODULE_LIST_TYPE::iterator iter2;
	ZModule *m = NULL;
	ZInnerAddress addr1, addr2;

	for (iter1 = msg_list_.begin(); iter1 != msg_list_.end(); ++iter1) {
		addr1 = (*iter1)->addr;
		for (iter2 = module_list_.begin(); iter2 != module_list_.end(); ++iter2) {
			if ((*iter2)->getType() == addr1.moduleType) {
				// if (addr2.moduleId == addr1.moduleId) {
					m = *iter2;
					m->sendMsg(*iter1);
					delete *iter1;
					break;
				// broadcast is not allowed, if you really want this feature, should
				// implement message serialization first(like internet protocol)
				// } else if (addr1.moduleId == 0) {
				// 	m = *iter2;
				// 	m->sendMsg(*iter1);
				// }
			}
		}

		if (iter2 == module_list_.end()) {
			// XXX:
			// delete (*iter1)->data;
			printf("target module not found\n");
			delete *iter1;
		}
	}

	msg_list_.clear();

#endif // Z_USE_MULTI_THREAD
}


