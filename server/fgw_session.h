#ifndef _FGW_SESSION_H__
#define _FGW_SESSION_H__

#include "libframework/zframework.h"

class FGWSession : public ZSession<uint32_t> {
 public:
 	FGWSession() {
 		setTimeout(1000);
 	}

 	typedef ZSession super;

 public:
 	ZInnerAddress src_addr_;
 	ZInnerAddress dst_addr_;
};

#endif // _FGW_SESSION_H__
