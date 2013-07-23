#ifndef _Z_INNER_MESSAGE_H__
#define _Z_INNER_MESSAGE_H__

#include <stdlib.h>
#include <stdint.h>
#include <vector>

struct ZInnerAddress {
	int module_type_;
	int module_id_;	// not used
	int handler_id_;

	ZInnerAddress(int type, int id, int hid)
		: module_type_(type), module_id_(id), handler_id_(hid)
	{}

	ZInnerAddress(const ZInnerAddress &addr)
		: module_type_(addr.module_type_),
		module_id_(addr.module_id_), handler_id_(addr.handler_id_)
	{}

	ZInnerAddress()
		: module_type_(-1), module_id_(-1), handler_id_(-1)
	{}

	bool isValid() {
		return (module_type_ >= 0) && (module_id_ >= 0) && (handler_id_ >= 0);
	}
};

/////////////////////////////////////////////////////
class ZInnerMsg {
 public:
	 ZInnerMsg(const ZInnerAddress &src_addr, uint32_t msg_type)
		 : src_addr_(src_addr), msg_type_(msg_type), seq_(0)
	 {
	 }

 // public:
	//  uint32_t getMsgType() { return msg_type_; }
	//  void setMsgType(uint32_t msg_type) { msg_type_ = msg_type; }
	//  uint32_t getSeq() { return seq_; }
	//  void setSeq(uint32_t seq) { seq_ = seq; }
	//  const ZInnerAddress& getSrcAddr() { return src_addr_; }

 // TODO: make it protected
 public:
	ZInnerAddress src_addr_;
	ZInnerAddress dst_addr_;
	uint32_t msg_type_;
	uint32_t seq_;
};

#endif // _Z_INNER_MESSAGE_H__


