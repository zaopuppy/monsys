#include "zwebapi_session.h"

#include <string.h>

uint32_t ZWebApiSession::sequence_ = 0x00;

int ZWebApiSession::event(evutil_socket_t fd, char *buf, uint32_t buf_len)
{
	int result = -1;
	
	switch (state_) {
	case STATE_INIT:
		{
			json_error_t error;
			json_t *jobj = json_loadb(buf, buf_len, 0, &error);
			if ((jobj == NULL) || (!json_is_object(jobj))) {
				Z_LOG_D("bad request");
				sendRsp(fd, "bad request", 400);
				return -1;
			}

			result = processJson(fd, jobj);

			// release reference
			json_decref(jobj);

			state_ = STATE_WAIT_FOR_ZB_RSP;

			break;
		}
	case STATE_WAIT_FOR_ZB_RSP:
		{
			state_ = STATE_FINISHED;
			break;
		}
	default:
		break;
	}
	
	return result;
}

void ZWebApiSession::sendRsp(evutil_socket_t fd, const char *text_msg, int status)
{
	send(fd, text_msg, strlen(text_msg), 0);
}

int ZWebApiSession::processJson(evutil_socket_t fd, json_t *root)
{
	Z_LOG_D("ZWebApiSession::processJson");
	// var req_obj = {        
	// 	"cmd": "get-dev-info",
	// 	"uid": "uid001",      
	// 	"dev-id": "5000",     
	// };                     
	json_t *cmd = json_object_get(root, "cmd");
	if (!cmd || !json_is_string(cmd)) {
		Z_LOG_D("Missing 'cmd' field, or 'cmd' is not a string");
		return -1;
	}

	const char *cmd_str = json_string_value(cmd);

	if (strncmp(cmd_str, "get-dev-info", sizeof("get-dev-info") - 1)) {
		Z_LOG_D("Unknown command");
		sendRsp(fd, "Unknown command", 400);
		return -1;
	}

	json_t *uid = json_object_get(root, "uid");
	if (!uid || !json_is_integer(uid)) {
		Z_LOG_D("uid is illegal");
		sendRsp(fd, "uid is illegal", 400);
		return -1;
	}

	json_t *dev_id = json_object_get(root, "dev-id");
	if (!dev_id || !json_is_integer(dev_id)) {
		Z_LOG_D("dev-id is illegal");
		sendRsp(fd, "dev-id is illegal", 400);
		return -1;
	}

	Z_LOG_D("uid: %d", (int)json_integer_value(uid));
	Z_LOG_D("dev-id: %d", (int)json_integer_value(dev_id));

	sendRsp(fd, "uid is good, dev-id is good, everything is good:)\n", 200);

	return 0;
}






