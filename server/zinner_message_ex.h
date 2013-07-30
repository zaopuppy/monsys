#ifndef Z__INNER_MESSAGE_EX_H__
#define Z__INNER_MESSAGE_EX_H__

#include "framework/zframework.h"

#include "zzigbee_message.h"
#include "zdevice.h"

enum {
	Z_INVALID = -1,

	Z_ZB_GET_DEV_LIST_REQ,
	Z_ZB_GET_DEV_LIST_RSP,

	Z_ZB_GET_DEV_REQ,
	Z_ZB_GET_DEV_RSP,

	Z_ZB_SET_DEV_REQ,
	Z_ZB_SET_DEV_RSP,
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


#endif // _Z_INNER_MESSAGE_EX_H__

