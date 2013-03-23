#ifndef _Z_INNER_MESSAGE_H__
#define _Z_INNER_MESSAGE_H__

// --- temporary using
struct ZData {
	int data;
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
	void* data;

	ZInnerMsg(int type, int id): data(NULL) {
		addr.moduleType = type;
		addr.moduleId = id;
	}
};

#endif // _Z_INNER_MESSAGE_H__


