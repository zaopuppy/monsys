// system api
#include <iostream>
#include <assert.h>

// libevent
#include <event2/event.h>

// project files
// #include "zsocket.h"
#include "zserver.h"
#include "zclient.h"
#include "zerrno.h"
#include "zlog.h"
#include "zdispatcher.h"
#include "zserial.h"

#include "zzigbee_message.h"

using namespace std;

// static void freeSession(ZTask *session)
// {
//     event_free(session->read_event);
//     // event_free(session->write_event);
//     delete session;
// }

bool start_server(const char* ip, uint16_t port, event_base* base, int type)
{
	ZTask *task = new ZServer(ip, port, base, type);
		// new ZServer(base, ZServer::TYPE_TERM);
	if (task->init() != OK) {
		printf("Failed to initialize server.\n");
		return false;
	}
	return true;
}

bool start_client(event_base *base)
{
	int count = 1;

	ZTask *task;
	for (int i = 0; i < count; ++i) {
	  task = new ZClient(base);
		if (task->init() != OK) {
			printf("Failed to init client: %d\n", i);
			return false;
		}
	}

	return true;
}

bool start_serial(event_base *base)
{
	ZTask *task = new ZSerial(base);
	if (task->init() != OK) {
		printf("Failed to init serial module.\n");
		return false;
	}
	return true;
}

void test_zigbee_message_get()
{
	// get req
	{
		char buf[128];
		ZZBGetReq req1, req2;

		req1.items_.push_back(0x34);
		
		int rv1, rv2;

		rv1 = req1.encode(buf, sizeof(buf));
		assert(rv1 > 0);
		trace_bin(buf, rv1);

		rv2 = req2.decode(buf, rv1);
		assert(rv2 > 0);

		assert(rv1 == rv2);

		// check header
		assert(req1.syn_ == req2.syn_);
		assert(req1.len_ == req2.len_);
		assert(req1.cmd_ == req2.cmd_);

		assert(req1.items_.size() == req2.items_.size());
		size_t s = req1.items_.size();
		for (size_t i = 0; i < s; ++i) {
			assert(req1.items_[i] == req2.items_[i]);
		}
	}
	
	// get rsp
	{
		char buf[128];
		ZZBGetRsp req1, req2;

		struct ZItemPair itemPair;
		itemPair.id = 0x78;
		itemPair.val = 0x89;
		
		req1.items_.push_back(itemPair);

		int rv1, rv2;

		rv1 = req1.encode(buf, sizeof(buf));
		assert(rv1 > 0);
		trace_bin(buf, rv1);

		rv2 = req2.decode(buf, rv1);
		assert(rv2 > 0);

		assert(rv1 == rv2);

		// check header
		assert(req1.syn_ == req2.syn_);
		assert(req1.len_ == req2.len_);
		assert(req1.cmd_ == req2.cmd_);

		assert(req1.items_.size() == req2.items_.size());
		size_t s = req1.items_.size();
		for (size_t i = 0; i < s; ++i) {
			assert(req1.items_[i].id == req2.items_[i].id);
			assert(req1.items_[i].val == req2.items_[i].val);
		}
	}
}

void test_zigbee_message_set()
{
	// set req
	{
		char buf[128];
		ZZBSetReq msg1, msg2;

		struct ZItemPair itemPair;

		itemPair.id = 0x23;
		itemPair.val = 0x9876;
		msg1.items_.push_back(itemPair);
		
		int rv1, rv2;

		rv1 = msg1.encode(buf, sizeof(buf));
		assert(rv1 > 0);
		trace_bin(buf, rv1);

		rv2 = msg2.decode(buf, rv1);
		assert(rv2 > 0);

		assert(rv1 == rv2);

		// check header
		assert(msg1.syn_ == msg2.syn_);
		assert(msg1.len_ == msg2.len_);
		assert(msg1.cmd_ == msg2.cmd_);

		assert(msg1.items_.size() == msg2.items_.size());
		size_t s = msg1.items_.size();
		for (size_t i = 0; i < s; ++i) {
			assert(msg1.items_[i].id == msg2.items_[i].id);
			printf("v1: 0x%X, v2: 0x%X\n", msg1.items_[i].val, msg2.items_[i].val);
			assert(msg1.items_[i].val == msg2.items_[i].val);
		}
	}
	
	// get rsp
	{
		char buf[128];
		ZZBSetRsp msg1, msg2;

		msg1.status_ = 0x88;
		
		int rv1, rv2;

		rv1 = msg1.encode(buf, sizeof(buf));
		assert(rv1 > 0);
		trace_bin(buf, rv1);

		rv2 = msg2.decode(buf, rv1);
		assert(rv2 > 0);

		assert(rv1 == rv2);

		// check header
		assert(msg1.syn_ == msg2.syn_);
		assert(msg1.len_ == msg2.len_);
		assert(msg1.cmd_ == msg2.cmd_);

		assert(msg1.status_ == msg2.status_);
	}
}

int main(int argc, char *argv[])
{
	// test_zigbee_message_get();
	// test_zigbee_message_set();

	struct event_base* base = event_base_new();
	assert(base);
	
	// if (!start_server("0.0.0.0", 1984, base, ZServer::TYPE_ZIGBEE)) {
	// 	printf("failed to start server: (0.0.0.0, 1983).\n");
	// 	return -1;
	// }

	if (!start_server("0.0.0.0", 1983, base, ZServer::TYPE_TERM)) {
		printf("failed to start server: (0.0.0.0, 1984).\n");
		return -1;
	}
	
 	// if (!start_client(base)) {
 	// 	printf("failed to start client.\n");
 	// 	return -1;
 	// }
	
	if (!start_serial(base)) {
		printf("failed to start serial.\n");
		return FAIL;
	}
	
	// event_base_dispatch(base);
	while (1) {
		// printf("loop\n");
		event_base_loop(base, EVLOOP_NONBLOCK);
		ZDispatcher::instance()->routine();
		usleep(100 * 1000);	// 100 milliseconds
	}

	return 0;
}


