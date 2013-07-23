#include "zdispatcher.h"

#include "zerrno.h"

// typedef struct {
// 	uint32_t msg_type;
// 	int src_module_type;
// 	int src_handler_id;
// 	int dst_module_type;
// 	int dst_handler_id;
// 	bool save_session;
// } route_info_t;

// // TODO: need check if records are valid(e.g. no duplicated records)
// static route_info_t g_route_info[] = {
// 	{ Z_ZB_GET_DEV_LIST_REQ,
// 		Z_MODULE_WEBAPI, ANY_ID, Z_MODULE_SERIAL, ANY_ID,
// 		true
// 	},

// 	// should search saved session
// 	{ Z_ZB_GET_DEV_LIST_RSP,
// 		Z_MODULE_SERIAL, ANY_ID, Z_MODULE_SERIAL, ANY_ID,
// 		false
// 	},
// };

int ZDispatcher::sendDirect(ZInnerMsg *msg)
{
	ZInnerAddress dst_addr = msg->dst_addr_;

	// find module
	ZModule *module = findModule(dst_addr.module_type_);
	if (module == NULL) {
		printf("Target module [%d] is not found\n", dst_addr.module_type_);
		return FAIL;
	}

	module->sendMsg(msg);

	return OK;
}

// we handle the memory
int ZDispatcher::sendMsg(ZInnerMsg *msg)
{
	if (!msg->src_addr_.isValid()) {
		printf("Source address is invalid\n");
		// delete msg;
		// return -1;
	}

	msg_list_.push_back(msg);

	return 0;
}

int ZDispatcher::registerModule(ZModule *m)
{
	if (findModule(m->getType())) {
		printf("Duplicated module\n");
		return FAIL;
	}

	module_list_.push_back(m);

	// printf("Module(%d:%s), added\n",
	// 		m->getType(), moduleType2string(m->getType()));

	return OK;
}

ZModule* ZDispatcher::findModule(int moduleType)
{
	MODULE_LIST_TYPE::iterator iter = module_list_.begin();
	ZModule *m = NULL;

	for (; iter != module_list_.end(); ++iter) {
		m = *iter;
		if (m->getType() == moduleType) {
			return m;
		}
	}

	return NULL;
}

// ZModule* ZDispatcher::findModule(int moduleType, int moduleId)
// {
// 	MODULE_LIST_TYPE::iterator iter;
// 	ZModule *m = NULL;

// 	for (iter = module_list_.begin(); iter != module_list_.end(); ++iter) {
// 		m = *iter;
// 		if (m->getType() != moduleType) {
// 			continue;
// 		}
// 		// TODO: add module-type and handler-id
// 		// if (moduleId == 0) {
// 		// 	return *iter;
// 		// } else if (m->get
// 		return m;
// 	}

// 	return NULL;
// }

// //////////////////////////////////////////////////////////////////
// // GetDevInfoReq
// void ZDispatcher::processMsg(ZInnerGetDevInfoReq *msg)
// {
// 	printf("ZDispatcher::processMsg(GetDevReq)\n");

// 	// send to serial
// 	// should have just one device
// 	ZModule *module = findModule(Z_MODULE_SERIAL);
// 	if (module == NULL) {
// 		printf("Damn, target module is not found\n");
// 		return;
// 	}

// 	// check if exist
// 	ZInnerForwardSession *session =
// 		(ZInnerForwardSession*)session_ctrl_.find(msg->seq_);
// 	if (session != NULL) {
// 		printf("Duplicated session found\n");
// 		return;
// 	}

// 	// create and save session
// 	// session = new ZInnerGetDevSession();
// 	session = new ZInnerForwardSession();
// 	// session->setKey(msg->seq_);
// 	session->module_type_ = Z_MODULE_SERIAL;
// 	session->module_id_ = 0;
// 	session->src_addr_ = msg->src_addr_;
// 	session_ctrl_.add(msg->seq_, session);

// 	module->sendMsg(msg);
// }

// //////////////////////////////////////////////////////////////////
// // GetDevInfoRsp
// void ZDispatcher::processMsg(ZInnerGetDevInfoRsp *msg)
// {
// 	printf("ZDispatcher::processMsg(GetDevReq)\n");

