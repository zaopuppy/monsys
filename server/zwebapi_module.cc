#include "zwebapi_module.h"

#include <unistd.h>
#include <assert.h>
#include <errno.h>

#include "zerrno.h"
#include "zlog.h"
#include "zdispatcher.h"
#include "zutil.h"

#include "zmodule.h"

int ZWebApiModule::init() {
	printf("Oops, client's coming\n");

	return super_::init();
}

void ZWebApiModule::close() {
	state_ = STATE_FINISHED;
}

void ZWebApiModule::event(evutil_socket_t fd, short events) {
	fd_ = fd;
	switch (state_) {
		case STATE_CONNECTED:
			onConnected(fd, events);
			break;
		default:
			close();
			break;
	}
}

void ZWebApiModule::doTimeout() {
}

bool ZWebApiModule::isComplete() {
	return (state_ == STATE_FINISHED);
}

int ZWebApiModule::onInnerMsg(ZInnerMsg *msg) {
	printf("ZWebApiModule::onInnerMsg()\n");
	return 0;
}

void ZWebApiModule::onConnected(evutil_socket_t fd, short events) {
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

void ZWebApiModule::sendRsp(const char *text_msg, int status)
{
	send(fd_, text_msg, strlen(text_msg), 0);
}

int ZWebApiModule::onRead(evutil_socket_t fd, char *buf, uint32_t buf_len)
{
	printf("ZWebApiModule::onRead()\n");
	trace_bin(buf, buf_len);

	if (buf_len <= 0) { // MIN_MSG_LEN(header length)
		printf("empty message\n");

		sendRsp("empty message\n", 404);

		return -1;
	}

	json_error_t error;
	json_t *jobj = json_loadb(buf, buf_len, 0, &error);
	if ((jobj == NULL) || (!json_is_object(jobj))) {
		printf("bad request\n");
		sendRsp("bad request\n", 400);
		return -1;
	}

	int result = processJson(jobj);

	// release reference
	json_decref(jobj);

	return result;
}

int ZWebApiModule::processJson(json_t *root)
{
	printf("ZWebApiModule::processJson\n");
	// var req_obj = {        
	// 	"cmd": "get-dev-info",
	// 	"uid": "uid001",      
	// 	"dev-id": "5000",     
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

	sendRsp("uid is good, dev-id is good, everything is good:)\n", 200);

	return 0;
}

