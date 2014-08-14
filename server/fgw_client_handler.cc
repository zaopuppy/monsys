#include "fgw_client_handler.h"

#include <unistd.h>
#include <string.h>

#include "webapi_msg.h"
#include "push.pb.h"
// #include "google/protobuf/io/coded_stream.h"
#include "protobuf_helper.h"
#include "protobuf_convert.h"

using namespace com::letsmidi::monsys::protocol;

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

void FGWClientHandler::fgwLogin()
{
  Z_LOG_D("FGWClientHandler::registerFGW()");

  PushMsg push_msg;// = new PushMsg();
  push_msg.set_version(1);
  push_msg.set_type(LOGIN);

  Login *login = new Login();
  login->set_device_id("DEVID-Z");

  push_msg.set_allocated_login(login);

  int rv = protobuf_encode(&push_msg, out_buf_, sizeof(out_buf_));
  if (rv < 0) {
    Z_LOG_E("Failed to encode push msg");
    return;
  }

  send(out_buf_, rv);

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

int FGWClientHandler::processLoginRsp(PushMsg *push_msg)
{
  Z_LOG_D("FGWClientHandler::processLoginRsp()");
  if (!push_msg->has_login_rsp()) {
    return -1;
  }

  const LoginRsp &rsp = push_msg->login_rsp();
  if (rsp.code() != 0) {
    Z_LOG_E("Failed to login to server, result is %d", rsp.code());
    return FAIL;
  }

  Z_LOG_D("Good, loged in success");

  setState(STATE_REGISTERED);

  // cancel timer
  timer_.cancelTimer(login_timer_id);
  login_timer_id = -1;

  return OK;
}

// XXX: continus package
int FGWClientHandler::onRead_WaitForServer(char *buf, uint32_t buf_len)
{
  Z_LOG_D("FGWClientHandler::onRead_WaitForServer()");

  PushMsg push_msg;

  int rv = protobuf_decode(&push_msg, buf, buf_len);
  Z_LOG_I("decoded(%d)", rv);
  if (rv < 0) {
    Z_LOG_E("Failed to decode push msg");
    return FAIL;
  }

  if (push_msg.type() != LOGIN_RSP) {
    Z_LOG_E("Unknown push msg type: %d", push_msg.type());
    return FAIL;
  }

  return processLoginRsp(&push_msg);
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

  // --------------------------------
  PushMsg push_msg;

  int rv = protobuf_decode(&push_msg, buf, buf_len);
  if (rv < 0) {
    Z_LOG_E("Failed to decode push msg");
    return FAIL;
  }

  Z_LOG_D("push msg type=%s received", push_msg.GetTypeName().c_str());

  ZInnerMsg *inner_msg = push2inner(push_msg);
  if (NULL == inner_msg) {
    Z_LOG_E("Failed to convert push to inner");
    return FAIL;
  }

  inner_msg->src_addr_.module_type_ = getModuleType();
  inner_msg->src_addr_.handler_id_ = getId();
  inner_msg->dst_addr_.module_type_ = MODULE_SERIAL;
  inner_msg->dst_addr_.handler_id_ = ANY_ID;

  ZDispatcher::instance()->sendDirect(inner_msg);

  return OK;

  // // decode
  // json_t* jmsg = decodeWebApiMsg(buf, buf_len);

  // // sequence
  // json_t *jseq = json_object_get(jmsg, "seq");
  // if (jseq == NULL || !json_is_integer(jseq)) {
  //  return -1;
  // }

  // ZInnerMsg *inner_msg = json2Inner(jmsg);
  // if (inner_msg == NULL) {
  //   sendRsp("bad request", 400);
  //   return -1;
  // }
  // inner_msg->seq_ = json_integer_value(jseq);

  // FGWClientSession *session = createSession(inner_msg);
  // if (NULL == session) {
  //   return FAIL;
  // }

  // session->event(inner_msg);
  // if (!session->isComplete()) {
  //   Z_LOG_D("Added: key1=%u, key2=%u", inner_msg->seq_, inner_msg->seq_);

  //   session_ctrl_.add(inner_msg->seq_, inner_msg->seq_, session);
  // } else {
  //   delete session;
  //   session = NULL;
  // }

  // return OK;
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

  PushMsg *push_msg = inner2push(*msg);

  int rv = protobuf_encode(push_msg, out_buf_, sizeof(out_buf_));
  if (rv < 0) {
    Z_LOG_E("Failed to encode push message");
    return FAIL;
  }

  send(out_buf_, rv);

  return OK;
}

void FGWClientHandler::routine(long delta)
{
  // --- FOR DEBUGGING ONLY ---
  return;
  // --- FOR DEBUGGING ONLY ---

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

