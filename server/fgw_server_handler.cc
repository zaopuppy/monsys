#include "fgw_server_handler.h"

#include <unistd.h>

#include "zinner_message_ex.h"
#include "fgw_server.h"
#include "webapi_msg.h"
#include "database.h"
#include "fgw_manager.h"

void FGWServerHandler::setState(int new_state)
{
  Z_LOG_D("changed from [%d] to [%d]", state_, new_state);
  state_ = new_state;
}

int FGWServerHandler::init()
{
  // addr_.module_type_ = getModuleType();
  // addr_.handler_id_ = getId();
  Z_LOG_D("FGWServerHandler::init()");
  Z_LOG_D("fd: %d", getFd());

  // setState(STATE_LOGED_IN);
  setState(STATE_WAIT_FOR_LOGIN);

  return OK;
}

// TODO: move to upper class
void FGWServerHandler::close()
{
  super_::close();
  // XXX
  // server_->removeHandler(this);
  // XXX: remove it to upper class
  ((FGWServer*)getModule())->removeHandler(this);

  FGWManager::instance()->remove_handler("DEVID-Z");
}

int FGWServerHandler::processLoginReq(json_t *jmsg)
{
  Z_LOG_D("FGWServerHandler::processLoginReq");

  json_t *jst = json_object_get(jmsg, "st");
  const char *st = json_string_value(jst);

  json_t *jdevid = json_object_get(jmsg, "devid");
  const char *devid = json_string_value(jdevid);

  char sql[128];
  snprintf(sql, sizeof(sql),
    "SELECT count(1) from `fgw_list` where `device` = '%s' and `pubkey` = '%s'",
    devid, st);

  Z_LOG_D("now ready to query database");
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
  json_t *jcmd = json_string("login-rsp");
  json_object_set_new(jrsp, "cmd", jcmd);

  // XXX: bad! very bad! but now I don't want to take too much time for this crap
  if (row[0][0] == '1') {
    // success
    setState(STATE_LOGED_IN);

    FGWManager::instance()->add_handler("DEVID-Z", getId());

    // status
    json_t *jst = json_integer(0);
    json_object_set_new(jrsp, "result", jst);

  } else {
    Z_LOG_E("Failed to check account");

    // status
    json_t *jst = json_integer(-1);
    json_object_set_new(jrsp, "result", jst);
  }

  char *str_dump = json_dumps(jrsp, 0);

  send(str_dump, strlen(str_dump));

  Z_LOG_D("Message sent");
  trace_bin(str_dump, strlen(str_dump));

  free(str_dump);
  json_decref(jrsp);

  // release result
  mysql_free_result(result);

  return OK;
}

int FGWServerHandler::onRead_WaitForLogin(char *buf, uint32_t buf_len)
{
  Z_LOG_D("FGWServerHandler::onRead_WaitForLogin()");

  // decode first
  json_error_t jerror;
  json_t *jmsg = json_loadb(buf, buf_len, 0, &jerror);
  if (jmsg == NULL || !json_is_object(jmsg)) {
    Z_LOG_E("Failed to decode web message");
    return FAIL;
  }

  json_t *jcmd = json_object_get(jmsg, "cmd");
  if (!jcmd || !json_is_string(jcmd)) {
    Z_LOG_E("Failed to get field 'cmd' from message");
    return FAIL;
  }

  const char *cmd = json_string_value(jcmd);
  if (!cmd) {
    Z_LOG_E("Failed to get string value");
    return FAIL;
  }

  if (strcmp(cmd, "login") == 0) {
    return processLoginReq(jmsg);
  } else {
    // XXX: sercurrity problem,
    // should check the length and content of cmd
    Z_LOG_E("unknown cmd: [%s]", cmd);
    return FAIL;
  }

  return OK;
}

int FGWServerHandler::onRead_LogedIn(char *buf, uint32_t buf_len)
{
  Z_LOG_D("FGWServerHandler::onRead_LogedIn()");

  // decode first
  json_error_t jerror;
  json_t *jmsg = json_loadb(buf, buf_len, 0, &jerror);
  if (jmsg == NULL || !json_is_object(jmsg)) {
    Z_LOG_E("Failed to decode web message");
    return FAIL;
  }

  // convert to inner message
  ZInnerMsg *inner_msg = json2Inner(jmsg);
  if (!inner_msg) {
    Z_LOG_E("failed to convert web message to inner message");
    return FAIL;
  }

  // get seq field
  json_t *jseq = json_object_get(jmsg, "seq");
  if (jseq == NULL || !json_is_integer(jseq)) {
    Z_LOG_E("Field 'seq' doesn't exist, ignore");
    return FAIL;
  }

  uint32_t seq = json_integer_value(jseq);

  // find session
  FGWSession *session = (FGWSession*)session_ctrl_.findByKey2(seq);
  if (session == NULL) {
    Z_LOG_E("Failed to find session by key2: %u", seq);
    return FAIL;
  }

  // send to web api
  inner_msg->dst_addr_ = session->src_addr_;

  ZDispatcher::instance()->sendDirect(inner_msg);

  // remove session
  session_ctrl_.removeByKey1(session->getKey());
  delete session;

  return OK;
}

