#include "fgw_client_handler.h"

#include <unistd.h>
#include <string.h>

#include "webapi_msg.h"

void FGWClientHandler::setState(int new_state)
{
  Z_LOG_D("changed from [%d] to [%d]", state_, new_state);
  state_ = new_state;
}

int FGWClientHandler::init()
{
  addr_.module_type_ = getModuleType();
  addr_.handler_id_ = getId();

  state_ = STATE_UNREGISTERED;
  // state_ = STATE_REGISTERED;

  return OK;
}

json_t* createLoginReq()
{
  json_t *jmsg = json_object();
  assert(jmsg);

  // cmd
  json_t *jcmd = json_string("login");
  json_object_set_new(jmsg, "cmd", jcmd);

  // st
  json_t *jst = json_string("ZHAOYIC");
  json_object_set_new(jmsg, "st", jst);

  // devid
  json_t *jdevid = json_string("DEVID-Z");
  json_object_set_new(jmsg, "devid", jdevid);

  return jmsg;
}

void FGWClientHandler::fgwLogin()
{
  Z_LOG_D("FGWClientHandler::registerFGW()");

  json_t *login_req = createLoginReq();

  sendJson(login_req);

  state_ = STATE_WAIT_FOR_SERVER;

  // set timer
  login_timer_id = timer_.setTimer(10 * 1000);
  if (login_timer_id < 0) {
    Z_LOG_E("Failed to set timer, there's nothing we can do now");
  }
}

void FGWClientHandler::close()
{
  ::close(fd_);
  fd_ = -1;
  event_free(read_event_);
  read_event_ = NULL;
}

int FGWClientHandler::onRead_Unregistered(char *buf, uint32_t buf_len)
{
  Z_LOG_D("FGWClientHandler::onRead_Unregistered()");

  // return not-register
  sendRsp("Not-register(i)", 404);

  return OK;
}

int FGWClientHandler::processLoginRsp(json_t *jmsg)
{
  Z_LOG_D("FGWClientHandler::processLoginRsp()");

  json_t *jresult = json_object_get(jmsg, "result");
  int result = json_integer_value(jresult);
  if (result != 0) {
    Z_LOG_E("Failed to login to server, result is [%d]", result);
    // TODO: should we retry or not?
    return FAIL;
  }

  Z_LOG_D("Good, login success");
  setState(STATE_REGISTERED);

  // cancel timer
  timer_.cancelTimer(login_timer_id);
  login_timer_id = -1;

  return OK;
}

int FGWClientHandler::onRead_WaitForServer(char *buf, uint32_t buf_len)
{
  Z_LOG_D("FGWClientHandler::onRead_WaitForServer()");

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

  if (strcmp(cmd, "login-rsp") == 0) {
    return processLoginRsp(jmsg);
  } else {
    // XXX: sercurrity problem,
    // should check the length and content of cmd
    Z_LOG_E("unknown cmd: [%s]", cmd);
    return FAIL;
  }

  return OK;
}

FGWClientSession* FGWClientHandler::createSession(ZInnerMsg *inner_msg)
{
  Z_LOG_D("FGWClientHandler::createSession()");

  switch (inner_msg->msg_type_)
  {
    case Z_ZB_GET_DEV_LIST_REQ:
    case Z_ZB_GET_DEV_REQ:
    case Z_ZB_SET_DEV_REQ:
    case Z_ZB_PRE_BIND_REQ:
    case Z_ZB_BIND_REQ:
      return new FGWClientSession(this);
    default:
    {
      Z_LOG_E("Unknown message type");
      return NULL;
    }
  }
}

