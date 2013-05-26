#ifndef _Z_SESSION_H__
#define _Z_SESSION_H__

#include "zinner_message.h"

class ZSession {
 public:
	uint32_t getKey() { return key_; }
	void setKey(uint32_t key) { key_ = key; }


 private:
	uint32_t key_;
};

class ZInnerGetDevSession : public ZSession {
 public:

 public:
	int module_type_;
	int module_id_;
	ZInnerAddress src_addr_;
};

#endif // _Z_SESSION_H__

