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


json_t* inner2Json(ZInnerMsg *innerMsg)
{
	switch (innerMsg->msg_type_) {
		case Z_ZB_GET_DEV_LIST_REQ:
			return inner2Json((ZInnerGetDevListReq*)innerMsg);
		case Z_ZB_GET_DEV_LIST_RSP:
			return inner2Json((ZInnerGetDevListRsp*)innerMsg);
		case Z_ZB_GET_DEV_REQ:
			return inner2Json((ZInnerGetDevInfoReq*)innerMsg);
		case Z_ZB_GET_DEV_RSP:
			return inner2Json((ZInnerGetDevInfoRsp*)innerMsg);
		case Z_ZB_SET_DEV_REQ:
			return inner2Json((ZInnerSetDevInfoReq*)innerMsg);
		case Z_ZB_SET_DEV_RSP:
			return inner2Json((ZInnerSetDevInfoRsp*)innerMsg);
		default:
			return NULL;
	}
}

ZInnerMsg* json2InnerGetDevListReq(json_t *jmsg)
{
	Z_LOG_D("json2InnerGetDevListReq()");

	// cmd = get-dev-list;
	json_t *jcmd = json_object_get(jmsg, "cmd");
	if (!jcmd || !json_is_string(jcmd)) {
		Z_LOG_D("bad cmd field");
		return NULL;
	}

	int rv;
	const char *cmd = json_string_value(jcmd);
	rv = strncmp(cmd, "get-dev-list", sizeof("get-dev-list") - 1);
	if (rv != 0) {
		Z_LOG_E("Not get-dev-list message");
		return NULL;
	}

	// uid
	json_t *juid = json_object_get(jmsg, "uid");
	if (!juid || !json_is_string(juid)) {
		Z_LOG_E("bad uid");
		return NULL;
	}

	// XXX ??
	// // sequence
	// json_t *jseq = json_object_get(jmsg, "seq");
	// if (jseq == NULL || !json_is_integer(jseq)) {
	// 	return NULL;
	// }

	ZInnerGetDevListReq *inner_msg = new ZInnerGetDevListReq();
	// inner_msg->seq_ = json_integer_value(jseq);
	inner_msg->uid_ = json_string_value(juid);

	return inner_msg;
}

ZInnerMsg* json2InnerGetDevListRsp(json_t *jmsg)
{
	Z_LOG_D("json2InnerGetDevListRsp()");

	json_t *jcmd = json_object_get(jmsg, "cmd");
	if (!jcmd || !json_is_string(jcmd)) {
		Z_LOG_D("bad cmd field");
		return NULL;
	}

	int rv;
	const char *cmd = json_string_value(jcmd);
	rv = strncmp(cmd, "get-dev-list-rsp", sizeof("get-dev-list-rsp") - 1);
	if (rv != 0) {
		Z_LOG_E("Not get-dev-list-rsp message");
		return NULL;
	}

	// status
	json_t *jstatus = json_object_get(jmsg, "status");
	if (!jstatus || !json_is_integer(jstatus)) {
		Z_LOG_E("bad uid");
		return NULL;
	}

	json_t *dev_infos = json_object_get(jmsg, "devs");
	if (!dev_infos || !json_is_array(dev_infos)) {
		Z_LOG_E("No dev-infos array");
		return NULL;
	}

	ZInnerGetDevListRsp *inner_msg = new ZInnerGetDevListRsp();
	inner_msg->status_ = json_integer_value(jstatus);

	uint32_t len = json_array_size(dev_infos);
	ZZBDevInfo *dev_info = NULL;
	// XXX: problem, we still need a inner-message for comunicating
	// between center and fgw
	for (uint32_t i = 0; i < len; ++i) {
		dev_info = new ZZBDevInfo();

		// addr
		json_t *jobj = json_array_get(dev_infos, i);
		json_t *jaddr = json_object_get(jobj, "addr");
		if (!jaddr || !json_is_integer(jaddr)) {
			Z_LOG_E("addr is not exist");
			return NULL;
		}
		dev_info->addr_ = json_integer_value(jaddr);

		// name
		json_t *jname = json_object_get(jobj, "name");
		if (!jname || !json_is_string(jname)) {
			return NULL;
		}
		dev_info->name_ = json_string_value(jname);

		// state
		json_t *jstate = json_object_get(jobj, "state");
		if (!jstate || !json_is_integer(jstate)) {
			return NULL;
		}
		dev_info->state_ = json_integer_value(jstate);

		// XX: mac_
		// type
		json_t *jtype = json_object_get(jobj, "type");
		if (!jstate || !json_is_integer(jstate)) {
			return NULL;
		}
		dev_info->state_ = json_integer_value(jstate);

		// dev_info
		inner_msg->info_list_.push_back(dev_info);
	}

	return inner_msg;
}


