// system api
#include <iostream>
#include <assert.h>

// libevent
#include <event2/event.h>

#include <jansson.h>

// project files
// #include "zsocket.h"
#include "zwebapi_server.h"
#include "zapi_server.h"
#include "zclient.h"
#include "zerrno.h"
#include "zlog.h"
#include "zdispatcher.h"
#include "zserial.h"

#include "zzigbee_message.h"

using namespace std;

void test_json()
{
	{
		const char *str = "{ \
			\"cmd\": \"get-dev-info-rsp\", \
			\"code\": -4, \
			\"info\": [ \
				{ \"id\": 1, \"status\": 2, \"desc\": \"OK0\" }, \
				{ \"id\": 2, \"status\": 3, \"desc\": \"OK1\" }, \
				{ \"id\": 3, \"status\": 4, \"desc\": \"OK3\" } \
			] \
		}";

		json_error_t error;
		json_t *jobj = json_loads(str, 0, &error);
		if (jobj == NULL) {
			printf("Failed to load json object\n");
			return;
		}

		printf("cmd: [%s]\n", json_string_value(json_object_get(jobj, "cmd")));
		printf("code: [%lld]\n", json_integer_value(json_object_get(jobj, "code")));

		json_t *infos = json_object_get(jobj, "info");
		for (unsigned int i = 0; i < json_array_size(infos); ++i) {
			printf("[%d]: id = [%lld], status = [%lld], desc = [%s]\n", i,
					json_integer_value(json_object_get(json_array_get(infos, i), "id")),
					json_integer_value(json_object_get(json_array_get(infos, i), "status")),
					json_string_value(json_object_get(json_array_get(infos, i), "desc")));
		}

		char *str_dump = json_dumps(jobj, 0);
		printf("encoded value: [%s]\n", str_dump);

		free(str_dump);
		json_decref(jobj);
	}

	{
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
		
			json_t *status1 = json_integer(2);
			json_object_set_new(obj1, "status", status1);
		
			json_t *desc1 = json_string("OK0");
			json_object_set_new(obj1, "desc", desc1);
		
			json_array_append_new(info, obj1);
		}
		
		json_object_set_new(jobj, "info", info);

		// jobj = json_array_get(jobj, 0);
		// print
		printf("cmd: [%s]\n", json_string_value(json_object_get(jobj, "cmd")));
		printf("code: [%lld]\n", json_integer_value(json_object_get(jobj, "code")));

		json_t *infos = json_object_get(jobj, "info");
		for (unsigned int i = 0; i < json_array_size(infos); ++i) {
			printf("[%d]: id = [%lld], status = [%lld], desc = [%s]\n", i,
					json_integer_value(json_object_get(json_array_get(infos, i), "id")),
					json_integer_value(json_object_get(json_array_get(infos, i), "status")),
					json_string_value(json_object_get(json_array_get(infos, i), "desc")));
		}

		char *str_dump = json_dumps(jobj, 0);
		printf("encoded value: [%s]\n", str_dump);

		free(str_dump);
		json_decref(jobj);
	}
}

bool start_webapi_server(const char* ip, uint16_t port, event_base* base)
{
	ZModule *module = new ZWebApiServer(ip, port, base);
	if (module->init() != OK) {
		printf("Failed to initialize server.\n");
		return false;
	}

	return true;
}

bool start_api_server(const char* ip, uint16_t port, event_base* base)
{
	ZModule *module = new ZApiServer(ip, port, base);
	if (module->init() != OK) {
		printf("Failed to initialize server.\n");
		return false;
	}

	return true;
}

bool start_client(event_base *base)
{
	int count = 1;

	ZModule *m;
	for (int i = 0; i < count; ++i) {
	  m = new ZClient(base);
		if (m->init() != OK) {
			printf("Failed to init client: %d\n", i);
			return false;
		}
	}

	return true;
}

bool start_serial(event_base *base)
{
	ZModule *h = new ZSerial(base);
	if (h->init() != OK) {
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

void test_zigbee_message_reg()
{
	// req
	{
		char buf[128];
		ZZBRegReq req1, req2;
		int rv1, rv2;

		// bad
		req1.mac_.assign("\xab\xcd\x12");
		rv1 = req1.encode(buf, sizeof(buf));
		assert(rv1 < 0);

		req1.mac_.assign("\xab\xcd\x12\x23\x34\x78", 6);

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

		assert(req1.mac_ == req2.mac_);
	}
	
	// rsp
	{
		char buf[128];
		ZZBRegRsp req1, req2;

		req1.addr_ = 0x0F;
		req1.status_ = 0x0B;

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

		assert(req1.addr_ == req2.addr_);
		assert(req1.status_ == req2.status_);
	}
}

int main(int argc, char *argv[])
{
	// test_zigbee_message_get();
	// test_zigbee_message_set();
	// test_zigbee_message_reg();
	// test_json();
	// return 0;

	struct event_base* base = event_base_new();
	assert(base);
	
	// if (!start_server("0.0.0.0", 1984, base, ZServer::TYPE_ZIGBEE)) {
	// 	printf("failed to start server: (0.0.0.0, 1983).\n");
	// 	return -1;
	// }

	// if (!start_server("0.0.0.0", 1983, base, ZServer::TYPE_WEBCLIENT)) {
	// 	printf("failed to start server: (0.0.0.0, 1984).\n");
	// 	return -1;
	// }

	// if (!start_server("0.0.0.0", 1983, base, ZServer::TYPE_APICLIENT)) {
	// 	printf("failed to start server: (0.0.0.0, 1984).\n");
	// 	return -1;
	// }

	if (!start_webapi_server("0.0.0.0", 1983, base)) {
		printf("failed to start server: (0.0.0.0, 1983).\n");
		return -1;
	}

	if (!start_api_server("0.0.0.0", 1984, base)) {
		printf("failed to start server: (0.0.0.0, 1983).\n");
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


