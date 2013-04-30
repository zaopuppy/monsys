#include "zutil.h"

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



