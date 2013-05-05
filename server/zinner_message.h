#ifndef _Z_INNER_MESSAGE_H__
#define _Z_INNER_MESSAGE_H__

// --- temporary using
struct ZData {
	int data;
};

enum {
	Z_INVALID = -1,
	Z_ZB_GET_DEV_REQ,
};

struct ZInnerAddress {
	int moduleType;
	int moduleId;

	ZInnerAddress(int type, int id): moduleType(type), moduleId(id) {}
	ZInnerAddress(): moduleType(-1), moduleId(-1) {}
	bool isValid() {
		return (moduleType >= 0) && (moduleId >= 0);
	}
};

struct ZInnerMsg {
	// dest address
	ZInnerAddress addr;
	// TODO: src address
	// ZInnerAddress srcAddr;
	// XXX: no delet method
	void* data;
	int msgType;

	ZInnerMsg(int moduleType, int moduleId): data(NULL) {
		addr.moduleType = moduleType;
		addr.moduleId = moduleId;
	}
};

#endif // _Z_INNER_MESSAGE_H__


