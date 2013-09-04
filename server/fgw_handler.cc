#include "fgw_handler.h"

#include <unistd.h>

#include <jansson.h>

#include "zinner_message_ex.h"
#include "fgw_server.h"
#include "webapi_msg.h"

int FGWHandler::init()
{
	// addr_.module_type_ = getModuleType();
	// addr_.handler_id_ = getId();
	Z_LOG_D("FGWHandler::init()");
	Z_LOG_D("fd: %d", getFd());
	return OK;
}

// TODO: move to upper class
void FGWHandler::close()
{
	super_::close();
	// XXX
	// server_->removeHandler(this);
	// XXX: remove it to upper class
	((FGWServer*)getModule())->removeHandler(this);
}

int FGWHandler::onRead(char *buf, uint32_t buf_len)
{
	Z_LOG_D("FGWHandler::onRead()");
	trace_bin(buf, buf_len);

	// decode first
	json_error_t jerror;
	json_t *jmsg = json_loadb(buf, buf_len, 0, &jerror);
	if (jmsg == NULL || !json_is_object(jmsg)) {
		return FAIL;
	}

	// get seq field
	json_t *jseq = json_object_get(jmsg, "seq");
	if (jseq == NULL || !json_is_integer(jseq)) {
		Z_LOG_E("Field 'field' doesn't exist, ignore");
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
	ZTransportMsg *m = new ZTransportMsg();
	m->set(buf, buf_len);
	m->dst_addr_ = session->src_addr_;

	ZDispatcher::instance()->sendDirect(m);

	// remove session
	session_ctrl_.removeByKey1(session->getKey());
	delete session;

	return OK;
}

bool FGWHandler::checkSessionBySequence(uint32_t sequence)
{
	FGWSession *session = (FGWSession*)session_ctrl_.findByKey1(sequence);
	if (session) {
		return true;
	}

	return false;
}

int FGWHandler::onInnerMsg(ZInnerMsg *msg)
{
	Z_LOG_D("FGWHandler::onInnerMsg");
	if (msg->msg_type_ != Z_TRANSPORT_MSG) {
		Z_LOG_E("Unknow message type: %d", msg->msg_type_);
		return FAIL;
	}

	if (checkSessionBySequence(msg->seq_)) {
		Z_LOG_D("Duplicated session: [%u]", msg->seq_);
		return FAIL;
	}

	ZTransportMsg *m = (ZTransportMsg*)msg;

	// send(m->data_, m->data_len_);
	json_error_t jerror;
	json_t *jmsg = json_loadb(m->data_, m->data_len_, 0, &jerror);
	if (jmsg == NULL || !json_is_object(jmsg)) {
		Z_LOG_E("Bad request");
		// Should return fail message immediately
		if (jmsg != NULL) {
			json_decref(jmsg);
		}
		return FAIL;
	}

	// add "seq" field
	json_t *jseq = json_integer(msg->seq_);
	int rv = json_object_set_new(jmsg, "seq", jseq);
	if (rv != 0) {
		json_decref(jmsg);
		return FAIL;
	}

	char *str_dump = json_dumps(jmsg, 0);

	send(str_dump, strlen(str_dump));

	Z_LOG_D("Message sent");
	trace_bin(str_dump, strlen(str_dump));

	free(str_dump);
	json_decref(jmsg);

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

void FGWHandler::routine(long delta)
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

int FGWHandler::send(const char *buf, uint32_t buf_len)
{
	Z_LOG_D("FGWHandler::send(%d)", getFd());
	int rv = ::send(getFd(), buf, buf_len, 0);
	Z_LOG_D("rv: %d", rv);
	return OK;
}
