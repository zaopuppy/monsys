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

	ZInnerAddress& operator=(const ZInnerAddress &other) {
		if (&other == this) {
			return *this;
		}

		module_type_ = other.module_type_;
		module_id_ = other.module_id_;
		handler_id_ = other.handler_id_;

		return *this;
	}
};

/////////////////////////////////////////////////////
class ZInnerMsg {
 public:
	ZInnerMsg(uint32_t msg_type): msg_type_(msg_type)
	{
		// TODO:
		static uint32_t sequence = 1;
		seq_ = sequence++;
		if (sequence > 0xFFFFFFF) {
			sequence = 1;
		}
	}

 // TODO: make it protected
 public:
	ZInnerAddress src_addr_;
	ZInnerAddress dst_addr_;
	uint32_t msg_type_;
	uint32_t seq_;
};

#endif // _Z_INNER_MESSAGE_H__