// 	ZInnerForwardSession *session =
// 		(ZInnerForwardSession*)session_ctrl_.find(msg->seq_);
// 	if (session == NULL) {
// 		printf("Can't found session, timeout?\n");
// 		return;
// 	}

// 	session_ctrl_.remove(msg->seq_);

// 	ZModule *module = findModule(session->src_addr_.module_type_);
// 	if (module == NULL) {
// 		printf("Can't found module, disconnected?\n");
// 		return;
// 	}

// 	module->sendMsg(msg);
// }

// //////////////////////////////////////////////////////////////////
// // SetDevInfoReq
// void ZDispatcher::processMsg(ZInnerSetDevInfoReq *msg)
// {
// 	printf("ZDispatcher::processMsg(SetDevInfoReq)\n");

// 	// send to serial
// 	// should have just one device
// 	ZModule *module = findModule(Z_MODULE_SERIAL);
// 	if (module == NULL) {
// 		printf("Damn, target module is not found\n");
// 		return;
// 	}

// 	// check if exist
// 	ZInnerForwardSession *session =
// 		(ZInnerForwardSession*)session_ctrl_.find(msg->seq_);
// 	if (session != NULL) {
// 		printf("Duplicated session found\n");
// 		return;
// 	}

// 	// create and save session
// 	// session = new ZInnerGetDevSession();
// 	session = new ZInnerForwardSession();
// 	// session->setKey(msg->seq_);
// 	session->module_type_ = Z_MODULE_SERIAL;
// 	session->module_id_ = 0;
// 	session->src_addr_ = msg->src_addr_;
// 	session_ctrl_.add(msg->seq_, session);

// 	module->sendMsg(msg);
// }

// //////////////////////////////////////////////////////////////////
// // SetDevInfoRsp
// void ZDispatcher::processMsg(ZInnerSetDevInfoRsp *msg)
// {
// 	printf("ZDispatcher::processMsg(SetDevInfoRsp)\n");

// 	ZInnerForwardSession *session =
// 		(ZInnerForwardSession*)session_ctrl_.find(msg->seq_);
// 	if (session == NULL) {
// 		printf("Can't found session, timeout?\n");
// 		return;
// 	}

// 	session_ctrl_.remove(msg->seq_);

// 	ZModule *module = findModule(session->src_addr_.module_type_);
// 	if (module == NULL) {
// 		printf("Can't found module, disconnected?\n");
// 		return;
// 	}

// 	module->sendMsg(msg);
// }

// void ZDispatcher::processMsg(ZInnerGetDevListReq *msg)
// {
// 	printf("ZDispatcher::processMsg(GetDevListReq)\n");

// 	ZInnerForwardSession *session =
// 		(ZInnerForwardSession*)session_ctrl_.find(msg->seq_);
// 	if (session != NULL) {
// 		printf("Duplicated session found: %d\n", msg->seq_);
// 		return;
// 	}

// 	ZModule *module = findModule(Z_MODULE_SERIAL);
// 	if (module == NULL) {
// 		printf("Target module is not found\n");
// 		return;
// 	}

// 	session = new ZInnerForwardSession();
// 	// session->setKey(msg->seq_);
// 	session->module_type_ = Z_MODULE_SERIAL;
// 	session->module_id_ = 0;
// 	session->src_addr_ = msg->src_addr_;
// 	session_ctrl_.add(msg->seq_, session);

// 	module->sendMsg(msg);
// }

// void ZDispatcher::processMsg(ZInnerGetDevListRsp *msg)
// {
// 	printf("ZDispatcher::processMsg(GetDevListRsp)\n");

// 	ZInnerForwardSession *session =
// 		(ZInnerForwardSession*)session_ctrl_.find(msg->seq_);
// 	if (session == NULL) {
// 		printf("Session not found: %d\n", msg->seq_);
// 		return;
// 	}

// 	session_ctrl_.remove(msg->seq_);

// 	ZModule *module = findModule(session->src_addr_.module_type_);
// 	if (module == NULL) {
// 		printf("Can't found module, disconnected?\n");
// 		return;
// 	}

