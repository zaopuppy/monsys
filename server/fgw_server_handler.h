#ifndef _FGW_SERVER_HANDLER_H__
#define _FGW_SERVER_HANDLER_H__

#include "libframework/zframework.h"
#include "fgw_session.h"
#include "push_msg.h"
#include <jansson.h>

class FGWServer;

class FGWHandler : public ZServerHandler {
 public:
	FGWHandler(int id, evutil_socket_t fd, ZModule *module)
		: ZServerHandler(id, fd, module)
		// , server_(server)
	{}
	~FGWHandler() {}

	typedef ZServerHandler super_;

	virtual int init();
	virtual void close();
	virtual int onRead(char *buf, uint32_t buf_len);
	virtual int onInnerMsg(ZInnerMsg *msg);
	virtual int onInnerMsgEx(ZInnerMsg *msg);
	virtual void routine(long delta);

	virtual int send(const char *buf, uint32_t buf_len);

	typedef ZSessionCtrl2Key<uint32_t, uint32_t, FGWSession> SESSION_CTRL_TYPE;
	// typedef ZSessionCtrl<uint32_t, ZSession> SESSION_CTRL_TYPE;

	enum {
    // TODO: 1 sencod waiting timeout
		STATE_WAIT_FOR_LOGIN = 0,
		STATE_LOGED_IN,
	};

	void setState(int new_state);

 protected:
 	bool checkSessionBySequence(uint32_t sequence);
 	int processLoginReq(json_t *jmsg);
	int onRead_WaitForLogin(char *buf, uint32_t buf_len);
	int onRead_LogedIn(char *buf, uint32_t buf_len);

 	push::Msg* inner2push(ZInnerMsg *innerMsg);
 	ZInnerMsg* push2inner(push::Msg *pushMsg);

 private:
 	// <sequence, fgw_id>
 	SESSION_CTRL_TYPE session_ctrl_;
 	int state_;

};

#endif // _FGW_SERVER_HANDLER_H__

