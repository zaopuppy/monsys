#include "push_message.h"

#include <string.h>

#include <jansson.h>

#include "libbase/zlog.h"

// { "cmd": "HB" }
ZInnerMsg* convertHB(json_t *jroot)
{
	return new ZPushHBMsg();
}

ZInnerMsg* json2Inner(json_t *jroot)
{
	json_t *jcmd = json_object_get(jroot, "cmd");
	if (!jcmd || !json_is_string(jcmd)) {
		Z_LOG_E("Missing 'cmd' field, or 'cmd' is not a string\n");
		return NULL;
	}

	// 3. check session
	const char *cmd_str = json_string_value(jcmd);
	if (!strncmp(cmd_str, "HB", sizeof("HB") - 1)) {
		return convertHB(jroot);
	} else {
		return NULL;
	}
}

ZInnerMsg* decodePushMsg(char *buf, uint32_t buf_len)
{
	json_error_t jerror;
	json_t *jroot = json_loadb(buf, buf_len, 0, &jerror);
	if (jroot == NULL || !json_is_object(jroot)) {
		Z_LOG_E("Bad request\n");
		return NULL;
	}

	return json2Inner(jroot);
}


