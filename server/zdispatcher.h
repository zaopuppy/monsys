
#ifndef _Z_DISPATCHER_H__
#define _Z_DISPATCHER_H__

#include <list>

#include "zmodule.h"
#include "zsession.h"
#include "zsession_ctrl.h"

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

 private:
	ZModule* findModule(int moduleType, int moduleId);
	void processGetDevReq(ZInnerGetDevInfoReq *msg);
	void processGetDevRsp(ZInnerGetDevInfoRsp *msg);
	void processMsg(ZInnerMsg *msg);

	typedef std::list<ZModule*> MODULE_LIST_TYPE;

 private:
	// TODO: use hash map instead of list
	MODULE_LIST_TYPE module_list_;
	std::list<ZInnerMsg*> msg_list_;
	ZSessionCtrl<uint32_t, ZSession> session_ctrl_;
};

#endif


