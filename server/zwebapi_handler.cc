#include "zwebapi_handler.h"

#include <unistd.h>
#include <assert.h>
#include <errno.h>

#include "libframework/zframework.h"

#include "libbase/zlog.h"
#include "zutil.h"
#include "module.h"
#include "libzigbee/zzigbee_message.h"
// #include "zjson_codec.h"

#include "zroute.h"

//////////////////////////////////////////////////////
int ZWebApiHandler::init() {
	printf("[%p] Oops, client's coming\n", this);

	// init address here
	addr_.module_type_ = getModuleType();
	addr_.handler_id_ = getId();

	return 0;
}

void ZWebApiHandler::close() {
	::close(fd_);
	fd_ = -1;
	event_del(read_event_);
	// XXX: don't forget to delete ourself
	// add to a free list, do it in routine
}

int ZWebApiHandler::processMsg(ZInnerGetDevListRsp *msg)
{
	printf("ZWebApiHandler::processMsg(GetDevInfoRsp)\n");

	int rv;

	// root
	json_t *jobj = json_object();
	assert(jobj);

	// cmd
	json_t *jcmd = json_string("get-dev-list-rsp");
	rv = json_object_set_new(jobj, "cmd", jcmd);
	if (rv != 0) {
		printf("Failed to set 'cmd'\n");
		return FAIL;
	}

	// status
	json_t *jstatus;
	jstatus = json_integer(0);
	rv = json_object_set_new(jobj, "status", jstatus);
	if (rv != 0) {
		printf("Failed to set 'status'\n");
		return FAIL;
	}

	// dev array
	json_t *jdevs = json_array();

	for (uint32_t i = 0; i < msg->info_list_.size(); ++i)	{
		json_t *jelem = json_object();

		// addr
		json_t *jaddr = json_integer(msg->info_list_[i]->addr_);
		json_object_set_new(jelem, "addr", jaddr);

		// mac
		// XXX

		// state
		json_t *jstate = json_integer(msg->info_list_[i]->state_);
		json_object_set_new(jelem, "state", jstate);

		// name
		json_t *jname = json_string(msg->info_list_[i]->name_.c_str());
		json_object_set_new(jelem, "name", jname);

		// type
		json_t *jtype = json_integer(msg->info_list_[i]->type_);
		json_object_set_new(jelem, "type", jtype);

		// // id count
		// json_t *jidCount = json_integer(msg->info_list_[i]->id_count_);
		// json_object_set_new(jelem, "id-count", jidCount);

		// XXX
		json_array_append_new(jdevs, jelem);
	}

	json_object_set_new(jobj, "devs", jdevs);

	char *str_dump = json_dumps(jobj, 0);

	sendRsp(str_dump, 200);

	free(str_dump);
	json_decref(jobj);


	msg->info_list_.clear();

	return 0;
}

int ZWebApiHandler::processMsg(ZInnerGetDevInfoRsp *msg)
{
	printf("ZWebApiHandler::processMsg(GetDevInfoRsp)\n");

	int rv;

	json_t *jobj = json_object();
	assert(jobj);

	json_t *cmd = json_string("get-dev-info-rsp");
	rv = json_object_set_new(jobj, "cmd", cmd);
	if (rv != 0) {
		printf("Failed to set 'cmd'\n");
		return FAIL;
	}

	json_t *code;
	if (msg->dev_infos_.size() > 0) {
		code = json_integer(0);
	} else {
		code = json_integer(-1);
	}

	rv = json_object_set_new(jobj, "code", code);

	json_t *info = json_array();

	for (uint32_t i = 0; i < msg->dev_infos_.size(); ++i) {
		json_t *jobj1 = json_object();

		json_t *id1 = json_integer(msg->dev_infos_[i].id);
		json_object_set_new(jobj1, "id", id1);

		json_t *jval = json_integer(msg->dev_infos_[i].val);
		json_object_set_new(jobj1, "val", jval);

		json_t *desc1 = json_string("OK0");
		json_object_set_new(jobj1, "desc", desc1);

		json_array_append_new(info, jobj1);
	}

	json_object_set_new(jobj, "info", info);

	char *str_dump = json_dumps(jobj, 0);

	sendRsp(str_dump, 200);

	free(str_dump);
	json_decref(jobj);

	return 0;
}

