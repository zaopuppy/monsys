#ifndef _Z_MON_MESSAGE_H__
#define _Z_MON_MESSAGE_H__

#include "zmsg.h"

class ZMonMsg : public ZMsg {
 public:
	virtual int encode(char *buf, uint32_t buf_len);
	virtual int decode(char *buf, uint32_t buf_len);

 public:
	uint16_t len_;
	uint8_t cmd_;
	uint32_t seq_;
 private:
	// std::vector<>
};


#endif // _Z_MON_MESSAGE_H__