int FGWServerHandler::onRead(char *buf, uint32_t buf_len)
{
  Z_LOG_D("FGWServerHandler::onRead()");
  trace_bin(buf, buf_len);

  if (state_ == STATE_WAIT_FOR_LOGIN) {
    return onRead_WaitForLogin(buf, buf_len);
  } else if (state_ == STATE_LOGED_IN) {
    return onRead_LogedIn(buf, buf_len);
  } else {
    Z_LOG_E("WTF: unknown state: %d", state_);
    return FAIL;
  }
}

bool FGWServerHandler::checkSessionBySequence(uint32_t sequence)
{
  FGWSession *session = (FGWSession*)session_ctrl_.findByKey1(sequence);
  if (session) {
    return true;
  }

  return false;
}

// int FGWServerHandler::onInnerMsgEx(ZInnerMsg *msg)
// {
//   return OK;
// }

int FGWServerHandler::onInnerMsg(ZInnerMsg *msg)
{
  Z_LOG_D("FGWServerHandler::onInnerMsg");

  if (checkSessionBySequence(msg->seq_)) {
    Z_LOG_D("Duplicated session: [%u]", msg->seq_);
    return FAIL;
  }

  json_t *web_msg = inner2Json(msg);
  if (web_msg == NULL) {
    Z_LOG_D("Failed to convert inner to web message");
    return FAIL;
  }

  char *str_dump = json_dumps(web_msg, 0);

  send(str_dump, strlen(str_dump));

  Z_LOG_D("Message sent");
  trace_bin(str_dump, strlen(str_dump));

  free(str_dump);
  json_decref(web_msg);

  // save session
  {
    // TODO: get inner sequence from ZBGet
    FGWSession *session = new FGWSession();
    // XXX: should use a better way
    session->setKey(msg->seq_);
    session->src_addr_ = msg->src_addr_;
    session->dst_addr_ = msg->dst_addr_;
    // session->extern_key_.u32 = (req.addr_ << 16) | 0x00;

    // XXX: implement the client id
    session_ctrl_.add(msg->seq_, msg->seq_, session);
    Z_LOG_D("session added: key1=%u, key2=%u", msg->seq_, msg->seq_);
    // session_ctrl_1_.add(msg->addr_, session);
  }

  return OK;
}

void FGWServerHandler::routine(long delta)
{
  FGWSession *session;

  SESSION_CTRL_TYPE::iterator iter = session_ctrl_.begin();
  SESSION_CTRL_TYPE::iterator tmp_iter;

  while (iter != session_ctrl_.end()) {
    session = iter->second;
    session->doTimeout(delta);
    if (session->isComplete()) {
      tmp_iter = iter;
      ++iter;

      delete tmp_iter->second;
      session_ctrl_.erase(tmp_iter);

    } else {
      ++iter;
    }
  }

}

int FGWServerHandler::send(const char *buf, uint32_t buf_len)
{
  Z_LOG_D("FGWServerHandler::send(%d)", getFd());
  int rv = ::send(getFd(), buf, buf_len, 0);
  Z_LOG_D("rv: %d", rv);
  return OK;
}

static push::Msg* inner2pushGetDevList(ZInnerMsg *innerMsg)
{
  return NULL;
}

static push::Msg* inner2pushGetDev(ZInnerMsg *innerMsg)
{
  return NULL;
}

static push::Msg* inner2pushSetDev(ZInnerMsg *innerMsg)
{
  return NULL;
}

push::Msg* FGWServerHandler::inner2push(ZInnerMsg *innerMsg)
{
  Z_LOG_D("FGWServerHandler::inner2push(%p)", innerMsg);

  push::Msg *pushMsg = NULL;

  switch (innerMsg->msg_type_) {
    case Z_ZB_GET_DEV_LIST_REQ:
      pushMsg = inner2pushGetDevList(innerMsg);
      break;
    case Z_ZB_GET_DEV_REQ:
      pushMsg = inner2pushGetDev(innerMsg);
      break;
    case Z_ZB_SET_DEV_REQ:
      pushMsg = inner2pushSetDev(innerMsg);
      break;
    default:
      break;
  }

  return pushMsg;
}

ZInnerMsg* FGWServerHandler::push2inner(push::Msg *pushMsg)
{
  Z_LOG_D("FGWServerHandler::push2inner()");
  return NULL;
}