int ZWebApiHandler::processMsg(ZInnerSetDevInfoRsp *msg)
{
	printf("ZWebApiHandler::processMsg(ZInnerSetDevInfoRsp)\n");
	printf("Z_ZB_SET_DEV_RSP\n");

	int rv;

	json_t *jroot = json_object();
	assert(jroot);

	json_t *jcmd = json_string("set-dev-info-rsp");
	rv = json_object_set_new(jroot, "cmd", jcmd);
	if (rv != 0) {
		printf("Failed to set 'cmd'\n");
		return FAIL;
	}

	json_t *jstatus = json_integer(msg->status_);
	rv = json_object_set_new(jroot, "status", jstatus);
	if (rv != 0) {
		printf("Failed to set 'status'\n");
		return FAIL;
	}

	char *str_dump = json_dumps(jroot, 0);

	sendRsp(str_dump, 200);

	free(str_dump);
	json_decref(jroot);

	return 0;
}

int ZWebApiHandler::onInnerMsg(ZInnerMsg *msg)
{
	printf("ZWebApiHandler::onInnerMsg()\n");

	switch (msg->msg_type_) {
		case Z_ZB_GET_DEV_LIST_RSP:
			{
				return processMsg((ZInnerGetDevListRsp*)msg);
			}
		case Z_ZB_GET_DEV_RSP:
			{
				return processMsg((ZInnerGetDevInfoRsp*)msg);
			}
		case Z_ZB_SET_DEV_RSP:
			{
				return processMsg((ZInnerSetDevInfoRsp*)msg);
			}
		default:
			return -1;
	}
}

void ZWebApiHandler::routine(long delta)
{}


void ZWebApiHandler::sendRsp(const char *text_msg, int status)
{
	printf("ZWebApiHandler::sendRsp\n");
	int rv = send(text_msg, strlen(text_msg));
	printf("sent %d bytes\n", rv);
}

ZInnerMsg* ZWebApiHandler::convertGetDevListReq(json_t *jobj)
{
	printf("ZWebApiHandler::convertGetDevListReq()\n");
	// cmd = get-dev-list;
	json_t *jcmd = json_object_get(jobj, "cmd");
	if (!jcmd || !json_is_string(jcmd)) {
		printf("bad cmd field\n");
		return NULL;
	}

	return new ZInnerGetDevListReq(addr_);
}

ZInnerMsg* ZWebApiHandler::convertGetDevInfoReq(json_t *jobj)
{
	printf("ZWebApiModule::convertGetDevInfoReq()\n");

	////////////////////////////////////////////////////////
	// command
	json_t *jcmd = json_object_get(jobj, "cmd");
	if (!jcmd || !json_is_string(jcmd)) {
		printf("Missing 'cmd' field, or 'cmd' is not a string\n");
		return NULL;
	}

	const char *cmd = json_string_value(jcmd);
	if (strncmp(cmd, "get-dev-info", sizeof("get-dev-info") - 1)) {
		printf("Unknown command\n");
		sendRsp("Unknown command\n", 400);
		return NULL;
	}

	////////////////////////////////////////////////////////
	// uid
	json_t *juid = json_object_get(jobj, "uid");
	if (!juid || !json_is_integer(juid)) {
		printf("uid is illegal\n");
		sendRsp("uid is illegal\n", 400);
		return NULL;
	}
	int uid = (int)json_integer_value(juid);

	////////////////////////////////////////////////////////
	// addr
	json_t *jaddr = json_object_get(jobj, "addr");
	if (!jaddr || !json_is_integer(jaddr))
	{
		printf("addr is illegal\n");
		sendRsp("addr is illegal\n", 400);
		return NULL;
	}

	int addr = (int)json_integer_value(jaddr);

	printf("uid: %d, addr: 0x%X\n", uid, (uint32_t)addr);

	if (uid < 0 || uid > 255) {
		printf("Invalid uid or dev-id\n");
		sendRsp("invalid uid or dev-id\n", 200);
		return NULL;
	}

	////////////////////////////////////////////////////////
	// id-list
	json_t *jid_list = json_object_get(jobj, "id-list");
	if (!jid_list || !json_is_string(jid_list)) {
		printf("invalid id-list format\n");
		sendRsp("invalid id-list format\n", 200);
		return NULL;
	}

	const char *id_list = json_string_value(jid_list);
	if (!id_list) {
		printf("invalid id-list format\n");
		sendRsp("invalid id-list format\n", 200);
		return NULL;
	}

	// sendRsp("uid is good, dev-id is good, everything is good:)\n", 200);
	// transfer from json to ZigBee message
	ZInnerGetDevInfoReq *req = new ZInnerGetDevInfoReq(addr_);
	req->addr_ = (uint32_t)(addr & 0xFFFF);

	if (!str2list(id_list, req->item_ids_)) {
		printf("failed to str2list: [%s]\n", id_list);
		delete req;
		req = NULL;
		return NULL;
	}

	return req;
}

