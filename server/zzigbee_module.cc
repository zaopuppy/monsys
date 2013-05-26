#include "zzigbee_module.h"

#include <unistd.h>
#include <assert.h>
#include <errno.h>

#include "zerrno.h"
#include "zlog.h"

static void SOCKET_CALLBACK(evutil_socket_t fd, short events, void *arg)
{
	// printf("SOCKET_CALLBACK\n");
	assert(arg);
	ZZigBeeModule *task = (ZZigBeeModule*)arg;
	task->onTimer();
}

static const struct timeval RETRY_INTERVAL = { 2, 0 };

int ZZigBeeModule::onTimer()
{
	printf("ZZigBeeModule::onTimer()\n");
	return 0;
}

int ZZigBeeModule::init() {
	printf("Caught a tiny zigbee\n");

	// --- DEBUG ---
	struct event* ev = evtimer_new(base_, SOCKET_CALLBACK, this);
	event_add(ev, &RETRY_INTERVAL);
	// --- DEBUG ---

	return OK;
}

void ZZigBeeModule::close() {
	state_ = STATE_FINISHED;
}

int ZZigBeeModule::onInnerMsg(ZInnerMsg *msg) {
	printf("ZZigBeeModule::onInnerMsg()\n");

	// ZData *data = (ZData*)msg->data;

	// delete msg;
	// msg = NULL;

	// if (data->data == 1) {
	// 	printf("Got one\n");
	// } else if (data->data == 2) {
	// 	printf("Got two\n");
	// } else {
	// 	printf("Unknown data\n");
	// }

	// delete data;

	return 0;
}

int ZZigBeeModule::sendMsg(ZInnerMsg *msg)
{
	printf("ZZigBeeModule::sendMsg\n");
	return 0;
}

void ZZigBeeModule::event(evutil_socket_t fd, short events) {
	switch (state_) {
		case STATE_CONNECTED:
			onConnected(fd, events);
			break;
		default:
			close();
			break;
	}
}

void ZZigBeeModule::onConnected(evutil_socket_t fd, short events)
{
	printf("ZZigBeeModule::doConnected()\n");
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

int ZZigBeeModule::onRead(evutil_socket_t fd, char *buf, uint32_t buf_len)
{
	printf("ZZigBeeModule::onRead()\n");
	trace_bin(buf, buf_len);

	// START(1)
	// ADDR(1)
	// LEN(1)
	// DATA(n)
	// CRC(2)
	if (buf_len < 5) {
		printf("message is too short\n");
		return -1;
	}

	uint8_t b, len;
	int idx = 0;

	len = (uint8_t)buf[2];
	if (buf_len < (uint32_t)(len + 3)) {
		printf("message is not long enough, expect %u, got %u\n",
					 (len + 3), buf_len);
		return -1;
	}

	b = (uint8_t)buf[idx];
	if (b == 0xAA) {
	} else if (b == 0x55) {
	} else {
		printf("Unknown ZJC message: 0x%02X\n", b);
		return -1;
	}

	
	// ssize_t rv = send(fd, buf, buf_len, 0);
	// // if (rv < 0 && (errno != EAGAIN)) {
	// if (rv < 0) {
	// 	printf("Failed to send data back...:(\n");
	// 	printf("errno: %d\n", errno);
	// } else {
	// 	printf("Send.\n");
	// }
	return 0;
}

void ZZigBeeModule::processReq(char* buf, uint32_t buf_len)
{
	printf("ZZigBeeModule::processMsg()\n");
}

void ZZigBeeModule::processRsp(char* buf, uint32_t buf_len)
{
	printf("ZZigBeeModule::processRsp()\n");
}


