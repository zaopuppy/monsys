#ifndef _FGW_CLIENT_SESSION_H__
#define _FGW_CLIENT_SESSION_H__

#include "libframework/zframework.h"

class FGWClientSession : public ZSession<uint32_t> {
public:
  FGWClientSession() {
    setTimeout(1000);
  }

  typedef ZSession super;

public:
  // ZInnerAddress src_addr_;
  // ZInnerAddress dst_addr_;
  uint32_t webapi_seq;
};

#endif // _FGW_CLIENT_SESSION_H__
