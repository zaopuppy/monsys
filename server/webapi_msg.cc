#include "webapi_msg.h"

#include <string.h>

#include <jansson.h>

#include "libbase/zlog.h"
#include "zinner_message_ex.h"

// // { "cmd": "HB" }
// ZInnerMsg* json2InnerHB(json_t *jroot)
// {
// 	return new ZPushHBMsg();
// }

ZInnerMsg* json2InnerGetDevListReq(json_t *jobj)
{
	Z_LOG_D("json2InnerGetDevListReq()\n");
	// cmd = get-dev-list;
	json_t *jcmd = json_object_get(jobj, "cmd");
	if (!jcmd || !json_is_string(jcmd)) {
		Z_LOG_D("bad cmd field\n");
		return NULL;
	}

	return new ZInnerGetDevListReq();
}

ZInnerMsg* json2InnerGetDevInfoReq(json_t *jobj)
{
	Z_LOG_D("json2InnerGetDevInfoReq()\n");

	////////////////////////////////////////////////////////
	// command
	json_t *jcmd = json_object_get(jobj, "cmd");
	if (!jcmd || !json_is_string(jcmd)) {
		Z_LOG_D("Missing 'cmd' field, or 'cmd' is not a string\n");
		return NULL;
	}

	const char *cmd = json_string_value(jcmd);
	if (strncmp(cmd, "get-dev-info", sizeof("get-dev-info") - 1)) {
		Z_LOG_D("Unknown command\n");
		return NULL;
	}

	////////////////////////////////////////////////////////
	// uid
	json_t *juid = json_object_get(jobj, "uid");
	if (!juid || !json_is_integer(juid)) {
		Z_LOG_D("uid is illegal\n");
		return NULL;
	}
	int uid = (int)json_integer_value(juid);

	////////////////////////////////////////////////////////
	// addr
	json_t *jaddr = json_object_get(jobj, "addr");
	if (!jaddr || !json_is_integer(jaddr))
	{
		Z_LOG_D("addr is illegal\n");
		return NULL;
	}

	int addr = (int)json_integer_value(jaddr);

	Z_LOG_D("uid: %d, addr: 0x%X\n", uid, (uint32_t)addr);

	if (uid < 0 || uid > 255) {
		Z_LOG_D("Invalid uid or dev-id\n");
		return NULL;
	}

	////////////////////////////////////////////////////////
	// id-list
	json_t *jid_list = json_object_get(jobj, "id-list");
	if (!jid_list || !json_is_string(jid_list)) {
		Z_LOG_D("invalid id-list format\n");
		return NULL;
	}

	const char *id_list = json_string_value(jid_list);
	if (!id_list) {
		Z_LOG_D("invalid id-list format\n");
		return NULL;
	}

	// sendRsp("uid is good, dev-id is good, everything is good:)\n", 200);
	// transfer from json to ZigBee message
	ZInnerGetDevInfoReq *req = new ZInnerGetDevInfoReq();
	req->addr_ = (uint32_t)(addr & 0xFFFF);

	if (!str2list(id_list, req->item_ids_)) {
		Z_LOG_D("failed to str2list: [%s]\n", id_list);
		delete req;
		req = NULL;
		return NULL;
	}

	return req;
}

ZInnerMsg* json2InnerSetDevInfoReq(json_t *jobj)
{
	Z_LOG_D("json2InnerSetDevInfoReq()\n");

	json_t *cmd = json_object_get(jobj, "cmd");
	if (!cmd || !json_is_string(cmd)) {
		Z_LOG_D("Missing 'cmd' field, or 'cmd' is not a string\n");
		return NULL;
	}

	const char *cmd_str = json_string_value(cmd);

	if (strncmp(cmd_str, "set-dev-info", sizeof("set-dev-info") - 1)) {
		Z_LOG_D("Unknown command\n");
		return NULL;
	}

	json_t *juid = json_object_get(jobj, "uid");
	if (!juid || !json_is_integer(juid)) {
		Z_LOG_D("uid is illegal\n");
		return NULL;
	}

	json_t *jaddr = json_object_get(jobj, "addr");
	if (!jaddr || !json_is_integer(jaddr)) {
		Z_LOG_D("addr is illegal\n");
		return NULL;
	}

	json_t *jvals = json_object_get(jobj, "vals");
	if (!jvals || !json_is_array(jvals)) {
		Z_LOG_D("vals is illeagl\n");
		return NULL;
	}

	int uid = json_integer_value(juid);
	int addr = json_integer_value(jaddr);

	Z_LOG_D("uid: %d, addr: %d\n", uid, addr);
	Z_LOG_D("Item valus:\n");

	// new request
	ZInnerSetDevInfoReq *req = new ZInnerSetDevInfoReq();
	req->addr_ = addr;

	ZItemPair pair;
	json_t *elem;
	json_t *jid;
	json_t *jval;
	for (uint32_t i = 0; i < json_array_size(jvals); ++i) {
		// elem
		elem = json_array_get(jvals, i);
		if (elem == NULL) {
			Z_LOG_D("bad element!\n");
			delete req;
			return NULL;
		}
		// id
		jid = json_object_get(elem, "id");
		if (jid == NULL || !json_is_integer(jid)) {
			Z_LOG_D("[%u]bad id\n", i);
			delete req;
			return NULL;
		}
		pair.id = json_integer_value(jid);
		// val
		jval = json_object_get(elem, "val");
		if (jval == NULL || !json_is_integer(jval)) {
			Z_LOG_D("[%u, %d]bad val\n", i, pair.id);
			delete req;
			return NULL;
		}
		pair.val = json_integer_value(jval);

		// print it out, for debugging only
		Z_LOG_D("[%u] id=%d, val=%d\n", i, pair.id, pair.val);

		req->dev_vals_.push_back(pair);
	}

	return req;
}

