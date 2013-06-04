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

//////////////////////////////////////////////////////////////////
// GetDevInfoReq
void ZDispatcher::processMsg(ZInnerGetDevInfoReq *msg)
{
	printf("ZDispatcher::processMsg(GetDevReq)\n");

	// send to serial
	// should have just one device
	ZModule *module = findModule(Z_MODULE_SERIAL, 0);
	if (module == NULL) {
		printf("Damn, target module is not found\n");
		return;
	}

	// check if exist
	ZInnerForwardSession *session =
		(ZInnerForwardSession*)session_ctrl_.find(msg->getSeq());
	if (session != NULL) {
		printf("Duplicated session found\n");
		return;
	}

	// create and save session
	// session = new ZInnerGetDevSession();
	session = new ZInnerForwardSession();
	session->setKey(msg->getSeq());
	session->module_type_ = Z_MODULE_SERIAL;
	session->module_id_ = 0;
	session->src_addr_ = msg->getSrcAddr();
	session_ctrl_.add(msg->getSeq(), session);

	module->sendMsg(msg);
}

//////////////////////////////////////////////////////////////////
// GetDevInfoRsp
void ZDispatcher::processMsg(ZInnerGetDevInfoRsp *msg)
{
	printf("ZDispatcher::processMsg(GetDevReq)\n");

	ZInnerForwardSession *session =
		(ZInnerForwardSession*)session_ctrl_.find(msg->getSeq());
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

//////////////////////////////////////////////////////////////////
// SetDevInfoReq
void ZDispatcher::processMsg(ZInnerSetDevInfoReq *msg)
{
	printf("ZDispatcher::processMsg(SetDevInfoReq)\n");

	// send to serial
	// should have just one device
	ZModule *module = findModule(Z_MODULE_SERIAL, 0);
	if (module == NULL) {
		printf("Damn, target module is not found\n");
		return;
	}

	// check if exist
	ZInnerForwardSession *session =
		(ZInnerForwardSession*)session_ctrl_.find(msg->getSeq());
	if (session != NULL) {
		printf("Duplicated session found\n");
		return;
	}

	// create and save session
	// session = new ZInnerGetDevSession();
	session = new ZInnerForwardSession();
	session->setKey(msg->getSeq());
	session->module_type_ = Z_MODULE_SERIAL;
	session->module_id_ = 0;
	session->src_addr_ = msg->getSrcAddr();
	session_ctrl_.add(msg->getSeq(), session);

	module->sendMsg(msg);
}

//////////////////////////////////////////////////////////////////
// SetDevInfoRsp
void ZDispatcher::processMsg(ZInnerSetDevInfoRsp *msg)
{
	printf("ZDispatcher::processMsg(SetDevInfoRsp)\n");

	ZInnerForwardSession *session =
		(ZInnerForwardSession*)session_ctrl_.find(msg->getSeq());
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

void ZDispatcher::processMsg(ZInnerGetDevListReq *msg)
{
	printf("ZDispatcher::processMsg(GetDevListReq)\n");

	ZInnerForwardSession *session =
		(ZInnerForwardSession*)session_ctrl_.find(msg->getSeq());
	if (session != NULL) {
		printf("Duplicated session found: %d\n", msg->getSeq());
		return;
	}

	ZModule *module = findModule(Z_MODULE_SERIAL, 0);
	if (module == NULL) {
		printf("Target module is not found\n");
		return;
	}

	session = new ZInnerForwardSession();
	session->setKey(msg->getSeq());
	session->module_type_ = Z_MODULE_SERIAL;
	session->module_id_ = 0;
	session->src_addr_ = msg->getSrcAddr();
	session_ctrl_.add(msg->getSeq(), session);

	module->sendMsg(msg);
}

void ZDispatcher::processMsg(ZInnerGetDevListRsp *msg)
{
	printf("ZDispatcher::processMsg(GetDevListRsp)\n");

	ZInnerForwardSession *session =
		(ZInnerForwardSession*)session_ctrl_.find(msg->getSeq());
	if (session == NULL) {
		printf("Session not found: %d\n", msg->getSeq());
		return;
	}

	session_ctrl_.remove(msg->getSeq());

	ZModule *module = findModule(session->src_addr_.moduleType, 0);
	if (module == NULL) {
		printf("Can't found module, disconnected?\n");
		return;
	}

	delete session;

	module->sendMsg(msg);
}

void ZDispatcher::processInnerMsg(ZInnerMsg *msg)
{
	printf("ZDispatcher::processInnerMsg(%p)\n", msg);

	switch (msg->getMsgType()) {
		case Z_ZB_GET_DEV_LIST_REQ:
			{
				processMsg((ZInnerGetDevListReq*)msg);
				break;
			}
		case Z_ZB_GET_DEV_LIST_RSP:
			{
				processMsg((ZInnerGetDevListRsp*)msg);
				break;
			}
		case Z_ZB_GET_DEV_REQ:
			{
				processMsg((ZInnerGetDevInfoReq*)msg);
				break;
			}
		case Z_ZB_GET_DEV_RSP:
			{
				processMsg((ZInnerGetDevInfoRsp*)msg);
				break;
			}
		case Z_ZB_SET_DEV_REQ:
			{
				processMsg((ZInnerSetDevInfoReq*)msg);
				break;
			}
			case Z_ZB_SET_DEV_RSP:
			{
				processMsg((ZInnerSetDevInfoRsp*)msg);
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

void ZDispatcher::consumeMsg()
{
	if (msg_list_.size() <= 0) {
		return;
	}

	std::list<ZInnerMsg*>::iterator iter;

	for (iter = msg_list_.begin(); iter != msg_list_.end(); ++iter) {
		processInnerMsg(*iter);
	}

	msg_list_.clear();

}

void ZDispatcher::checkTimeout(long delta)
{
	// printf("ZDispatcher::checkTimeout\n");

	ZSession *session;

	SESSION_CTRL_TYPE::iterator iter = session_ctrl_.begin();
	SESSION_CTRL_TYPE::iterator tmp_iter;

	while (iter != session_ctrl_.end()) {
		// printf("while loop\n");
		session = iter->second;
		session->doTimeout(delta);
		if (session->isComplete())	{
			// printf("delta: [%ld]\n", delta);
			// printf("timeout: [%ld]\n", session->getTimeout());
			tmp_iter = iter;
			++iter;
			session_ctrl_.erase(tmp_iter);
		} else {
			++iter;
		}
	}
}

void ZDispatcher::routine(long delta)
{
	consumeMsg();

	checkTimeout(delta);
}

