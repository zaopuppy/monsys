#ifndef Z__INNER_MESSAGE_EX_H__
#define Z__INNER_MESSAGE_EX_H__

#include "libframework/zframework.h"

#include "libzigbee/zzigbee_message.h"
#include "libzigbee/zbdefines.h"

enum {
	Z_INVALID = -1,

	Z_TRANSPORT_MSG,

	Z_ZB_GET_DEV_LIST_REQ,
	Z_ZB_GET_DEV_LIST_RSP,

	Z_ZB_GET_DEV_REQ,
	Z_ZB_GET_DEV_RSP,

	Z_ZB_SET_DEV_REQ,
	Z_ZB_SET_DEV_RSP,
};

////////////////////////////////////////////////////
// Transport
class ZTransportMsg : public ZInnerMsg {
 public:
 	ZTransportMsg()
 		: ZInnerMsg(Z_TRANSPORT_MSG), data_(NULL)
	{
	}
	~ZTransportMsg() {
		if (data_) {
			delete []data_;
			data_ = NULL;
		}
	}

 public:
 	void set(char *data, uint32_t data_len) {
 		if (data_) {
 			delete []data_;
 			data_ = NULL;
 		}
 		data_len_ = data_len;
 		data_ = new char[data_len_];
 		memcpy(data_, data, data_len_);
 	}

 public:
 	char *data_;
 	uint32_t data_len_;
};

////////////////////////////////////////////////////
// GetDevList
class ZInnerGetDevListReq : public ZInnerMsg {
 public:
	ZInnerGetDevListReq()
		: ZInnerMsg(Z_ZB_GET_DEV_LIST_REQ)
	{
	}

};

class ZInnerGetDevListRsp : public ZInnerMsg {
 public:
 	ZInnerGetDevListRsp()
 		: ZInnerMsg(Z_ZB_GET_DEV_LIST_RSP)
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
	ZInnerGetDevInfoReq()
	  : ZInnerMsg(Z_ZB_GET_DEV_REQ)
	{
	}

 public:
	uint16_t addr_;
	std::vector<uint8_t> item_ids_;
};

class ZInnerGetDevInfoRsp : public ZInnerMsg {
 public:
	ZInnerGetDevInfoRsp()
	  : ZInnerMsg(Z_ZB_GET_DEV_RSP)
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
	ZInnerSetDevInfoReq()
	  : ZInnerMsg(Z_ZB_SET_DEV_REQ)
	{
	}

 public:
	uint16_t addr_;
	std::vector<ZItemPair> dev_vals_;
};

class ZInnerSetDevInfoRsp : public ZInnerMsg {
 public:
	ZInnerSetDevInfoRsp()
	  : ZInnerMsg(Z_ZB_SET_DEV_RSP)
	{
	}

 public:
	uint8_t status_;
};


#endif // _Z_INNER_MESSAGE_EX_H__

