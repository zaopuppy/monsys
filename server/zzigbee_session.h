#ifndef _Z_ZIGBEE_SESSION_H__
#define _Z_ZIGBEE_SESSION_H__

#include "zframework.h"

class ZZigBeeSession : public ZSession {
 public:
 	ZZigBeeSession() {
 		setTimeout(1000);
 	}

 	typedef ZSession super;

 	union common_data {
 		uint32_t u32;
 		uint16_t u16;
 	};

 public:
 	ZInnerAddress src_addr_;
 	ZInnerAddress dst_addr_;
 	union common_data extern_key_;
};


#endif // _Z_ZIGBEE_SESSION_H__

