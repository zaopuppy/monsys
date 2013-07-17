#ifndef _Z_DEVICE_H__
#define _Z_DEVICE_H__

#include <iostream>
#include <map>
#include <vector>

const uint32_t MAX_ID_PER_ZB_DEV = 256;

typedef uint16_t zb_addr_type_t;
typedef struct {
	char data[8];
} zb_mac_type_t;

enum zb_item_id_type {
	zb_item_id_type_invalid = 0,
	zb_item_id_type_s8,
};

// struct ItemIdInfo {
typedef struct zb_item_id_info {
	uint8_t 			id_;					// 0x00 and 0xFF are special, never use it
	std::string 	name_;
	std::string 	desc_;
	uint8_t 			type_;				// 0: invalid
	std::string 	formatter_;
	time_t 				last_update_time_;	// XXX: not using yet

	void clone(const zb_item_id_info &other) {
		id_   						= other.id_;
		name_ 						= other.name_;
		desc_ 						= other.desc_;
		type_ 						= other.type_;
		formatter_ 				= other.formatter_;
		last_update_time_ 	= other.last_update_time_;
	}

	void reset() {
		id_ 							= 0;
		name_ 						= "-";
		desc_ 						= "-";
		type_ 						= zb_item_id_type_invalid;
		formatter_ 				= "";
		last_update_time_ 	= 0;
	}

	void print() {
		printf("id:        [%u]\n", id_);
		printf("name:      [%s]\n", name_.c_str());
		printf("desc:      [%s]\n", desc_.c_str());
		printf("type:      [%u]\n", type_);
		printf("formatter: [%s]\n", formatter_.c_str());
		printf("last_update_time: [%ld]\n", last_update_time_);
	}
} zb_item_id_info_t;

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
 		addr_ 		= other.addr_;
 		name_ 		= other.name_;
 		state_ 		= other.state_;
 		type_ 		= other.type_;

 		// id_count_ = other.id_count_;
 		memcpy(&mac_, &other.mac_, sizeof(mac_));

 		// for (uint32_t i = 0; i < MAX_ID_PER_ZB_DEV; ++i) {
 		// 	id_info_list_[i].clone(other.id_info_list_[i]);
 		// }
 	}

 	void reset() {
 		addr_ = 0x00;
 		name_ = "NA";
 		state_ = zb_dev_state_invalid;
 		// id_count_ = 0;
 		memset(&mac_, 0x00, sizeof(mac_));
 	}

 public:
	zb_addr_type_t addr_;
	std::string name_;
	int state_;
	// char mac[ZB_MAC_LEN];
	// uint8_t id_count_;
	zb_mac_type_t mac_;
	// zb_item_id_info_t id_info_list_[MAX_ID_PER_ZB_DEV];
	// std::vector<zb_item_id_info_t*> id_info_list;
	uint16_t type_;
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
 	bool add(zb_mac_type_t &mac, zb_addr_type_t addr, const char *name, uint16_t type/*uint8_t id_count*/);

 	const MAC_DEV_MAP_TYPE& getMacDevMap() { return mac_dev_map_; }

 private:
	// MAC --> dev*
	MAC_DEV_MAP_TYPE mac_dev_map_;
	// addr --> dev*
	ADDR_DEV_MAP_TYPE addr_dev_map_;
};

#endif

