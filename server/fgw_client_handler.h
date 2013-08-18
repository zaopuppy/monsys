#ifndef _FGW_CLIENT_HANDLER_H__
#define _FGW_CLIENT_HANDLER_H__

#include "libframework/zframework.h"

#include "module.h"

class FGWClientHandler : public ZClientHandler {
 public:
	FGWClientHandler(int id, ZModule *module)
		: ZClientHandler(id, module)
	{}
	// : ZClient(base, MODULE_FGW_CLIENT), handler_(NULL) {
	// 	handler_ = new FGWClientHandler();
	// }
	// ~FGWClientHandler() {}

 public:
	virtual int init();
	virtual void close();
	virtual int onRead(char *buf, uint32_t buf_len);
	virtual int onInnerMsg(ZInnerMsg *msg);
	virtual void routine(long delta);

	virtual int send(const char *buf, uint32_t buf_len);

	virtual void onConnected();

 protected:
	void sendRsp(const char *text_msg, int status);

 private:
	char buf_[512 << 10];
	char out_buf_[512 << 10];
	// std::map<uint32_t, ZWebApiSession*> session_map_;

	ZInnerAddress addr_;
};

#endif // _FGW_CLIENT_HANDLER_H__

