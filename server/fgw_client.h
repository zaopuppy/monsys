#ifndef _FGW_CLIENT_H__
#define _FGW_CLIENT_H__

#include "libframework/zclient.h"

#include "fgw_client_handler.h"

class FGWClient : public ZClient {
 public:
	FGWClient(event_base *base)
	: ZClient(base, MODULE_FGW_CLIENT) {
		handler_ = new FGWClientHandler(1, this);
		ZClient::setHandler(handler_);
	}

	typedef ZClient super;

	virtual ~FGWClient() {}

 public:
	virtual void routine(long delta);

 private:
 	FGWClientHandler *handler_;
};

#endif // _FGW_CLIENT_H__

