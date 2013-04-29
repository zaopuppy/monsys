#include "zapi_module.h"

#include <unistd.h>
#include <assert.h>
#include <errno.h>

#include "zerrno.h"
#include "zmessage_codec.h"
#include "zlog.h"
#include "zdispatcher.h"

#include "zmodule.h"

int ZApiModule::init() {
	printf("Oops, client's coming\n");

	return super_::init();
}

void ZApiModule::close() {
	state_ = STATE_FINISHED;
}

void ZApiModule::event(evutil_socket_t fd, short events) {
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

void ZApiModule::doTimeout() {
}

bool ZApiModule::isComplete() {
	return (state_ == STATE_FINISHED);
}

int ZApiModule::onInnerMsg(ZInnerMsg *msg) {
	printf("ZApiModule::onInnerMsg()\n");
	return 0;
}

void ZApiModule::onConnected(evutil_socket_t fd, short events) {
	printf("ZApiModule::doConnected()\n");
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

int ZApiModule::onRead(evutil_socket_t fd, char *buf, uint32_t buf_len) {
	printf("ZApiModule::onRead()\n");
	trace_bin(buf, buf_len);

	// ---- FOR DEBUGGING ONLY ----
	if (true) {
		processCmd(fd, buf, buf_len);
		return 0;
	}
	// ---- FOR DEBUGGING ONLY ----

	if (buf_len < 12) { // MIN_MSG_LEN(header length)
		printf("message is not long enough: %u\n", buf_len);
		
		const char *data = "too short\n";
		send(fd_, data, strlen(data), 0);
		
		return -1;
	}
	
	struct z_header hdr;
	int rv = z_decode_header(&hdr, buf, buf_len);
	if (rv < 0) {
		printf("Unknown message, discard.\n");
		trace_bin(buf, buf_len);
		
		const char *data = "unknown message type\n";
		send(fd_, data, strlen(data), 0);
		
		return -1;
	}

	int result = -1;
	switch (hdr.cmd) {
	case (uint16_t)ID_GET_DEV_REQ:
		{
			struct z_query_dev_req req;
			int enc_len = z_decode_query_dev_req(&req, buf, buf_len);
			if (enc_len <= 0) {
				printf("failed to decode request.\n");
				break;
			}

			processMsg(req);
			
			break;
		}
	default:
		printf("unknown message type: 0x%08X\n", hdr.cmd);
		break;
	}

	return result;
}

void ZApiModule::processMsg(struct z_query_dev_req &msg)
{
	printf("ZApiModule::processMsg()\n");

	struct z_dev_info info;
	info.id = 0x14;
	info.stat = 0x15;
	info.desc = (char*)"randam device";

	struct z_query_dev_rsp rsp;
	rsp.code = 444;
	rsp.reason = (char*)"good reason...";
	rsp.info_list.count = 1;
	rsp.info_list.infos = &info;

	int rv = z_encode_query_dev_rsp(&rsp, out_buf_, sizeof(out_buf_));
	if (rv <= 0) {
		printf("Bubu, failed to encode response\n");
		return;
	}

	printf("Read to send response\n");
	rv = send(fd_, out_buf_, rv, 0);
	printf("Sending complete[%d]\n", rv);
}

void ZApiModule::printMsg(struct z_header &msg)
{
	printf("- z_header::sync[0]: 0x%02X\n", msg.syn[0]);
	printf("- z_header::sync[1]: 0x%02X\n", msg.syn[1]);
	printf("- z_header::len: %d\n", msg.len);
	printf("- z_header::cmd: %u\n", msg.cmd);
	printf("- z_header::seq: %u\n", msg.seq);
}

void ZApiModule::printMsg(struct z_dev_info &msg)
{
	printf("- z_dev_info::id: %u\n", msg.id);
	printf("- z_dev_info::id: %u\n", msg.stat);
	printf("- z_dev_info::desc: %s\n", msg.desc);
}

void ZApiModule::printMsg(struct z_dev_info_list &msg)
{
	int i;
	printf("- z_dev_info_list::count: %u\n", msg.count);
	
	for (i = 0; i < msg.count; ++i) {
		printMsg(msg.infos[i]);
	}
}

void ZApiModule::printMsg(struct z_query_dev_req &msg)
{
	printf("-------------------------------------\n");
	printf("print message: z_query_dev_req\n");
	printMsg(msg.hdr);

	printf("- z_query_dev_req::uid: %s\n", msg.uid);
	printf("- z_query_dev_req::dev-id: %u\n", msg.dev_id);
}

void ZApiModule::printMsg(struct z_query_dev_rsp &msg)
{
	printf("-------------------------------------\n");
	printf("print message: z_query_dev_rsp\n");
	printMsg(msg.hdr);

	printf("- z_query_dev_req::code: %u\n", msg.code);
	printf("- z_query_dev_req::reason: %s\n", msg.reason);
	
	printMsg(msg.info_list);
}

static bool isBlank(const char *str, uint32_t str_len)
{
	while (str_len > 0) {
		if (*str != ' ' && *str != '\t' && *str != '\r' && *str != '\n') {
			return false;
		}
		++str;
	}
	return true;
}

void ZApiModule::processCmd(evutil_socket_t fd, char *buf, uint32_t buf_len)
{
	printf("ZApiModule::processCmd\n");

	const char *msg;

	if (isBlank(buf, buf_len)) {
		msg = "no command\n";
		send(fd, msg, strlen(msg), 0);
		return;
	}

	int target_module = Z_MODULE_SERIAL;

	const char* p = buf;
	if (*p == '1') {
		msg = "You gave me a '1', right?\n";
		send(fd, msg, strlen(msg), 0);

		ZInnerMsg *innerMsg = new ZInnerMsg(target_module, 1);
		ZData *data = new ZData();
		data->data = 1;
		innerMsg->data = data;
		if (0 != ZDispatcher::instance()->sendMsg(innerMsg)) {
			printf("Failed to send inner message to other module\n");
		}
	} else if (*p == '2') {
		msg = "You gave me a '2', right?\n";
		send(fd, msg, strlen(msg), 0);

		ZInnerMsg *innerMsg = new ZInnerMsg(target_module, 1);
		ZData *data = new ZData();
		data->data = 2;
		innerMsg->data = data;
		if (0 != ZDispatcher::instance()->sendMsg(innerMsg)) {
			printf("Failed to send inner message to other module\n");
		}
	} else {
		msg = "Unknown command.\n";
		// send(fd, msg, strlen(msg), 0);
	}
}


