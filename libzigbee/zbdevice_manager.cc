#include "zbdevice_manager.h"

#include <assert.h>

#include "libbase/zlog.h"

void ZZBDevManager::reset()
{
  ///////////////////////////////////////////
  // MAC mapping
  MAC_DEV_MAP_TYPE::iterator iter = mac_dev_map_.begin();
  // typedef MAC_DEV_MAP_TYPE::iterator tmp_iter;

  for (; iter != mac_dev_map_.end(); ++iter) {
    delete iter->second;
  }

  mac_dev_map_.clear();

  //////////////////////////////////////////
  // ADDR mapping
  // DON'T do anything, cause previous operation has freed all memory
  addr_dev_map_.clear();
}

bool ZZBDevManager::add(zb_mac_type_t &mac,
  zb_addr_type_t addr, const char *name, uint16_t type/*uint8_t id_count*/)
{
  Z_LOG_D("ZZBDevManager::add(%u)", addr);

  ZZBDevInfo *info_from_mac = find(mac);
  ZZBDevInfo *info_from_addr = find(addr);

  if (info_from_mac && !info_from_addr) {
    Z_LOG_I("mac already exists, but addr is changed.");

    info_from_mac->name_ = name;
    // info_from_mac->id_count_ = id_count;
    info_from_mac->type_ = type;

    // addr changed, check and update
    ADDR_DEV_MAP_TYPE::iterator iter = addr_dev_map_.find(info_from_mac->addr_);
    assert(iter != addr_dev_map_.end());
    assert(iter->second == info_from_mac);
    addr_dev_map_.erase(iter);

    info_from_mac->addr_ = addr;
    addr_dev_map_[info_from_mac->addr_] = info_from_mac;

    return true;

  } else if (!info_from_mac && info_from_addr) {
    // XXX: should we kick the old one out?
    // addr has been token, this should not happen
    Z_LOG_I("address has been token, huh?\n");

    return false;

  } else if (info_from_mac && info_from_addr) {
    Z_LOG_I("mac and addr are already exist, update info only");

    // XXX
    assert(info_from_mac == info_from_addr);

    // exist, check and update
    info_from_mac->name_ = name;
    info_from_mac->type_ = type;
    // info_from_mac->id_count_ = id_count;

    return true;

  } else { // !info_from_mac && !info_from_addr
    Z_LOG_I("no mac or addr is found, add as new one");

    // good, it's empty, just add it
    info_from_mac = new ZZBDevInfo();
    info_from_mac->addr_ = addr;
    info_from_mac->name_ = name;
    info_from_mac->type_ = type;
    // info_from_mac->id_count_ = id_count;
    memcpy(&info_from_mac->mac_, &mac, sizeof(mac));

    // don't update state
    info_from_mac->state_ = ZB_DEV_STATE_ACTIVE;

    // add to map
    addr_dev_map_[addr] = info_from_mac;
    mac_dev_map_[mac] = info_from_mac;

    return true;

  }
}