ZInnerMsg* ZWebApiHandler::convertSetDevInfoReq(json_t *jobj)
{
	printf("ZWebApiModule::convertSetDevInfoReq()\n");

	json_t *cmd = json_object_get(jobj, "cmd");
	if (!cmd || !json_is_string(cmd)) {
		printf("Missing 'cmd' field, or 'cmd' is not a string\n");
		return NULL;
	}

	const char *cmd_str = json_string_value(cmd);

	if (strncmp(cmd_str, "set-dev-info", sizeof("set-dev-info") - 1)) {
		printf("Unknown command\n");
		sendRsp("Unknown command\n", 400);
		return NULL;
	}

	json_t *juid = json_object_get(jobj, "uid");
	if (!juid || !json_is_integer(juid)) {
		printf("uid is illegal\n");
		sendRsp("uid is illegal\n", 400);
		return NULL;
	}

	json_t *jaddr = json_object_get(jobj, "addr");
	if (!jaddr || !json_is_integer(jaddr)) {
		printf("addr is illegal\n");
		sendRsp("addr is illegal\n", 400);
		return NULL;
	}

	json_t *jvals = json_object_get(jobj, "vals");
	if (!jvals || !json_is_array(jvals)) {
		printf("vals is illeagl\n");
		sendRsp("vals is illeagl\n", 400);
		return NULL;
	}

	int uid = json_integer_value(juid);
	int addr = json_integer_value(jaddr);

	printf("uid: %d, addr: %d\n", uid, addr);
	printf("Item valus:\n");

	// new request
	ZInnerSetDevInfoReq *req = new ZInnerSetDevInfoReq(addr_);
	req->addr_ = addr;

	ZItemPair pair;
	json_t *elem;
	json_t *jid;
	json_t *jval;
	for (uint32_t i = 0; i < json_array_size(jvals); ++i) {
		// elem
		elem = json_array_get(jvals, i);
		if (elem == NULL) {
			printf("bad element!\n");
			delete req;
			return NULL;
		}
		// id
		jid = json_object_get(elem, "id");
		if (jid == NULL || !json_is_integer(jid)) {
			printf("[%u]bad id\n", i);
			delete req;
			return NULL;
		}
		pair.id = json_integer_value(jid);
		// val
		jval = json_object_get(elem, "val");
		if (jval == NULL || !json_is_integer(jval)) {
			printf("[%u, %d]bad val\n", i, pair.id);
			delete req;
			return NULL;
		}
		pair.val = json_integer_value(jval);

		// print it out, for debugging only
		printf("[%u] id=%d, val=%d\n", i, pair.id, pair.val);

		req->dev_vals_.push_back(pair);
	}

	return req;
}

ZInnerMsg* ZWebApiHandler::webMsg2InnerMsg(json_t *web_msg)
{
	// 2. check `cmd' field
	// (sequence value should allocate from server)
	json_t *cmd = json_object_get(web_msg, "cmd");
	if (!cmd || !json_is_string(cmd)) {
		printf("Missing 'cmd' field, or 'cmd' is not a string\n");
		sendRsp("Missing 'cmd' field, or 'cmd' is not a string\n", 400);
		return NULL;
	}

	// 3. check session
	const char *cmd_str = json_string_value(cmd);
	if (!strncmp(cmd_str, "get-dev-list", sizeof("get-dev-list") - 1)) {
		return convertGetDevListReq(web_msg);
	} else if (!strncmp(cmd_str, "get-dev-info", sizeof("get-dev-info") - 1)) {
		return convertGetDevInfoReq(web_msg);
	} else if (!strncmp(cmd_str, "set-dev-info", sizeof("set-dev-info") - 1)) {
		return convertSetDevInfoReq(web_msg);
	} else {
		return NULL;
	}
}

int ZWebApiHandler::onRead(char *buf, uint32_t buf_len)
{
	printf("ZWebApiModule::onRead()\n");

	if (buf_len <= 0) { // MIN_MSG_LEN(header length)
		printf("empty message\n");

		sendRsp("empty message\n", 404);

		return -1;
	}

	trace_bin(buf, buf_len);

	{
		// 1. decode message
		json_error_t error;
		json_t *jobj = json_loadb(buf, buf_len, 0, &error);
		if ((jobj == NULL) || (!json_is_object(jobj))) {
			printf("bad request\n");
			sendRsp("bad request\n", 400);
			return -1;
		}

		ZInnerMsg *inner_msg = webMsg2InnerMsg(jobj);
		if (inner_msg == NULL) {
			sendRsp("bad request\n", 400);
			return -1;
		}

		// set destination address
		inner_msg->dst_addr_.module_type_ = Z_MODULE_SERIAL;
		inner_msg->dst_addr_.handler_id_ = ANY_ID;

		// ZDispatcher::instance()->sendMsg(inner_msg);
		ZDispatcher::instance()->sendDirect(inner_msg);
	}

	return 0;
}


