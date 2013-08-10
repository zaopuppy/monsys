#ifndef _FGW_CLIENT_H__
#define _FGW_CLIENT_H__

#include "libframework/zclient.h"

#include "fgw_client_handler.h"

class FGWClient : public ZClient {
 public:
	FGWClient(event_base *base)
	: ZClient(base, 0) {
	}

	virtual ~FGWClient() {}

 public:
	virtual void routine(long delta);

 private:
 	// ZClientHandler *handler_;
};

#endif // _FGW_CLIENT_H__

