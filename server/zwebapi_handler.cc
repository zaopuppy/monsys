#include "zwebapi_handler.h"

#include <unistd.h>
#include <assert.h>
#include <errno.h>

#include "zerrno.h"
#include "zlog.h"
#include "zdispatcher.h"
#include "zutil.h"

#include "zmodule.h"
#include "zzigbee_message.h"

//////////////////////////////////////////////////////
int ZWebApiHandler::init() {
	printf("Oops, client's coming\n");

	return 0;
}

void ZWebApiHandler::close() {
	::close(fd_);
	fd_ = -1;
	event_del(read_event_);
}

int ZWebApiHandler::processMsg(ZInnerGetDevInfoRsp *msg)
{
	printf("ZWebApiHandler::processMsg(GetDevInfoRsp)\n");

	// no session
	printf("Z_ZB_GET_DEV_RSP\n");
	// const char *str = "{
	// 	\"cmd\": \"get-dev-info-rsp\",
	// 	\"code\": -4,
	// 	\"info\": [
	// 		{ \"id\": 1, \"status\": 2, \"desc\": \"OK0\" },
	// 		{ \"id\": 2, \"status\": 3, \"desc\": \"OK1\" },
	// 		{ \"id\": 3, \"status\": 4, \"desc\": \"OK3\" }
	// 	]
	// }";

	// ZZBGetRsp *rsp = (ZZBGetRsp*)msg->data;

	do {
		int rv;

		json_t *jobj = json_object();
		assert(jobj);

		json_t *cmd = json_string("get-dev-info-rsp");
		rv = json_object_set_new(jobj, "cmd", cmd);

		json_t *code;
		if (msg->dev_infos_.size() > 0) {
			code = json_integer(0);
		} else {
			code = json_integer(-1);
		}

		rv = json_object_set_new(jobj, "code", code);

		json_t *info = json_array();

		for (uint32_t i = 0; i < msg->dev_infos_.size(); ++i) {
			json_t *obj1 = json_object();

			json_t *id1 = json_integer(msg->dev_infos_[i].id);
			json_object_set_new(obj1, "id", id1);

			json_t *status1 = json_integer(msg->dev_infos_[i].val);
			json_object_set_new(obj1, "status", status1);

			json_t *desc1 = json_string("OK0");
			json_object_set_new(obj1, "desc", desc1);

			json_array_append_new(info, obj1);
		}

		json_object_set_new(jobj, "info", info);

		char *str_dump = json_dumps(jobj, 0);

		sendRsp(str_dump, 200);

		free(str_dump);
		json_decref(jobj);

	} while (0);

	return 0;
}

// int processMsg(ZInnerSetDevInfoRsp *msg)
// {
// 	printf("ZWebApiHandler::processMsg(SetDevInfoRsp)\n");
// 	printf("Z_ZB_SET_DEV_RSP\n");
// 	// const char *str = "{
// 	// 	\"cmd\": \"set-dev-info-rsp\",
// 	// 	\"code\": -4,
// 	// 	\"info\": [
// 	// 		{ \"id\": 1, \"status\": 2, \"desc\": \"OK0\" },
// 	// 		{ \"id\": 2, \"status\": 3, \"desc\": \"OK1\" },
// 	// 		{ \"id\": 3, \"status\": 4, \"desc\": \"OK3\" }
// 	// 	]
// 	// }";
// 
// 	// ZZBGetRsp *rsp = (ZZBGetRsp*)msg->data;
// 
// 	do {
// 		int rv;
// 
// 		json_t *jobj = json_object();
// 		assert(jobj);
// 
// 		json_t *cmd = json_string("set-dev-info-rsp");
// 		rv = json_object_set_new(jobj, "cmd", cmd);
// 
// 		json_t *code = json_integer(-4);
// 		rv = json_object_set_new(jobj, "code", code);
// 
// 		json_t *info = json_array();
// 		// 1
// 		{
// 			json_t *obj1 = json_object();
// 
// 			json_t *id1 = json_integer(1);
// 			json_object_set_new(obj1, "id", id1);
// 
// 			json_t *status1 = json_integer(0);
// 			json_object_set_new(obj1, "status", status1);
// 
// 			json_t *desc1 = json_string("OK0");
// 			json_object_set_new(obj1, "desc", desc1);
// 
// 			json_array_append_new(info, obj1);
// 		}
// 
// 		json_object_set_new(jobj, "info", info);
// 
// 		char *str_dump = json_dumps(jobj, 0);
// 
// 		sendRsp(str_dump, 200);
// 
// 		free(str_dump);
// 		json_decref(jobj);
// 
// 	} while (0);
// 
// 	return 0;
// }

int ZWebApiHandler::onInnerMsg(ZInnerMsg *msg) {
	printf("ZWebApiHandler::onInnerMsg()\n");

	switch (msg->msg_type_) {
		case Z_ZB_GET_DEV_RSP:
			{
				return processMsg((ZInnerGetDevInfoRsp*)msg);
			}
		case Z_ZB_SET_DEV_RSP:
			{
				return -1;
				// return processMsg((ZInnerSetDevInfoRsp*)msg);
			}
		default:
			return -1;
	}
}

void ZWebApiHandler::sendRsp(const char *text_msg, int status)
{
	printf("ZWebApiHandler::sendRsp\n");
	int rv = send(text_msg, strlen(text_msg));
	printf("sent %d bytes\n", rv);
}

