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

	// return super_::init();
	return 0;
}

void ZWebApiHandler::close() {
	::close(fd_);
	fd_ = -1;
	event_del(read_event_);
}

int ZWebApiHandler::event(evutil_socket_t fd, short events)
{
	// FIXME: remove following line
	fd_ = fd;
	onConnected(fd, events);
	return OK;
}

int ZWebApiHandler::onInnerMsg(ZInnerMsg *msg) {
	printf("ZWebApiModule::onInnerMsg()\n");

	switch (msg->msgType) {
		case Z_ZB_GET_DEV_RSP:
			{
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

					json_t *code = json_integer(-4);
					rv = json_object_set_new(jobj, "code", code);

					json_t *info = json_array();
					// 1
					{
						json_t *obj1 = json_object();

						json_t *id1 = json_integer(1);
						json_object_set_new(obj1, "id", id1);

						json_t *status1 = json_integer(0);
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

				break;
			}
		case Z_ZB_SET_DEV_RSP:
			{
				printf("Z_ZB_SET_DEV_RSP\n");
				// const char *str = "{
				// 	\"cmd\": \"set-dev-info-rsp\",
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

					json_t *cmd = json_string("set-dev-info-rsp");
					rv = json_object_set_new(jobj, "cmd", cmd);

					json_t *code = json_integer(-4);
					rv = json_object_set_new(jobj, "code", code);

					json_t *info = json_array();
					// 1
					{
						json_t *obj1 = json_object();

						json_t *id1 = json_integer(1);
						json_object_set_new(obj1, "id", id1);

						json_t *status1 = json_integer(0);
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

				break;
			}
		default:
			break;
	}

	return 0;
}

void ZWebApiHandler::onConnected(evutil_socket_t fd, short events) {
	printf("ZWebApiModule::doConnected()\n");
	unsigned int buf_idx = 0;
	ssize_t len;
	do {
		len = recv(fd, buf_ + buf_idx, sizeof(buf_), 0);
		if (len > 0)
			buf_idx += len;
	} while (buf_idx <= sizeof(buf_) && len > 0);

	printf("len: %ld\n", len);
	if (len < 0 && errno != EAGAIN) {
		perror("recv");
		// freeSession(session);
		close();
	} else if (len == 0) {
		printf("peer closed.\n");
		close();
	}else {
		// XXX, may write beyond the bound.
		if (buf_idx > 0) {
			buf_[buf_idx] = 0x00;
			onRead(fd, buf_, buf_idx);
		}
	}
}

void ZWebApiHandler::sendRsp(const char *text_msg, int status)
{
	send(fd_, text_msg, strlen(text_msg), 0);
}

int ZWebApiHandler::onRead(evutil_socket_t fd, char *buf, uint32_t buf_len)
{
	printf("ZWebApiModule::onRead()\n");
	trace_bin(buf, buf_len);

	return event(buf, buf_len);
}

int ZWebApiHandler::event(char *buf, uint32_t buf_len)
{
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
			return processGetDevInfo(jobj);
		} else if (!strncmp(cmd_str, "set-dev-info", sizeof("set-dev-info") - 1)) {
			return processSetDevInfo(jobj);
		} else {
			return -1;
		}
	}
}

int ZWebApiHandler::processGetDevInfo(json_t *root)
{
	printf("ZWebApiModule::processGetDevInfo()\n");

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
	ZZBGetReq *req = new ZZBGetReq();
	req->items_.push_back(1);
	req->items_.push_back(2);
	req->items_.push_back(3);

	// broadcast to all bees
	ZInnerMsg *innerMsg = new ZInnerMsg(Z_MODULE_SERIAL, 1);
	innerMsg->msgType = Z_ZB_GET_DEV_REQ;
	innerMsg->data = req;

	ZDispatcher::instance()->sendMsg(innerMsg);

	// sendRsp("request has been sent\n", 200);

	return 0;
}

int ZWebApiHandler::processSetDevInfo(json_t *root)
{
	printf("ZWebApiModule::processSetDevInfo()\n");

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

	// broadcast to all bees
	ZInnerMsg *innerMsg = new ZInnerMsg(Z_MODULE_SERIAL, 1);
	innerMsg->msgType = Z_ZB_GET_DEV_REQ;
	innerMsg->data = req;

	ZDispatcher::instance()->sendMsg(innerMsg);

	// sendRsp("request has been sent\n", 200);

	return 0;
}


