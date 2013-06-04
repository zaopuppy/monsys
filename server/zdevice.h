#ifndef _Z_DEVICE_H__
#define _Z_DEVICE_H__

#include <iostream>
#include <map>

const uint32_t ZB_MAC_LEN = 8;

typedef uint16_t zb_addr_type_t;
typedef struct {
	char data[8];
} zb_mac_type_t;

enum zb_dev_state {
	zb_dev_state_invalid = -1,
	zb_dev_state_active,
	zb_dev_state_inactive,
};

class ZZBDevInfo {
 public:
 	ZZBDevInfo() {
 		reset();
 	}

 	ZZBDevInfo(const ZZBDevInfo &other) {
 		addr = other.addr;
 		name = other.name;
 		state = other.state;
 		memcpy(&mac, &other.mac, sizeof(mac));
 	}

 	void reset() {
 		addr = 0x00;
 		name = "NA";
 		state = zb_dev_state_invalid;
 		memset(&mac, 0x00, sizeof(mac));
 	}

 public:
	zb_addr_type_t addr;
	std::string name;
	int state;
	// char mac[ZB_MAC_LEN];
	zb_mac_type_t mac;
};

class ZBMacComp {
 public:
 	bool operator()(const zb_mac_type_t &v1, const zb_mac_type_t &v2) {
 		for (unsigned int i = 0; i < sizeof(v1.data); ++i)	{
 			if (v1.data[i] < v2.data[i]) {
 				return true;
 			} else if (v1.data[i] > v2.data[i])	{
 				return false;
 			}
 		}

 		return false;
 	}
};

class ZDevManager {
 public:
	ZDevManager() { }
	~ZDevManager() {}

 	// typedef
 	typedef std::map<zb_mac_type_t, ZZBDevInfo*, ZBMacComp> MAC_DEV_MAP_TYPE;
 	typedef std::map<zb_addr_type_t, ZZBDevInfo*> ADDR_DEV_MAP_TYPE;

 public:
 	void reset();

 	ZZBDevInfo* find(const zb_addr_type_t addr) {
 		ADDR_DEV_MAP_TYPE::iterator iter = addr_dev_map_.find(addr);
 		if (iter == addr_dev_map_.end()) {
 			return NULL;
 		}
 		return iter->second;
 	}

 	ZZBDevInfo* find(const zb_mac_type_t &mac) {
 		MAC_DEV_MAP_TYPE::iterator iter = mac_dev_map_.find(mac);
 		if (iter == mac_dev_map_.end()) {
 			return NULL;
 		}
 		return iter->second;
 	}

 	// add or update
 	bool add(zb_mac_type_t &mac, zb_addr_type_t addr);

 	const MAC_DEV_MAP_TYPE& getMacDevMap() { return mac_dev_map_; }

 private:
	// MAC --> dev*
	MAC_DEV_MAP_TYPE mac_dev_map_;
	// addr --> dev*
	ADDR_DEV_MAP_TYPE addr_dev_map_;
};

#endif

