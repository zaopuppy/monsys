#ifndef _Z_TIME_H__
#define _Z_TIME_H__

#include <sys/time.h>
#include <assert.h>

class ZTime {
 public:
	static long getInMillisecond() {
		struct timeval tv;
		int rv = gettimeofday(&tv, NULL);
		if (rv != 0) {
			assert(false);
			return 0xFFFFFF;
		}

		// printf("tv_sec: [%ld]\n", tv.tv_sec);
		// printf("tv_uec: [%d]\n", tv.tv_usec);

		return tv.tv_sec*1000 + (tv.tv_usec/1000);
	}
 // private:
 // 	struct timeval tv_;
};

#endif