int ZWebApiHandler::onRead(char *buf, uint32_t buf_len)
{
	printf("ZWebApiModule::onRead()\n");

	if (buf_len <= 0) { // MIN_MSG_LEN(header length)
		printf("empty message\n");

		sendRsp("empty message\n", 404);

		return -1;
	}

	{
		// 1. decode message
		json_error_t error;
		json_t *jobj = json_loadb(buf, buf_len, 0, &error);
		if ((jobj == NULL) || (!json_is_object(jobj))) {
			printf("bad request\n");
			sendRsp("bad request\n", 400);
			return -1;
		}

		// 2. check `cmd' field
		// (sequence value should allocate from server)
		json_t *cmd = json_object_get(jobj, "cmd");
		if (!cmd || !json_is_string(cmd)) {
			printf("Missing 'cmd' field, or 'cmd' is not a string\n");
			sendRsp("Missing 'cmd' field, or 'cmd' is not a string\n", 400);
			return -1;
		}

		// 3. check session
		const char *cmd_str = json_string_value(cmd);
		if (!strncmp(cmd_str, "get-dev-info", sizeof("get-dev-info") - 1)) {
			return processGetDevInfoReq(jobj);
		} else if (!strncmp(cmd_str, "set-dev-info", sizeof("set-dev-info") - 1)) {
			return processSetDevInfoReq(jobj);
		} else {
			return -1;
		}
	}
}

int ZWebApiHandler::processGetDevInfoReq(json_t *root)
{
	printf("ZWebApiModule::processGetDevInfoReq()\n");

	// var req_obj = {
	// 	"cmd": "get-dev-info",
	// 	"uid": 001,
	// 	"dev-id": 5000,
	// };
	json_t *cmd = json_object_get(root, "cmd");
	if (!cmd || !json_is_string(cmd)) {
		printf("Missing 'cmd' field, or 'cmd' is not a string\n");
		return -1;
	}

	const char *cmd_str = json_string_value(cmd);

	if (strncmp(cmd_str, "get-dev-info", sizeof("get-dev-info") - 1)) {
		printf("Unknown command\n");
		sendRsp("Unknown command\n", 400);
		return -1;
	}

	json_t *juid = json_object_get(root, "uid");
	if (!juid || !json_is_integer(juid)) {
		printf("uid is illegal\n");
		sendRsp("uid is illegal\n", 400);
		return -1;
	}

	json_t *jdev_id = json_object_get(root, "dev-id");
	if (!jdev_id || !json_is_integer(jdev_id)) {
		printf("dev-id is illegal\n");
		sendRsp("dev-id is illegal\n", 400);
		return -1;
	}

	int uid = (int)json_integer_value(juid);
	int dev_id = (int)json_integer_value(jdev_id);

	printf("uid: %d, dev-id: %d\n", uid, dev_id);

	if (uid < 0 || uid > 255 || dev_id < 0 || dev_id > 255) {
		printf("Invalid uid or dev-id\n");
		sendRsp("invalid uid or dev-id\n", 200);
		return 0;
	}

	// sendRsp("uid is good, dev-id is good, everything is good:)\n", 200);
	// transfer from json to ZigBee message
	ZInnerGetDevInfoReq *req = new ZInnerGetDevInfoReq(addr_);
	req->dev_addr_ = dev_id;

	ZDispatcher::instance()->sendMsg(req);
	// // broadcast to all bees
	// ZInnerMsg *innerMsg = new ZInnerMsg(Z_MODULE_SERIAL, 1);
	// innerMsg->msgType = Z_ZB_GET_DEV_REQ;
	// innerMsg->data = req;

	// ZDispatcher::instance()->sendMsg(innerMsg);

	// sendRsp("request has been sent\n", 200);

	return 0;
}

int ZWebApiHandler::processSetDevInfoReq(json_t *root)
{
	printf("ZWebApiModule::processSetDevInfoReq()\n");

	// var req_obj = {
	// 	"cmd": "set-dev-info",
	// 	"uid": 001,
	// 	"dev-id": 5000,
	// };
	json_t *cmd = json_object_get(root, "cmd");
	if (!cmd || !json_is_string(cmd)) {
		printf("Missing 'cmd' field, or 'cmd' is not a string\n");
		return -1;
	}

	const char *cmd_str = json_string_value(cmd);

	if (strncmp(cmd_str, "set-dev-info", sizeof("set-dev-info") - 1)) {
		printf("Unknown command\n");
		sendRsp("Unknown command\n", 400);
		return -1;
	}

	json_t *uid = json_object_get(root, "uid");
	if (!uid || !json_is_integer(uid)) {
		printf("uid is illegal\n");
		sendRsp("uid is illegal\n", 400);
		return -1;
	}

	json_t *dev_id = json_object_get(root, "dev-id");
	if (!dev_id || !json_is_integer(dev_id)) {
		printf("dev-id is illegal\n");
		sendRsp("dev-id is illegal\n", 400);
		return -1;
	}

	printf("uid: %d\n", (int)json_integer_value(uid));
	printf("dev-id: %d\n", (int)json_integer_value(dev_id));

	// sendRsp("uid is good, dev-id is good, everything is good:)\n", 200);
	// transfer from json to ZigBee message
	ZZBSetReq *req = new ZZBSetReq();
	ZItemPair pair;

	pair.id = 1; pair.val = 0x01;
	req->items_.push_back(pair);
	pair.id = 2; pair.val = 0x02;
	req->items_.push_back(pair);
	pair.id = 3; pair.val = 0x03;
	req->items_.push_back(pair);

	// // broadcast to all bees
	// ZInnerMsg *innerMsg = new ZInnerMsg(Z_MODULE_SERIAL, 1);
	// innerMsg->msgType = Z_ZB_GET_DEV_REQ;
	// innerMsg->data = req;

	// ZDispatcher::instance()->sendMsg(innerMsg);

	// sendRsp("request has been sent\n", 200);

	return 0;
}


