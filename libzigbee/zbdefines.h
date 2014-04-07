#ifndef _ZB_DEFINES_H__
#define _ZB_DEFINES_H__

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libbase/ztypes.h"

const uint32_t MAX_ID_PER_ZB_DEV = 256;

const uint8_t Z_ID_ZB_REG_REQ = 0x01;
const uint8_t Z_ID_ZB_REG_RSP = 0x81;

const uint8_t Z_ID_ZB_GET_REQ = 0x02;
const uint8_t Z_ID_ZB_GET_RSP = 0x82;

const uint8_t Z_ID_ZB_SET_REQ = 0x03;
const uint8_t Z_ID_ZB_SET_RSP = 0x83;

const uint8_t Z_ID_ZB_BROADCAST_IND = 0x04;

const uint8_t Z_ID_ZB_BIND_REQ = 0x05;
const uint8_t Z_ID_ZB_BIND_RSP = 0x85;

const uint8_t Z_ID_ZB_UPDATE_ID_REQ = 0x0A;
const uint8_t Z_ID_ZB_UPDATE_ID_RSP = 0x8A;

enum enZBBroadcastWhat {
  ZB_BROADCAST_WHAT_INVALID = 0x00,
  ZB_BROADCAST_WHAT_FGW_CONNECTED = 1,
};

// enum ITEM_ID {
//  ITEM_ID_ALL = 0,
//  ITEM_ID_DEV_TYPE = 1,
// };

enum enZB_ITEM_ID_TYPE {
  ZB_ITEM_ID_TYPE_INVALID = 0,
  ZB_ITEM_ID_TYPE_S8,
};

enum enZB_DEV_STATE {
  ZB_DEV_STATE_INVALID = -1,
  ZB_DEV_STATE_ACTIVE,
  ZB_DEV_STATE_INACTIVE,
};

typedef struct ZZBHeader {
  uint8_t  syn_;
  uint8_t  ver_;
  uint16_t len_;
  uint8_t  cmd_;
  uint16_t addr_;
} zb_header_t;

typedef struct ZItemPair {
  uint8_t id;
  uint16_t val;
} zb_item_pair_t;

typedef union {
  uint16_t int_val;
  char str_val[1];
} zb_dev_item_val_t;

typedef uint16_t zb_addr_type_t;

typedef struct {
  char data[8];
} zb_mac_type_t;

typedef struct zb_item_id_info {
  uint8_t       id_;          // 0x00 and 0xFF are special, never use it
  std::string   name_;
  std::string   desc_;
  uint8_t       type_;        // 0: invalid
  std::string   formatter_;
  time_t        last_update_time_;  // XXX: not using yet

  void clone(const zb_item_id_info &other) {
    id_               = other.id_;
    name_             = other.name_;
    desc_             = other.desc_;
    type_             = other.type_;
    formatter_        = other.formatter_;
    last_update_time_   = other.last_update_time_;
  }

  void reset() {
    id_               = 0;
    name_             = "-";
    desc_             = "-";
    type_             = ZB_ITEM_ID_TYPE_INVALID;
    formatter_        = "";
    last_update_time_   = 0;
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

class ZZBDevInfo {
 public:
  ZZBDevInfo() {
    reset();
  }

  ZZBDevInfo(const ZZBDevInfo &other) {
    addr_     = other.addr_;
    name_     = other.name_;
    state_    = other.state_;
    type_     = other.type_;

    // id_count_ = other.id_count_;
    memcpy(&mac_, &other.mac_, sizeof(mac_));

    // for (uint32_t i = 0; i < MAX_ID_PER_ZB_DEV; ++i) {
    //  id_info_list_[i].clone(other.id_info_list_[i]);
    // }
  }

  void reset() {
    addr_ = 0x00;
    name_ = "NA";
    state_ = ZB_DEV_STATE_INVALID;
    // id_count_ = 0;
    memset(&mac_, 0x00, sizeof(mac_));
  }

 public:
  zb_addr_type_t addr_;
  std::string name_;
  int state_;
  zb_mac_type_t mac_;
  uint16_t type_;
};

class ZBMacComp {
 public:
  bool operator()(const zb_mac_type_t &v1, const zb_mac_type_t &v2) const {
    for (unsigned int i = 0; i < sizeof(v1.data); ++i)  {
      if (v1.data[i] < v2.data[i]) {
        return true;
      } else if (v1.data[i] > v2.data[i]) {
        return false;
      }
    }

    return false;
  }
};

#endif // _ZB_DEFINES_H__


