#ifndef _FGW_MANAGER_H__
#define _FGW_MANAGER_H__

#include <map>
#include <iostream>

class FGWManager {

public:
  static FGWManager* instance() {
    static FGWManager manager;
    return &manager;
  }

private:
  FGWManager() {}

public:
  void add_handler(std::string name, int handler_id);
  void remove_handler(std::string name);
  int find_handler(std::string name);

  typedef std::map<std::string, int> FGW_MAP_TYPE;

private:
  FGW_MAP_TYPE fgw_handler_map_;
};

#endif // _FGW_MANAGER_H__

