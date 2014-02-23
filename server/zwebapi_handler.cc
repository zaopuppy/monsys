#include "zwebapi_handler.h"

#include <unistd.h>
#include <assert.h>
#include <errno.h>

#include "libframework/zframework.h"
#include "libzigbee/zzigbee_message.h"
#include "libbase/zlog.h"

#include "module.h"

#include "webapi_msg.h"
#include "zwebapi_server.h"
#include "fgw_manager.h"
#include "database.h"

//////////////////////////////////////////////////////
int ZWebApiHandler::init() {
	Z_LOG_D("[%p] Oops, client's coming", this);

	// init address here
	addr_.module_type_ = getModuleType();
	addr_.handler_id_ = getId();

	return 0;
}

void ZWebApiHandler::close() {
	super_::close();
	// XXX: remove it to upper class
	((ZWebApiServer*)getModule())->removeHandler(this);
}

int ZWebApiHandler::onInnerMsg(ZInnerMsg *msg)
{
	Z_LOG_D("ZWebApiHandler::onInnerMsg()");

	json_t *web_msg = inner2Json(msg);
	if (web_msg == NULL) {
		Z_LOG_E("Failed to convert inner message to web message");
		return FAIL;
	}

	send(web_msg);
	json_decref(web_msg);

	return OK;
}

void ZWebApiHandler::routine(long delta)
{
}

void ZWebApiHandler::sendRsp(const char *text_msg, int status)
{
	Z_LOG_D("ZWebApiHandler::sendRsp");
	int rv = send(text_msg, strlen(text_msg));
	Z_LOG_D("sent %d bytes", rv);
}

int ZWebApiHandler::onRead(char *buf, uint32_t buf_len)
{
	Z_LOG_D("ZWebApiModule::onRead(fd_=%d)", getFd());

	if (buf_len <= 0) { // MIN_MSG_LEN(header length)
		Z_LOG_D("empty message");
		sendRsp("empty message", 404);
		return -1;
	}

	trace_bin(buf, buf_len);

	// decode first
	json_error_t jerror;
	json_t *jmsg = json_loadb(buf, buf_len, 0, &jerror);
	if (jmsg == NULL || !json_is_object(jmsg)) {
		Z_LOG_D("Failed to decode web message");
		sendRsp("Failed to decode web message", 404);
		return FAIL;
	}

  // for `get-fgw-list'
	{
		json_t *jcmd = json_object_get(jmsg, "cmd");
		if (jcmd == NULL || !json_is_string(jcmd)) {
			Z_LOG_D("Bad request, cmd field is missing");
			sendRsp("Bad request, cmd field is missing", 404);
			return FAIL;
		}

		// TODO: check length
		const char *cmd = json_string_value(jcmd);
		if (0 == strcmp("get-fgw-list", cmd)) {
			return processFGWGetListReq(jmsg);
		}
	}

	// find fgw first
	int handler_id = INVALID_ID;
	{
		json_t *jfgw = json_object_get(jmsg, "fgw");
		if (!jfgw || !json_is_string(jfgw)) {
			Z_LOG_D("Missing fgw field");
			sendRsp("fgw missed", 404);
			return FAIL;
		}

		const char *fgw = json_string_value(jfgw);

		handler_id = FGWManager::instance()->find_handler(fgw);
		if (handler_id == INVALID_ID) {
			Z_LOG_D("no such fgw");
			sendRsp("No such FGW", 404);
			return FAIL;
		}
	}

	// convert to inner message
	ZInnerMsg *inner_msg = json2Inner(jmsg);
	if (inner_msg == NULL) {
		Z_LOG_D("Failed to convert web message to inner message");
		return FAIL;
	}

	// add 'seq' field
	inner_msg->seq_ = getId();

	inner_msg->src_addr_ = addr_;
	inner_msg->dst_addr_.module_type_ = MODULE_FGW_SERVER;
	inner_msg->dst_addr_.handler_id_ = handler_id;

	ZDispatcher::instance()->sendDirect(inner_msg);

	return 0;
}

int ZWebApiHandler::processFGWGetListReq(json_t *jmsg)
{
	Z_LOG_D("ZWebApiHandler::processFGWGetListReq");

	json_t *jaccount = json_object_get(jmsg, "account");
	if (jaccount == NULL || !json_is_string(jaccount)) {
		Z_LOG_D("bad request, account needed");
		sendRsp("bad request, account needed", 404);
		return FAIL;
	}
	const char *account = json_string_value(jaccount);

	// FIXME: security problem
	char sql[128];
  snprintf(sql, sizeof(sql),
    "SELECT `fgw_list` from `account_info` where `account` = '%s'", account);

  Z_LOG_D("now ready to query database: [%s]", sql);
  MYSQL_RES *result = MySQLDatabase::instance()->query(sql);
  if (result == NULL) {
    Z_LOG_E("Failed to query database");
    return FAIL;
  }

  Z_LOG_D("query over");

  int row_num = mysql_num_rows(result);
  Z_LOG_D("returned %d rows", row_num);

  if (row_num < 1) {
    Z_LOG_E("returned 0 rows?? how could it be possible...");
    return FAIL;
  }

  MYSQL_ROW row = mysql_fetch_row(result);

  // create response
  json_t *jrsp = json_object();
  assert(jrsp);

  // cmd
  json_t *jcmd = json_string("get-fgw-list-rsp");
  json_object_set_new(jrsp, "cmd", jcmd);

  // XXX: bad! very bad! but now I don't want to take too much time for this crap
  const char *fgw_list_str = row[0];

  Z_LOG_D("fgw list string: [%s]", fgw_list_str);

  // transfer to list
  std::vector<std::string> fgw_list;
  {
  	int begin_pos = 0;
  	int end_pos = 0;
  	const char *p = fgw_list_str;

  	while (true) {
	  	while (p[end_pos] && p[end_pos] != '|') {
	  		++end_pos;
	  	}

	  	if (end_pos > begin_pos) {
		  	std::string v(p, begin_pos, end_pos - begin_pos);
		  	fgw_list.push_back(v);
	  	}

	  	if (p[end_pos] == 0x00) {
	  		// over
	  		break;
	  	}

	  	++end_pos; // skip '|'
	  	begin_pos = end_pos;
	  }

	  Z_LOG_D("formated list length: [%lu]", fgw_list.size());
  }

  json_t *jfgws = json_array();
  for (uint32_t i = 0; i < fgw_list.size(); ++i) {
  	json_t *jfgw = json_string(fgw_list[i].c_str());
  	json_array_append(jfgws, jfgw);
  }

  json_object_set_new(jrsp, "fgws", jfgws);

  send(jrsp);

	json_decref(jmsg);

	return OK;
}

int ZWebApiHandler::send(json_t *jmsg)
{
	char *str_dump = json_dumps(jmsg, 0);

	int rv = send(str_dump, strlen(str_dump));

	Z_LOG_D("Message sent");
	trace_bin(str_dump, strlen(str_dump));

	free(str_dump);

	return rv;
}


