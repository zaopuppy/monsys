#include "zdispatcher.h"

#include "zerrno.h"

// we handle the memory
int ZDispatcher::sendMsg(ZInnerMsg *msg)
{
#ifdef Z_USE_MULTI_THREAD
	LIST_TYPE::iterator iter;
	ZTask *session = NULL;
	
	for (iter = session_list_.begin(); iter != session_list_.end(); ++iter) {
		if ((*iter)->getId() == moduleId) {
			session = *iter;
			break;
		}
	}

	if (session == NULL) {
		return -1;
	}

	return session->sendMsg(msg);
#else // not define Z_USE_MULTI_THREAD
	if (!msg->addr.isValid()) {
		delete msg;
		return -1;
	}

	msg_list_.push_back(msg);

	return 0;
#endif // Z_USE_MULTI_THREAD
}

int ZDispatcher::registerModule(ZTask *task)
{
	session_list_.push_back(task);
	printf("Module(%d, %d), added\n",
			task->getAddr().moduleType,
			task->getAddr().moduleId);
	return OK;
}

void ZDispatcher::routine()
{
#ifdef Z_USE_MULTI_THREAD
	// do nothing, actually, this is not neccessary if multi-thread is using
#else // Z_USE_MULTI_THREAD

	if (msg_list_.size() <= 0) {
		return;
	}

	std::list<ZInnerMsg*>::iterator iter1;
	LIST_TYPE::iterator iter2;
	ZTask *session = NULL;
	ZInnerAddress addr1, addr2;

	for (iter1 = msg_list_.begin(); iter1 != msg_list_.end(); ++iter1) {
		addr1 = (*iter1)->addr;
		for (iter2 = session_list_.begin(); iter2 != session_list_.end(); ++iter2) {
			addr2 = (*iter2)->getAddr();
			if (addr2.moduleType == addr1.moduleType) {
				if (addr2.moduleId == addr1.moduleId) {
					session = *iter2;
					session->sendMsg(*iter1);
					delete *iter1;
					break;
				// broadcast is not allowed, if you really want this feature, should
				// implement message serialization first(like internet protocol)
				// } else if (addr1.moduleId == 0) {
				// 	session = *iter2;
				// 	session->sendMsg(*iter1);
				}
			}
		}
		if (iter2 == session_list_.end()) {
			// XXX:
			// delete (*iter1)->data;
			printf("target module not found\n");
			delete *iter1;
		}
	}

	msg_list_.clear();

#endif // Z_USE_MULTI_THREAD
}