ZInnerMsg* json2InnerGetDevInfoReq(json_t *jobj)
{
	Z_LOG_D("json2InnerGetDevInfoReq()");

	////////////////////////////////////////////////////////
	// command
	json_t *jcmd = json_object_get(jobj, "cmd");
	if (!jcmd || !json_is_string(jcmd)) {
		Z_LOG_D("Missing 'cmd' field, or 'cmd' is not a string");
		return NULL;
	}

	const char *cmd = json_string_value(jcmd);
	if (strncmp(cmd, "get-dev-info", sizeof("get-dev-info") - 1)) {
		Z_LOG_D("Unknown command");
		return NULL;
	}

	////////////////////////////////////////////////////////
	// uid
	json_t *juid = json_object_get(jobj, "uid");
	if (!juid || !json_is_string(juid)) {
		Z_LOG_D("uid is illegal");
		return NULL;
	}
	// int uid = (int)json_integer_value(juid);

	////////////////////////////////////////////////////////
	// addr
	json_t *jaddr = json_object_get(jobj, "addr");
	if (!jaddr || !json_is_integer(jaddr))
	{
		Z_LOG_D("addr is illegal");
		return NULL;
	}

	int addr = (int)json_integer_value(jaddr);

	Z_LOG_D("uid: %s, addr: 0x%X", json_string_value(juid), (uint32_t)addr);

	// if (uid < 0 || uid > 255) {
	// 	Z_LOG_D("Invalid uid or dev-id");
	// 	return NULL;
	// }

	////////////////////////////////////////////////////////
	// id-list
	json_t *jid_list = json_object_get(jobj, "id-list");
	if (!jid_list || !json_is_string(jid_list)) {
		Z_LOG_D("invalid id-list format");
		return NULL;
	}

	const char *id_list = json_string_value(jid_list);
	if (!id_list) {
		Z_LOG_D("invalid id-list format");
		return NULL;
	}

	// sendRsp("uid is good, dev-id is good, everything is good:)", 200);
	// transfer from json to ZigBee message
	ZInnerGetDevInfoReq *req = new ZInnerGetDevInfoReq();
	req->uid_ = json_string_value(juid);
	req->addr_ = (uint32_t)(addr & 0xFFFF);

	if (!str2list(id_list, req->item_ids_)) {
		Z_LOG_D("failed to str2list: [%s]", id_list);
		delete req;
		req = NULL;
		return NULL;
	}

	return req;
}

ZInnerMsg* json2InnerGetDevInfoRsp(json_t *jobj)
{
	Z_LOG_D("json2InnerGetDevInfoRsp()");

	////////////////////////////////////////////////////////
	// command
	json_t *jcmd = json_object_get(jobj, "cmd");
	if (!jcmd || !json_is_string(jcmd)) {
		Z_LOG_D("Missing 'cmd' field, or 'cmd' is not a string");
		return NULL;
	}

	const char *cmd = json_string_value(jcmd);
	if (strncmp(cmd, "get-dev-info-rsp", sizeof("get-dev-info-rsp") - 1)) {
		Z_LOG_D("Unknown command");
		return NULL;
	}

	////////////////////////////////////////////////////////
	// status
	json_t *jstatus = json_object_get(jobj, "status");
	if (!jstatus || !json_is_integer(jstatus)) {
		Z_LOG_D("status is illegal");
		return NULL;
	}

	////////////////////////////////////////////////////////
	// list
	json_t *jinfo_list = json_object_get(jobj, "id-infos");
	if (!jinfo_list || !json_is_array(jinfo_list)) {
		Z_LOG_D("invalid info-list format");
		return NULL;
	}

	ZInnerGetDevInfoRsp *msg = new ZInnerGetDevInfoRsp();
	msg->status_ = json_integer_value(jstatus);

	uint32_t info_list_len = json_array_size(jinfo_list);
	for (uint32_t i = 0; i < info_list_len; ++i) {
		ZItemPair pair;
		json_t *jobj = json_array_get(jinfo_list, i);

		json_t *jid = json_object_get(jobj, "id");
		pair.id = json_integer_value(jid);

		json_t *jvalue = json_object_get(jobj, "value");
		pair.id = json_integer_value(jvalue);

		msg->dev_infos_.push_back(pair);
	}

	return msg;
}


