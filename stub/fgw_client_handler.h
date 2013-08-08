#ifndef _FGW_CLIENT_HANDLER_H__
#define _FGW_CLIENT_HANDLER_H__

#include "libframework/zframework.h"

#include "module.h"

class FGWClientHandler : public ZClientHandler {
 public:
	// FGWClientHandler(event_base *base)
	// : ZClient(base, MODULE_FGW_CLIENT), handler_(NULL) {
	// 	handler_ = new FGWClientHandler();
	// }
	// ~FGWClientHandler() {}

 private:
 	// ZClientHandler *handler_;
};

#endif // _FGW_CLIENT_HANDLER_H__

