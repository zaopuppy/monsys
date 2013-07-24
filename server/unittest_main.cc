#include "unittest_main.h"


#include <jansson.h>

#include "framework/zframework.h"

#include "zlog.h"
#include "zzigbee_message.h"
#include "zutil.h"
#include "zformatter.h"
#include "unittest_main.h"


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
	// // req
	// {
	// 	char buf[128];
	// 	ZZBRegReq req1, req2;
	// 	int rv1, rv2;

	// 	// bad
	// 	req1.mac_.assign("\xab\xcd\x12");
	// 	rv1 = req1.encode(buf, sizeof(buf));
	// 	assert(rv1 < 0);

	// 	req1.mac_.assign("\xab\xcd\x12\x23\x34\x78\x90\x33", 8);
	// 	req1.addr_ = 0x4455;

	// 	rv1 = req1.encode(buf, sizeof(buf));
	// 	assert(rv1 > 0);
	// 	trace_bin(buf, rv1);

	// 	rv2 = req2.decode(buf, rv1);
	// 	assert(rv2 > 0);

	// 	assert(rv1 == rv2);

	// 	// check header
	// 	assert(req1.syn_ == req2.syn_);
	// 	assert(req1.len_ == req2.len_);
	// 	assert(req1.cmd_ == req2.cmd_);

	// 	assert(req1.mac_ == req2.mac_);
	// }
	
	// // rsp
	// {
	// 	char buf[128];
	// 	ZZBRegRsp req1, req2;

	// 	req1.addr_ = 0x0F;
	// 	req1.status_ = 0x0B;

	// 	int rv1, rv2;

	// 	rv1 = req1.encode(buf, sizeof(buf));
	// 	assert(rv1 > 0);
	// 	trace_bin(buf, rv1);

	// 	rv2 = req2.decode(buf, rv1);
	// 	assert(rv2 > 0);

	// 	assert(rv1 == rv2);

	// 	// check header
	// 	assert(req1.syn_ == req2.syn_);
	// 	assert(req1.len_ == req2.len_);
	// 	assert(req1.cmd_ == req2.cmd_);

	// 	assert(req1.addr_ == req2.addr_);
	// 	assert(req1.status_ == req2.status_);
	// }
}

void test_util()
{
	const char *str = "ab12345,124";
	int rv;

	rv = ZStringUtil::atoi(str, -1);
	assert(-1 == rv);

	rv = ZStringUtil::atoi(str, -1, 2, 6);
	assert(12345 == rv);

	rv = ZStringUtil::atoi(str, -1, 8, 10);
	assert(124 == rv);

	{
		const char *id_list = "1,2,3,4,5";

		// tokenize string
		size_t begin_idx = 0;
		size_t end_idx = 0;
		size_t id_list_len = strlen(id_list);
		int id;

		while (true) {
			while (id_list[end_idx] != ',' && end_idx < id_list_len) {
				end_idx++;
			}

			// don't include ',' or eol
			--end_idx;

			id = ZStringUtil::atoi(id_list, -1, begin_idx, end_idx);
			printf("id: [%d]\n", id);
			if (id == -1) {
				printf("Bad number\n");
				break;
			}

			end_idx += 2;	// skip ','
			begin_idx = end_idx;
			if (begin_idx >= id_list_len) {
				break;
			}
		}
	}
}

void test_other()
{
	std::vector<int> v;
	bool rv = str2list("1,2,3,4,5", v);
	assert(rv);
}

class ZStream
{
 public:
 	virtual int read(char *buf, uint32_t buf_len) = 0;
 	virtual int write(char *buf, uint32_t buf_len) = 0;
 	virtual void rewind() = 0;
 	virtual bool seek(uint32_t offset, int startpos) = 0;

 	virtual bool eof() = 0;

 public:
 	enum SEEK_DIR {
 		SEEK_BEGINNING = -1,
 		SEEK_CURRENT,
 		SEEK_ENDING,
 	};
};

// Input String Stream
class ZSStreamIn : public ZStream {
 public:
 	ZSStreamIn(const char *str, uint32_t str_len)
 	: input_(str), input_len_(str_len), pos_(0)
 	{}

