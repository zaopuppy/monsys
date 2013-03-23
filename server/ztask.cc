#include "ztask.h"

#include <assert.h>

#include "zdispatcher.h"

int ZTask::init()
{
	int rv = ZDispatcher::instance()->registerModule(this);
	assert(rv == 0);

	return rv;
}

int ZTask::sendMsg(ZInnerMsg *msg)
{
	// well, there's just one thread currently, but don't forget to add a
	// thread lock, if you want to use multi-thread.

#ifdef Z_USE_MULTI_THREAD
	// AutoLock_();
	// msg_list_.push_back(msg);
#else
	onInnerMsg(msg);
#endif // Z_USE_MULTI_THREAD

	return 0;
}


