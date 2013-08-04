#ifndef _Z_ERRNO_H__
#define _Z_ERRNO_H__

enum ERR_CODE {
	OK = 0,
	ERR_IO_PENDING = -1,
	FAIL = -2,
};

#define ECHECK(_rv, _code) assert((_rv) == (_code))

#endif // _Z_ERRNO_H__

