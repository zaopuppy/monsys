#ifndef _FGW_CLIENT_HANDLER_H__
#define _FGW_CLIENT_HANDLER_H__

#include <jansson.h>

#include "libframework/zframework.h"

#include "module.h"
#include "fgw_client_session.h"

class FGWClientHandler : public ZClientHandler {
 public:
	FGWClientHandler(int id, ZModule *module)
		: ZClientHandler(id, module)
	{}

 public:
	virtual int init();
	virtual void close();
	virtual int onRead(char *buf, uint32_t buf_len);
	virtual int onInnerMsg(ZInnerMsg *msg);
	virtual void routine(long delta);

	virtual int send(const char *buf, uint32_t buf_len);
	virtual int sendJson(json_t *jmsg);

	virtual void onConnected();

	void fgwLogin();

 protected:
	int onRead_Unregistered(char *buf, uint32_t buf_len);
	int onRead_WaitForServer(char *buf, uint32_t buf_len);
	int onRead_Registered(char *buf, uint32_t buf_len);
 	int processLoginRsp(json_t *jmsg);

	void sendRsp(const char *text_msg, int status);

	typedef ZSessionCtrl2Key<uint32_t, uint32_t, FGWClientSession> SESSION_CTRL_TYPE;

	// state-machine
	enum {
		STATE_UNREGISTERED,
		STATE_WAIT_FOR_SERVER,
		STATE_REGISTERED,
	};

	void setState(int new_state);

 private:
	// char buf_[512 << 10];
	// char out_buf_[512 << 10];

	ZInnerAddress addr_;

 	SESSION_CTRL_TYPE session_ctrl_;

 	// state-machine
 	int state_;
};

#endif // _FGW_CLIENT_HANDLER_H__

