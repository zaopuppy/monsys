#ifndef _Z_UTIL_H__
#define _Z_UTIL_H__

#include <stdint.h>
#include <string.h>

bool isBlank(const char *str, uint32_t str_len);

class ZStringUtil
{
 public:
 	static int atoi(const char *str, int def, int begin = 0, int end = -1/*, int base = 10 */);
};

// a,b,c,d
// a=1, b=2, c=3
inline int val(const char *str, int str_len, int &v)
{
	if (str_len <= 0) {
		return -1;
	}

	int idx = 0;

	while (str[idx] >= '0' && str[idx] <= '9' && idx < (int)str_len) {
		++idx;
	}

	--idx;

	v = ZStringUtil::atoi(str, -1, 0, idx);

	return idx + 1;
}

template<typename T_List>
inline bool str2list(const char *str, T_List &list)
{
	int str_len = (int)strlen(str);
	int len = str_len;
	int v;

	int lastpos;

	do {
		lastpos = val(str, str_len, v);
		list.push_back(v);
		lastpos += 1;	// skip ','
		str += lastpos;
		len -= lastpos;
	} while (lastpos > 0 && len > 0);

	if (len > 0) {
		return false;
	}

	return true;
}


#endif // _Z_UTIL_H__


