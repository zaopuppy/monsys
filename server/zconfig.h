#ifndef _Z_CONFIG_H__
#define _Z_CONFIG_H__

// in millisecond
const int ROUTINE_INTERVAL    = 1000;

class Config {
public:
  Config() {}

public:
  int init(const char *filename);

};

#endif // _Z_CONFIG_H__

