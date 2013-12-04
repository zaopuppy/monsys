#include "fgw_manager.h"

#include "libbase/zlog.h"

#include "libframework/zframework.h"

void FGWManager::add_handler(std::string name, int handler_id)
{
  Z_LOG_D("FGWManager::add_handler(%s, %d)", name.c_str(), handler_id);

  // TODO: duplication checking
  fgw_handler_map_[name] = handler_id;
}

void FGWManager::remove_handler(std::string name)
{
  Z_LOG_D("FGWManager::remove_handler(%s)", name.c_str());

  FGW_MAP_TYPE::iterator iter = fgw_handler_map_.find(name);
  if (iter != fgw_handler_map_.end()) {
    fgw_handler_map_.erase(iter);
  }
}

int FGWManager::find_handler(std::string name)
{
  Z_LOG_D("FGWManager::find_handler(%s)", name.c_str());

  FGW_MAP_TYPE::iterator iter = fgw_handler_map_.find(name);
  if (iter == fgw_handler_map_.end()) {
    return INVALID_ID;
  }

  int handler_id = iter->second;

  return handler_id;
}