// 	delete session;

// 	module->sendMsg(msg);
// }

// static route_info_t* findRoute(uint32_t msg_type, const ZInnerAddress &src_addr)
// {
// 	size_t route_table_size = sizeof(g_route_info)/sizeof(g_route_info[0]);
// 
// 	route_info_t *info;
// 	for (size_t i = 0; i < route_table_size; ++i) {
// 
// 		info = &g_route_info[i];
// 
// 		if ((info->msg_type == msg_type)
// 			&& (info->src_module_type == src_addr.module_type_)) {
// 
// 			if ((info->src_handler_id == src_addr.handler_id_)
// 				|| (src_addr.handler_id_ == ANY_ID)) {
// 
// 				return info;
// 			}
// 
// 		}
// 	}
// 
// 	return NULL;
// }

void ZDispatcher::processInnerMsgEx(ZInnerMsg *msg)
{
	printf("ZDispatcher::processInnerMsgEx(0x%08X)\n", msg->msg_type_);

	/*
	uint32_t msg_type = msg->msg_type_;
	ZInnerAddress src_addr = msg->src_addr_;

	route_info_t* info = findRoute(msg_type, src_addr);
	if (info == NULL) {
		printf("Route info not found for msg type: 0x%08X\n", msg_type);
		return;
	}
	*/

	// // send to serial
	// // should have just one device
	// ZModule *module = findModule(Z_MODULE_SERIAL, msg->src_addr_);
	// if (module == NULL) {
	// 	printf("Damn, target module is not found\n");
	// 	return;
	// }

	// // check if exist
	// ZInnerForwardSession *session =
	// 	(ZInnerForwardSession*)session_ctrl_.find(msg->seq_);
	// if (session != NULL) {
	// 	printf("Duplicated session found\n");
	// 	return;
	// }

	// // create and save session
	// // session = new ZInnerGetDevSession();
	// session = new ZInnerForwardSession();
	// session->setKey(msg->seq_);
	// session->module_type_ = Z_MODULE_SERIAL;
	// session->module_id_ = 0;
	// session->src_addr_ = msg->src_addr_;
	// session_ctrl_.add(msg->seq_, session);

	// module->sendMsg(msg);
}

void ZDispatcher::processInnerMsg(ZInnerMsg *msg)
{
	printf("ZDispatcher::processInnerMsg(%p)\n", msg);

	assert(false);

	// switch (msg->msg_type_) {
	// 	case Z_ZB_GET_DEV_LIST_REQ:
	// 		{
	// 			processMsg((ZInnerGetDevListReq*)msg);
	// 			break;
	// 		}
	// 	case Z_ZB_GET_DEV_LIST_RSP:
	// 		{
	// 			processMsg((ZInnerGetDevListRsp*)msg);
	// 			break;
	// 		}
	// 	case Z_ZB_GET_DEV_REQ:
	// 		{
	// 			processMsg((ZInnerGetDevInfoReq*)msg);
	// 			break;
	// 		}
	// 	case Z_ZB_GET_DEV_RSP:
	// 		{
	// 			processMsg((ZInnerGetDevInfoRsp*)msg);
	// 			break;
	// 		}
	// 	case Z_ZB_SET_DEV_REQ:
	// 		{
	// 			processMsg((ZInnerSetDevInfoReq*)msg);
	// 			break;
	// 		}
	// 		case Z_ZB_SET_DEV_RSP:
	// 		{
	// 			processMsg((ZInnerSetDevInfoRsp*)msg);
	// 			break;
	// 		}
	// 	default:
	// 		{
	// 			printf("Unknown message type: %d\n", Z_ZB_GET_DEV_REQ);
	// 			break;
	// 		}
	// }

	// delete msg;
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

void ZDispatcher::moduleRoutine(long delta)
{
	ZModule *module;
	MODULE_LIST_TYPE::iterator iter = module_list_.begin();
	for (; iter != module_list_.end(); ++iter) {
		module = *iter;
		module->routine(delta);
	}
}

void ZDispatcher::routine(long delta)
{
	consumeMsg();

	checkTimeout(delta);

	moduleRoutine(delta);
}