 public:
 	virtual int read(char *buf, uint32_t buf_len) {
 		if (eof()) {
 			return 0;
 		}

 		uint32_t remain = input_len_ - pos_;
 		uint32_t len = remain > buf_len ? buf_len : remain;

 		memcpy(buf, input_ + pos_, len);

 		pos_ += len;

 		return len;
 	}

 	virtual int write(char *buf, uint32_t buf_len) {
 		// read only
 		return 0;
 	}

 	virtual void rewind() { pos_ = 0; }

 	virtual bool seek(uint32_t offset, int startpos) {
 		if (startpos == ZStream::SEEK_BEGINNING) {
 			if (offset > input_len_) {
 				return false;
 			}

 			pos_ = offset;
 		} else if (startpos == ZStream::SEEK_CURRENT) {
 			if ((pos_ + offset) >= input_len_) {
 				return false;
 			}

 			pos_ += offset;
 		} else if (startpos == ZStream::SEEK_ENDING) {
 			if (offset > input_len_) {
 				return false;
 			}

 			pos_ = input_len_ - offset;
 		} else {
 			return false;
 		}
 	}

 	char getc() {
 		if (pos_ < input_len_) {
 			return input_[pos_++];
 		}

 		return -1;
 	}

 	char peekc() {
 		if (pos_ < input_len_) {
 			return input_[pos_];
 		}

 		return -1;
 	}


 	bool eof() { return pos_ >= input_len_; }

 	// for testing only
 	static void test() {
 		const char *script = "Hello, I'm Joey from China.";
 		uint32_t script_len = strlen(script);
 		ZSStreamIn in(script, script_len);

 		char buf[512];
 		int rv = in.read(buf, sizeof(buf));
 		buf[rv] = 0x00;
 		printf("rv: [%d], read: [%s]\n", rv, buf);

 		assert(in.eof());
 		while (!in.eof()) {
 			printf("%c", in.getc());
 		}
 	}
 	// for testing only

 public:
 	const char *input_;
 	const uint32_t input_len_;
 	uint32_t pos_;
};

void test_formatter()
{
	ZFormatter formatter;
	// const char *script = "hello, world, ${div($div(1,2), 2)}";
	const char *script = "hello, world, $div($div($1, 3), 2)";

	printf("input: [%s]\n", script);

	assert(formatter.compile(script));

	formatter.exe(4);
}

void test_zigbee_update()
{
	// request
	{
		ZZBUpdateIdInfoReq req;
		int rv;

		char buf[1024];

		// empty
		rv = req.encode(buf, sizeof(buf));
		assert(rv > 0);

		trace_bin(buf, rv);

		// 1 item
		// ItemIdInfo info;
		zb_item_id_info_t info;
		info.id_ = 0x34;
		info.name_ = "hello";
		info.desc_ = "good";
		info.type_ = 0x08;
		info.formatter_ = "printf";
		info.last_update_time_ = 123;

		req.id_list_.push_back(info);
		rv = req.encode(buf, sizeof(buf));
		assert(rv > 0);

		trace_bin(buf, rv);

		ZZBUpdateIdInfoReq req1;

		rv = req1.decode(buf, rv);
		assert(rv > 0);

		assert(req.id_list_.size() == req1.id_list_.size());
		for (size_t i = 0; i < req.id_list_.size(); ++i) {
			assert(req.id_list_[i].id_        == req1.id_list_[i].id_);
			assert(req.id_list_[i].name_      == req1.id_list_[i].name_);
			assert(req.id_list_[i].desc_      == req1.id_list_[i].desc_);
			assert(req.id_list_[i].type_      == req1.id_list_[i].type_);
			assert(req.id_list_[i].formatter_ == req1.id_list_[i].formatter_);
		}
	}
	// response
	{
		ZZBUpdateIdInfoRsp rsp;
		int rv;

		char buf[512];

		rsp.status_ = 0xBB;

		rv = rsp.encode(buf, sizeof(buf));
		assert(rv > 0);

		trace_bin(buf, rv);

		ZZBUpdateIdInfoRsp rsp1;

		rv = rsp1.decode(buf, rv);
		assert(rv > 0);

		assert(rsp.status_ == rsp1.status_);
	}
}


// int main(int argc, char const *argv[])
// {
// 	printf("Start to do unit test.\n");
// 	return 0;
// }

