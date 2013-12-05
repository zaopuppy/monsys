#ifndef _Z_ERRNO_H__
#define _Z_ERRNO_H__

enum ERR_CODE {
  NO_ENOUGH_BUFFER = -3,
  ERR_IO_PENDING,
  FAIL = -1,
  OK = 0,
};

#define ECHECK(_rv, _code) assert((_rv) == (_code))

#endif // _Z_ERRNO_H__