ZInnerMsg* json2Inner(json_t *jroot)
{
	json_t *jcmd = json_object_get(jroot, "cmd");
	if (!jcmd || !json_is_string(jcmd)) {
		Z_LOG_E("Missing 'cmd' field, or 'cmd' is not a string\n");
		return NULL;
	}

	// 3. check session
	const char *cmd_str = json_string_value(jcmd);
	/* if (!strncmp(cmd_str, "HB", sizeof("HB") - 1)) {
		return json2InnerHB(jroot);
	}	else */if (!strncmp(cmd_str, "get-dev-list", sizeof("get-dev-list") - 1)) {
		return json2InnerGetDevListReq(jroot);
	} else if (!strncmp(cmd_str, "get-dev-info", sizeof("get-dev-info") - 1)) {
		return json2InnerGetDevInfoReq(jroot);
	} else if (!strncmp(cmd_str, "set-dev-info", sizeof("set-dev-info") - 1)) {
		return json2InnerSetDevInfoReq(jroot);
	} else {
		return NULL;
	}
}

json_t* inner2Json(ZInnerGetDevListRsp *msg)
{
	int rv;

	// root
	json_t *jobj = json_object();
	assert(jobj);

	// cmd
	json_t *jcmd = json_string("get-dev-list-rsp");
	rv = json_object_set_new(jobj, "cmd", jcmd);
	if (rv != 0) {
		json_decref(jobj);
		Z_LOG_D("Failed to set 'cmd'\n");
		return NULL;
	}

	// status
	json_t *jstatus;
	jstatus = json_integer(0);
	rv = json_object_set_new(jobj, "status", jstatus);
	if (rv != 0) {
		json_decref(jobj);
		Z_LOG_D("Failed to set 'status'\n");
		return NULL;
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
	return jobj;
}

json_t* inner2Json(ZInnerGetDevInfoRsp *msg)
{
	int rv;

	json_t *jobj = json_object();
	assert(jobj);

	json_t *cmd = json_string("get-dev-info-rsp");
	rv = json_object_set_new(jobj, "cmd", cmd);
	if (rv != 0) {
		json_decref(jobj);
		Z_LOG_D("Failed to set 'cmd'\n");
		return NULL;
	}

	json_t *code;
	if (msg->dev_infos_.size() > 0) {
		code = json_integer(0);
	} else {
		code = json_integer(-1);
	}

	rv = json_object_set_new(jobj, "code", code);
	if (rv != 0) {
		json_decref(jobj);
		return NULL;
	}

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

	return jobj;
}

json_t* inner2Json(ZInnerSetDevInfoRsp *msg)
{
	int rv;

	json_t *jroot = json_object();
	assert(jroot);

	json_t *jcmd = json_string("set-dev-info-rsp");
	rv = json_object_set_new(jroot, "cmd", jcmd);
	if (rv != 0) {
		json_decref(jroot);
		Z_LOG_D("Failed to set 'cmd'\n");
		return NULL;
	}

	json_t *jstatus = json_integer(msg->status_);
	rv = json_object_set_new(jroot, "status", jstatus);
	if (rv != 0) {
		Z_LOG_D("Failed to set 'status'\n");
		json_decref(jroot);
		return NULL;
	}

	return jroot;
}

// 
ZInnerMsg* decodePushMsg(char *buf, uint32_t buf_len)
{
	json_error_t jerror;
	json_t *jroot = json_loadb(buf, buf_len, 0, &jerror);
	if (jroot == NULL || !json_is_object(jroot)) {
		Z_LOG_E("Bad request\n");
		return NULL;
	}

	return json2Inner(jroot);
}

json_t* decodeWebApiMsg(char *buf, uint32_t buf_len)
{
	json_error_t jerror;
	json_t *jmsg = json_loadb(buf, buf_len, 0, &jerror);
	if (jmsg == NULL || !json_is_object(jmsg)) {
		Z_LOG_E("Bad request\n");
		return NULL;
	}

	return jmsg;
}


