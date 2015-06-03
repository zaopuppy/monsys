#include "zconfig.h"

#include <fstream>

using namespace std;

bool Config::load(const char *filename)
{
  if (!filename) {
    Z_LOG_E("NULL pointer");
    return false;
  }

  ifstream in;
  in.open(filename);
  if (!in.is_open() || !in.good()) {
    Z_LOG_E("Failed to open file: %s", filename);
    return false;
  }

  char line[1024];
  char *key, *val;

  bool result = true;
  while (in.getline(line, sizeof(line))) {
    Z_LOG_D("read line: [%s]", line);

    key = line;

    // skip comment
    while (*key == ' ' || *key == '\t') { ++key; }
    if (*key == '#') {
      Z_LOG_D("comment, skip");
      continue;
    }

    val = strchr(key, '=');

    // no '=' was found
    if (NULL == val) {
      Z_LOG_E("Bad line, no '=' was found");
      result = false;
      break;
    }

    // no key was found
    if (key == val) {
      Z_LOG_E("Bad line, no key before '='");
      result = false;
      break;
    }

    
  }


  return result;
}


