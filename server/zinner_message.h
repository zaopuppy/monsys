#ifndef _Z_INNER_MESSAGE_H__
#define _Z_INNER_MESSAGE_H__

#include <stdlib.h>
#include <stdint.h>
#include <vector>

// XXX
#include "zzigbee_message.h"

#include "zdevice.h"

// --- temporary using
struct ZData {
	int data;
};

enum {
	Z_INVALID = -1,

	Z_ZB_GET_DEV_LIST_REQ,
	Z_ZB_GET_DEV_LIST_RSP,

	Z_ZB_GET_DEV_REQ,
	Z_ZB_GET_DEV_RSP,

	Z_ZB_SET_DEV_REQ,
	Z_ZB_SET_DEV_RSP,
};

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

////////////////////////////////////////////////////
// GetDevList
class ZInnerGetDevListReq : public ZInnerMsg {
 public:
	ZInnerGetDevListReq(const ZInnerAddress &src_addr)
		: ZInnerMsg(src_addr, Z_ZB_GET_DEV_LIST_REQ)
	{
	}

};

class ZInnerGetDevListRsp : public ZInnerMsg {
 public:
 	ZInnerGetDevListRsp(const ZInnerAddress &src_addr)
 		: ZInnerMsg(src_addr, Z_ZB_GET_DEV_LIST_RSP)
 	{
 	}
 	~ZInnerGetDevListRsp()
 	{
 		// delete infos
		for (uint32_t i = 0; i < info_list_.size(); ++i) {
			delete info_list_[i];
		}
		info_list_.clear();
 	}

 public:
 	std::vector<ZZBDevInfo*> info_list_;
};

////////////////////////////////////////////////////
// GetDevInfo
class ZInnerGetDevInfoReq : public ZInnerMsg {
 public:
	ZInnerGetDevInfoReq(const ZInnerAddress &src_addr)
	  : ZInnerMsg(src_addr, Z_ZB_GET_DEV_REQ)
	{
	}

 public:
	uint16_t addr_;
	std::vector<uint8_t> item_ids_;
};

class ZInnerGetDevInfoRsp : public ZInnerMsg {
 public:
	ZInnerGetDevInfoRsp(const ZInnerAddress &src_addr)
	  : ZInnerMsg(src_addr, Z_ZB_GET_DEV_RSP)
	{
	}

 public:
	// uint16_t addr_;
	// std::vector< std::vector<ZItemPair>* > dev_infos_;
	std::vector<ZItemPair> dev_infos_;
	// std::vector<uint8_t> item_ids_;
	// std::vector<uint16_t> item_vals_;
};

////////////////////////////////////////////////////
// SetDevInfo
class ZInnerSetDevInfoReq : public ZInnerMsg {
 public:
	ZInnerSetDevInfoReq(const ZInnerAddress &src_addr)
	  : ZInnerMsg(src_addr, Z_ZB_SET_DEV_REQ)
	{
	}

 public:
	uint16_t addr_;
	std::vector<ZItemPair> dev_vals_;
};

class ZInnerSetDevInfoRsp : public ZInnerMsg {
 public:
	ZInnerSetDevInfoRsp(const ZInnerAddress &src_addr)
	  : ZInnerMsg(src_addr, Z_ZB_SET_DEV_RSP)
	{
	}

 public:
	uint8_t status_;
};

#endif // _Z_INNER_MESSAGE_H__


