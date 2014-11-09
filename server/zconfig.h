#ifndef _Z_CONFIG_H__
#define _Z_CONFIG_H__

#include <string>

// in millisecond
const int ROUTINE_INTERVAL = 1000;

// push server
const char * const PUSH_SERVER_IP = "0.0.0.0";
const int PUSH_SERVER_PORT = 1984;

// web api server
const char * const WEB_API_SERVER_IP = "0.0.0.0";
const int WEB_API_SERVER_PORT = 1983;

// echo server
const char * const ECHO_SERVER_IP = "0.0.0.0";
const int ECHO_SERVER_PORT = 4444;

// FGW client
const int FGW_CLIENT_SESSION_TIMEOUT = 10*1000;

// ZIGBEE client
const int ZIGBEE_CLIENT_SESSION_TIMEOUT = 8*1000;


class Config {
  public:
    static Config* instance() {
      static Config config;
      return &config;
    }

private:
  Config() {}

public:
  bool load(const char *filename);
  int getInt(std::string key, int default_val);
  std::string getStr(std::string key, std::string default_val);

};

#endif // _Z_CONFIG_H__

