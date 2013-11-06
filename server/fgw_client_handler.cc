#include "fgw_client_handler.h"

#include <unistd.h>
#include <string.h>

#include "webapi_msg.h"

int FGWClientHandler::init()
{
	addr_.module_type_ = getModuleType();
	addr_.handler_id_ = getId();

	return OK;
}

void FGWClientHandler::close()
{
	::close(fd_);
	fd_ = -1;
	event_free(read_event_);
	read_event_ = NULL;
}

int FGWClientHandler::onRead(char *buf, uint32_t buf_len)
{
	Z_LOG_D("FGWClientHandler::onRead()");

	if (buf_len <= 0) { // MIN_MSG_LEN(header length)
		Z_LOG_D("empty message");

		sendRsp("empty message", 404);

		return -1;
	}

	Z_LOG_D("Received message from FGW server");
	trace_bin(buf, buf_len);

	// decode
	json_t* jmsg = decodeWebApiMsg(buf, buf_len);
	// check sequence
	json_t *jseq = json_object_get(jmsg, "seq");
	if (jseq == NULL || !json_is_integer(jseq)) {
		Z_LOG_E("Field seq is not in request message, ignore");
		return FAIL;
	}
	uint32_t seq = (uint32_t)json_integer_value(jseq);
	Z_LOG_D("push msg sequence id: %u", seq);

	// check duplicated session
	FGWClientSession *session =	(FGWClientSession*)session_ctrl_.findByKey2(seq);
	if (session != NULL) {
		Z_LOG_E("Duplicated session: [%u]", seq);
		return FAIL;
	}

	// everything is OK, now doing the convert
	ZInnerMsg *inner_msg = json2Inner(jmsg);
	if (inner_msg == NULL) {
		sendRsp("bad request", 400);
		return -1;
	}

	// set source address
	inner_msg->src_addr_ = addr_;

	// set destination address
	inner_msg->dst_addr_.module_type_ = MODULE_SERIAL;
	inner_msg->dst_addr_.handler_id_ = ANY_ID;	// should have only one

	// save session
	{
		FGWClientSession *session = new FGWClientSession();
		session->setKey(inner_msg->seq_);
		session->webapi_seq = seq;

		session_ctrl_.add(inner_msg->seq_, seq, session);
 		Z_LOG_D("Added: key1=%u, key2=%u", inner_msg->seq_, seq);
	}

	ZDispatcher::instance()->sendDirect(inner_msg);

	return 0;
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

	json_t *jmsg = NULL;

	switch (msg->msg_type_) {
		case Z_ZB_GET_DEV_LIST_RSP:
			{
				jmsg = inner2Json((ZInnerGetDevListRsp*)msg);
				break;
			}
		case Z_ZB_GET_DEV_RSP:
			{
				jmsg = inner2Json((ZInnerGetDevInfoRsp*)msg);
				break;
			}
		case Z_ZB_SET_DEV_RSP:
			{
				jmsg = inner2Json((ZInnerSetDevInfoRsp*)msg);
				break;
			}
		default:
			Z_LOG_E("Unknown message type: %d", msg->msg_type_);
			return -1;
	}

	if (!jmsg) {
		Z_LOG_D("Failed to convert ");
		return FAIL;
	}

	// add "seq" field from saved seq
	uint32_t seq = session->webapi_seq;
	json_t *jseq = json_integer(seq);
	int rv = json_object_set_new(jmsg, "seq", jseq);
	if (rv != 0) {
		json_decref(jmsg);
		return FAIL;
	}

	char *str_dump = json_dumps(jmsg, 0);
	sendRsp(str_dump, 200);
	trace_bin(str_dump, strlen(str_dump));

	free(str_dump);
	json_decref(jmsg);

	// delete session
	session_ctrl_.removeByKey1(session->getKey());
	delete session;

	return OK;
}

void FGWClientHandler::routine(long delta)
{
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

void FGWClientHandler::onConnected()
{
	Z_LOG_D("FGWClientHandler::onConnected()");
}

void FGWClientHandler::sendRsp(const char *text_msg, int status)
{
	Z_LOG_D("FGWClientHandler::sendRsp");
	int rv = send(text_msg, strlen(text_msg));
	Z_LOG_D("sent %d bytes", rv);
}


