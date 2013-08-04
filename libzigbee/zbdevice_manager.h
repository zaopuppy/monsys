#ifndef _ZB_DEVICE_MANAGER_H__
#define _ZB_DEVICE_MANAGER_H__

#include "zbdefines.h"

#include <map>

class ZZBDevManager {
 public:
	ZZBDevManager() { }
	~ZZBDevManager() {}

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
 	bool add(zb_mac_type_t &mac, zb_addr_type_t addr, const char *name, uint16_t type/*uint8_t id_count*/);

 	const MAC_DEV_MAP_TYPE& getMacDevMap() { return mac_dev_map_; }

 private:
	// MAC --> dev*
	MAC_DEV_MAP_TYPE mac_dev_map_;
	// addr --> dev*
	ADDR_DEV_MAP_TYPE addr_dev_map_;
};



#endif // _ZB_DEVICE_MANAGER_H__


