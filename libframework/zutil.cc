#include "zutil.h"

#include <string.h>

bool
isBlank(const char *str, uint32_t str_len)
{
	while (str_len > 0) {
		if (*str != ' ' && *str != '\t' && *str != '\r' && *str != '\n') {
			return false;
		}
		++str;
	}
	return true;
}

int ZStringUtil::atoi(const char *str, int def, int begin, int end/*, int base */)
{

	if (str[0] == 0x00) {
		// error
		return def;
	}

	if (end < 0) {
		end = strlen(str) - 1;
	}

	if (end < begin) {
		// error
		return def;
	}

	int val = 0;
	int tmp;

	while (true) {
		tmp = str[begin] - '0';
		if (tmp < 0 || tmp > 9) {
			return def;
		}
		val += tmp;
		++begin;
		if (begin > end) {
			break;
		}
		val *= 10;
	}

	return val;
}


