#ifndef _Z_TIME_H__
#define _Z_TIME_H__

#include <sys/time.h>
#include <assert.h>

class ZTime {
 public:
	static long getMillisecond() {
		struct timeval tv;
		int rv = gettimeofday(&tv, NULL);
		if (rv != 0) {
			assert(false);
			return 0xFFFFFF;
		}

		return tv.tv_sec*1000 + (tv.tv_usec/1000);
	}

};

#endif

