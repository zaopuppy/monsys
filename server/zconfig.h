#ifndef _Z_CONFIG_H__
#define _Z_CONFIG_H__

// in millisecond
const int ROUTINE_INTERVAL = 1000;

// push server
const char *PUSH_SERVER_IP = "0.0.0.0";
const int PUSH_SERVER_PORT = 1984;

// web api server
const char *WEB_API_SERVER_IP = "0.0.0.0";
const int WEB_API_SERVER_PORT = 1983;

// echo server
const char *ECHO_SERVER_IP = "0.0.0.0";
const int ECHO_SERVER_PORT = 4444;


class Config {
public:
  Config() {}

public:
  int init(const char *filename);

};

#endif // _Z_CONFIG_H__