ZInnerMsg* json2InnerSetDevInfoReq(json_t *jobj)
{
	Z_LOG_D("json2InnerSetDevInfoReq()");

	// cmd
	json_t *cmd = json_object_get(jobj, "cmd");
	if (!cmd || !json_is_string(cmd)) {
		Z_LOG_D("Missing 'cmd' field, or 'cmd' is not a string");
		return NULL;
	}

	const char *cmd_str = json_string_value(cmd);

	if (strncmp(cmd_str, "set-dev-info", sizeof("set-dev-info") - 1)) {
		Z_LOG_D("Unknown command");
		return NULL;
	}

	json_t *juid = json_object_get(jobj, "uid");
	if (!juid || !json_is_integer(juid)) {
		Z_LOG_D("uid is illegal");
		return NULL;
	}

	json_t *jaddr = json_object_get(jobj, "addr");
	if (!jaddr || !json_is_integer(jaddr)) {
		Z_LOG_D("addr is illegal");
		return NULL;
	}

	json_t *jvals = json_object_get(jobj, "vals");
	if (!jvals || !json_is_array(jvals)) {
		Z_LOG_D("vals is illeagl");
		return NULL;
	}

	// int uid = json_integer_value(juid);
	int addr = json_integer_value(jaddr);

	Z_LOG_D("addr: %d", addr);
	Z_LOG_D("Item valus:");

	// new request
	ZInnerSetDevInfoReq *req = new ZInnerSetDevInfoReq();
	req->uid_ = json_string_value(juid);
	req->addr_ = addr;

	ZItemPair pair;
	json_t *elem;
	json_t *jid;
	json_t *jval;
	for (uint32_t i = 0; i < json_array_size(jvals); ++i) {
		// elem
		elem = json_array_get(jvals, i);
		if (elem == NULL) {
			Z_LOG_D("bad element!");
			delete req;
			return NULL;
		}
		// id
		jid = json_object_get(elem, "id");
		if (jid == NULL || !json_is_integer(jid)) {
			Z_LOG_D("[%u]bad id", i);
			delete req;
			return NULL;
		}
		pair.id = json_integer_value(jid);
		// val
		jval = json_object_get(elem, "val");
		if (jval == NULL || !json_is_integer(jval)) {
			Z_LOG_D("[%u, %d]bad val", i, pair.id);
			delete req;
			return NULL;
		}
		pair.val = json_integer_value(jval);

		// print it out, for debugging only
		Z_LOG_D("[%u] id=%d, val=%d", i, pair.id, pair.val);

		req->dev_vals_.push_back(pair);
	}

	return req;
}

ZInnerMsg* json2InnerSetDevInfoRsp(json_t *jobj)
{
	Z_LOG_D("json2InnerSetDevInfoRsp()");

	////////////////////////////////////////////////////////
	// command
	json_t *jcmd = json_object_get(jobj, "cmd");
	if (!jcmd || !json_is_string(jcmd)) {
		Z_LOG_D("Missing 'cmd' field, or 'cmd' is not a string");
		return NULL;
	}

	const char *cmd = json_string_value(jcmd);
	if (strncmp(cmd, "set-dev-info-rsp", sizeof("set-dev-info-rsp") - 1)) {
		Z_LOG_D("Unknown command");
		return NULL;
	}

	////////////////////////////////////////////////////////
	// status
	json_t *jstatus = json_object_get(jobj, "status");
	if (!jstatus || !json_is_integer(jstatus)) {
		Z_LOG_D("status is illegal");
		return NULL;
	}

	ZInnerSetDevInfoRsp *msg = new ZInnerSetDevInfoRsp();
	msg->status_ = json_integer_value(jstatus);

	return msg;
}


ZInnerMsg* json2Inner(json_t *jroot)
{
	json_t *jcmd = json_object_get(jroot, "cmd");
	if (!jcmd || !json_is_string(jcmd)) {
		Z_LOG_E("Missing 'cmd' field, or 'cmd' is not a string");
		return NULL;
	}

	// 3. check session
	// XXX: use strncmp instead of strcmp, but...take care of compared length
	const char *cmd_str = json_string_value(jcmd);
	/* if (!strncmp(cmd_str, "HB", sizeof("HB") - 1)) {
		return json2InnerHB(jroot);
	}	else */
	if (!strcmp(cmd_str, "get-dev-list")) {
		return json2InnerGetDevListReq(jroot);
	}	else if (!strcmp(cmd_str, "get-dev-list-rsp")) {
		return json2InnerGetDevListRsp(jroot);

	} else if (!strcmp(cmd_str, "get-dev-info")) {
		return json2InnerGetDevInfoReq(jroot);
	} else if (!strcmp(cmd_str, "get-dev-info-rsp")) {
		return json2InnerGetDevInfoRsp(jroot);

	} else if (!strcmp(cmd_str, "set-dev-info")) {
		return json2InnerSetDevInfoReq(jroot);
	} else if (!strcmp(cmd_str, "set-dev-info-rsp")) {
		return json2InnerSetDevInfoRsp(jroot);
	} else {
		return NULL;
	}
}

