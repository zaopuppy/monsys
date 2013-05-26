#include "zdispatcher.h"

#include "zerrno.h"

// we handle the memory
int ZDispatcher::sendMsg(ZInnerMsg *msg)
{
	if (!msg->dst_addr_.isValid()) {
		printf("destination address is invalid\n");
		// delete msg;
		// return -1;
	}

	msg_list_.push_back(msg);
	printf("add new inner msg\n");

	return 0;
}

int ZDispatcher::registerModule(ZModule *m)
{
	module_list_.push_back(m);

	printf("Module(%d:%s), added\n",
			m->getType(), moduleType2string(m->getType()));

	return OK;
}

ZModule* ZDispatcher::findModule(int moduleType, int moduleId)
{
	MODULE_LIST_TYPE::iterator iter;
	ZModule *m = NULL;

	for (iter = module_list_.begin(); iter != module_list_.end(); ++iter) {
		m = *iter;
		if (m->getType() != moduleType) {
			continue;
		}
		// TODO: add module-type and handler-id
		// if (moduleId == 0) {
		// 	return *iter;
		// } else if (m->get
		return m;
	}

	return NULL;
}

void ZDispatcher::processGetDevReq(ZInnerGetDevInfoReq *msg)
{
	printf("ZDispatcher::processGetDevReq(%p)\n", msg);

	// send to serial
	// should have just one device
	ZModule *module = findModule(Z_MODULE_SERIAL, 0);
	if (module == NULL) {
		printf("Damn, target module is not found\n");
		return;
	}

	// check if exist
	ZInnerGetDevSession *session =
		(ZInnerGetDevSession*)session_ctrl_.find(msg->getSeq());
	if (session != NULL) {
		printf("Duplicated session found\n");
		return;
	}

	// create and save session
	session = new ZInnerGetDevSession();
	session->setKey(msg->getSeq());
	session->module_type_ = Z_MODULE_SERIAL;
	session->module_id_ = 0;
	session->src_addr_ = msg->getSrcAddr();
	session_ctrl_.add(msg->getSeq(), session);

	module->sendMsg(msg);
}

void ZDispatcher::processGetDevRsp(ZInnerGetDevInfoRsp *msg)
{
	printf("ZDispatcher::processGetDevReq(%p)\n", msg);

	ZInnerGetDevSession *session =
		(ZInnerGetDevSession*)session_ctrl_.find(msg->getSeq());
	if (session == NULL) {
		printf("Can't found session, timeout?\n");
		return;
	}

	session_ctrl_.remove(msg->getSeq());

	ZModule *module = findModule(session->src_addr_.moduleType, 0);
	if (module == NULL) {
		printf("Can't found module, disconnected?\n");
		return;
	}

	module->sendMsg(msg);
}

void ZDispatcher::processMsg(ZInnerMsg *msg)
{
	printf("ZDispatcher::processMsg(%p)\n", msg);

	switch (msg->getMsgType()) {
		case Z_ZB_GET_DEV_REQ:
			{
				processGetDevReq((ZInnerGetDevInfoReq*)msg);
				break;
			}
		case Z_ZB_GET_DEV_RSP:
			{
				processGetDevRsp((ZInnerGetDevInfoRsp*)msg);
				break;
			}
		default:
			{
				printf("Unknown message type: %d\n", Z_ZB_GET_DEV_REQ);
				break;
			}
	}

	delete msg;
}

void ZDispatcher::routine()
{
	// printf("ZDispatcher::routine\n");

	if (msg_list_.size() <= 0) {
		return;
	}

	std::list<ZInnerMsg*>::iterator iter;

	for (iter = msg_list_.begin(); iter != msg_list_.end(); ++iter) {
		processMsg(*iter);
	}

	msg_list_.clear();

}