int FGWClientHandler::onRead_Registered(char *buf, uint32_t buf_len)
{
  Z_LOG_D("FGWClientHandler::onRead_Registered()");

  if (buf_len <= 0) { // MIN_MSG_LEN(header length)
    Z_LOG_D("empty message");

    sendRsp("empty message", 404);

    return -1;
  }

  // Z_LOG_D("Received message from FGW server");
  // trace_bin(buf, buf_len);

  // decode
  json_t* jmsg = decodeWebApiMsg(buf, buf_len);

  // sequence
  json_t *jseq = json_object_get(jmsg, "seq");
  if (jseq == NULL || !json_is_integer(jseq)) {
   return -1;
  }

  ZInnerMsg *inner_msg = json2Inner(jmsg);
  if (inner_msg == NULL) {
    sendRsp("bad request", 400);
    return -1;
  }
  inner_msg->seq_ = json_integer_value(jseq);

  FGWClientSession *session = createSession(inner_msg);
  if (NULL == session) {
    return FAIL;
  }

  session->event(inner_msg);
  if (!session->isComplete()) {
    Z_LOG_D("Added: key1=%u, key2=%u", inner_msg->seq_, inner_msg->seq_);

    session_ctrl_.add(inner_msg->seq_, inner_msg->seq_, session);
  } else {
    delete session;
    session = NULL;
  }

  return OK;
}

int FGWClientHandler::onRead(char *buf, uint32_t buf_len)
{
  Z_LOG_D("FGWClientHandler::onRead()");
  trace_bin(buf, buf_len);

  int rv = FAIL;
  switch (state_) {
    case STATE_UNREGISTERED:
      rv = onRead_Unregistered(buf, buf_len);
      break;
    case STATE_WAIT_FOR_SERVER:
      rv = onRead_WaitForServer(buf, buf_len);
      break;
    case STATE_REGISTERED:
      rv = onRead_Registered(buf, buf_len);
      break;
    default:
      rv = FAIL;
  }

  return rv;
}

int FGWClientHandler::onInnerMsg(ZInnerMsg *msg)
{
  Z_LOG_D("FGWClientHandler::onInnerMsg");
  Z_LOG_D("inner message type: %d", msg->msg_type_);

  // find session first
  FGWClientSession *session = session_ctrl_.findByKey1(msg->seq_);
  if (session == NULL) {
    Z_LOG_D("No session was found for key: %u", msg->seq_);
    return FAIL;
  }

  session->event(msg);
  if (session->isComplete()) {
    session_ctrl_.removeByKey1(msg->seq_);
    delete session;
    session = NULL;
  }

  return OK;
}


void FGWClientHandler::routine(long delta)
{
  // Z_LOG_D("FGWClientHandler::routine()");

  FGWClientSession *session;

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

int FGWClientHandler::send(const char *buf, uint32_t buf_len)
{
  Z_LOG_D("FGWClientHandler::send(%d)", fd_);
  return ::send(fd_, buf, buf_len, 0);
}

int FGWClientHandler::sendJson(json_t *jmsg)
{
  Z_LOG_D("FGWClientHandler::sendJson()");

  char *str_dump = json_dumps(jmsg, 0);

  int rv = send(str_dump, strlen(str_dump));
  trace_bin(str_dump, strlen(str_dump));

  free(str_dump);
  json_decref(jmsg);

  return rv;
}

void FGWClientHandler::onConnected()
{
  Z_LOG_D("FGWClientHandler::onConnected()");

  // reset state
  setState(STATE_UNREGISTERED);

  fgwLogin();
}

void FGWClientHandler::sendRsp(const char *text_msg, int status)
{
  Z_LOG_D("FGWClientHandler::sendRsp");
  int rv = send(text_msg, strlen(text_msg));
  Z_LOG_D("sent %d bytes", rv);
}

void FGWClientHandler::onTimeout(int handler_id)
{
  Z_LOG_D("FGWClientHandler::onTimeout(%d)", handler_id);
  if (handler_id == login_timer_id) {
    if (state_ == STATE_WAIT_FOR_SERVER) {
      setState(STATE_UNREGISTERED);
      fgwLogin();
    }
  } else {
    Z_LOG_E("unknown handler id: ", handler_id);
  }
}