json_t* inner2Json(ZInnerGetDevListReq *msg)
{
	int rv;

	// root
	json_t *jmsg = json_object();
	assert(jmsg);

	// cmd
	json_t *jcmd = json_string("get-dev-list");
	rv = json_object_set_new(jmsg, "cmd", jcmd);
	if (rv != 0) {
		json_decref(jmsg);
		json_decref(jcmd);
		return NULL;
	}

	// uid
	json_t *juid = json_string(msg->uid_.c_str());
	json_object_set_new(jmsg, "uid", juid);

	// seq
	json_t *jseq = json_integer(msg->seq_);
	json_object_set_new(jmsg, "seq", jseq);

	return jmsg;
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
		Z_LOG_D("Failed to set 'cmd'");
		return NULL;
	}

	// status
	json_t *jstatus;
	jstatus = json_integer(0);
	rv = json_object_set_new(jobj, "status", jstatus);
	if (rv != 0) {
		json_decref(jobj);
		Z_LOG_D("Failed to set 'status'");
		return NULL;
	}

	// dev array
	json_t *jdevs = json_array();

	for (uint32_t i = 0; i < msg->info_list_.size(); ++i)	{
		json_t *jelem = json_object();

		// addr
		json_t *jaddr = json_integer(msg->info_list_[i]->addr_);
		json_object_set_new(jelem, "addr", jaddr);

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

json_t* inner2Json(ZInnerGetDevInfoReq *msg)
{
	int rv;

	// root
	json_t *jmsg = json_object();
	assert(jmsg);

	// cmd
	json_t *jcmd = json_string("get-dev-info");
	rv = json_object_set_new(jmsg, "cmd", jcmd);
	if (rv != 0) {
		json_decref(jmsg);
		json_decref(jcmd);
		return NULL;
	}

	// uid
	json_t *juid = json_string(msg->uid_.c_str());
	json_object_set_new(jmsg, "uid", juid);

	// seq
	json_t *jseq = json_integer(msg->seq_);
	json_object_set_new(jmsg, "seq", jseq);

	// addr
	json_t *jaddr = json_integer(msg->addr_);
	rv = json_object_set_new(jmsg, "addr", jaddr);
	if (rv != 0) {
		json_decref(jmsg);
		json_decref(jaddr);
	}

	// id-list
	json_t *jid_list = json_array();

	uint32_t id_list_len = msg->item_ids_.size();
	for (uint32_t i = 0; i < id_list_len; ++i) {
		json_t *jobj = json_integer(msg->item_ids_[i]);
		json_array_append_new(jid_list, jobj);
	}
	json_object_set_new(jmsg, "id-list", jid_list);

	return jmsg;
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
		Z_LOG_D("Failed to set 'cmd'");
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

json_t* inner2Json(ZInnerSetDevInfoReq *msg)
{
	int rv;

	json_t *jmsg = json_object();
	assert(jmsg);

	// cmd
	json_t *jcmd = json_string("set-dev-info");
	rv = json_object_set_new(jmsg, "cmd", jcmd);
	if (rv != 0) {
		json_decref(jmsg);
		json_decref(jcmd);
		Z_LOG_D("Failed to set 'cmd'");
		return NULL;
	}

	// uid
	json_t *juid = json_string(msg->uid_.c_str());
	json_object_set_new(jmsg, "uid", juid);

	// seq
	json_t *jseq = json_integer(msg->seq_);
	json_object_set_new(jmsg, "seq", jseq);

	//
	json_t *jvals = json_array();

	uint32_t vals_len = msg->dev_vals_.size();
	for (uint32_t i = 0; i < vals_len; ++i) {
		ZItemPair &pair = msg->dev_vals_[i];

		json_t *jobj = json_object();

		json_t *jid = json_integer(pair.id);
		json_object_set_new(jobj, "id", jid);

		json_t *jval = json_integer(pair.val);
		json_object_set_new(jobj, "val", jval);

		json_array_append_new(jvals, jobj);
	}

	json_object_set_new(jmsg, "vals", jvals);

	return jmsg;
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
		Z_LOG_D("Failed to set 'cmd'");
		return NULL;
	}

	json_t *jstatus = json_integer(msg->status_);
	rv = json_object_set_new(jroot, "status", jstatus);
	if (rv != 0) {
		Z_LOG_D("Failed to set 'status'");
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
		Z_LOG_E("Bad request");
		return NULL;
	}

	return json2Inner(jroot);
}

json_t* decodeWebApiMsg(char *buf, uint32_t buf_len)
{
	json_error_t jerror;
	json_t *jmsg = json_loadb(buf, buf_len, 0, &jerror);
	if (jmsg == NULL || !json_is_object(jmsg)) {
		Z_LOG_E("Bad request");
		return NULL;
	}

	return jmsg;
}


