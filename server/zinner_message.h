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
	int moduleType;
	int moduleId;
	int handlerId;

	ZInnerAddress(int type, int id, int hid)
		: moduleType(type), moduleId(id), handlerId(hid)
	{}

	ZInnerAddress(const ZInnerAddress &addr)
		: moduleType(addr.moduleType),
		moduleId(addr.moduleId), handlerId(addr.handlerId)
	{}

	ZInnerAddress()
		: moduleType(-1), moduleId(-1), handlerId(-1)
	{}

	bool isValid() {
		return (moduleType >= 0) && (moduleId >= 0) && (handlerId >= 0);
	}
};

/////////////////////////////////////////////////////
class ZInnerMsg {
 public:
	 ZInnerMsg(const ZInnerAddress &src_addr)
		 : src_addr_(src_addr), seq_(0)
	 {
	 }

 public:
	 uint32_t getMsgType() { return msg_type_; }
	 void setMsgType(uint32_t msg_type) { msg_type_ = msg_type; }
	 uint32_t getSeq() { return seq_; }
	 void setSeq(uint32_t seq) { seq_ = seq; }
	 const ZInnerAddress& getSrcAddr() { return src_addr_; }

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
		: ZInnerMsg(src_addr)
	{
		setMsgType(Z_ZB_GET_DEV_LIST_REQ);
	}

};

class ZInnerGetDevListRsp : public ZInnerMsg {
 public:
 	ZInnerGetDevListRsp(const ZInnerAddress &src_addr)
 		: ZInnerMsg(src_addr)
 	{
 		setMsgType(Z_ZB_GET_DEV_LIST_RSP);
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
	  : ZInnerMsg(src_addr)
	{
		setMsgType(Z_ZB_GET_DEV_REQ);
	}

 public:
	uint16_t addr_;
	std::vector<uint8_t> item_ids_;
};

class ZInnerGetDevInfoRsp : public ZInnerMsg {
 public:
	ZInnerGetDevInfoRsp(const ZInnerAddress &src_addr)
	  : ZInnerMsg(src_addr)
	{
		setMsgType(Z_ZB_GET_DEV_RSP);
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
	  : ZInnerMsg(src_addr)
	{
		setMsgType(Z_ZB_SET_DEV_REQ);
	}

 public:
	uint16_t addr_;
	std::vector<ZItemPair> dev_vals_;
};

class ZInnerSetDevInfoRsp : public ZInnerMsg {
 public:
	ZInnerSetDevInfoRsp(const ZInnerAddress &src_addr)
	  : ZInnerMsg(src_addr)
	{
		setMsgType(Z_ZB_SET_DEV_RSP);
	}

 public:
	uint8_t status_;
};

#endif // _Z_INNER_MESSAGE_H__


