#ifndef _FGW_SERVER_HANDLER_H__
#define _FGW_SERVER_HANDLER_H__

#include "libframework/zframework.h"
#include "fgw_session.h"

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
	virtual void routine(long delta);

	virtual int send(const char *buf, uint32_t buf_len);

	typedef ZSessionCtrl2Key<uint32_t, uint32_t, FGWSession> SESSION_CTRL_TYPE;
	// typedef ZSessionCtrl<uint32_t, ZSession> SESSION_CTRL_TYPE;

 protected:
 	bool checkSessionBySequence(uint32_t sequence);

 private:
 	// <sequence, fgw_id>
 	SESSION_CTRL_TYPE session_ctrl_;

};

#endif // _FGW_SERVER_HANDLER_H__

