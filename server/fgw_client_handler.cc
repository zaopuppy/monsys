#include "fgw_client_handler.h"

#include <unistd.h>
#include <string.h>

#include "push_message.h"

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
	event_del(read_event_);
	read_event_ = NULL;
}

int FGWClientHandler::onRead(char *buf, uint32_t buf_len)
{
	Z_LOG_D("FGWClientHandler::onRead()\n");

	if (buf_len <= 0) { // MIN_MSG_LEN(header length)
		Z_LOG_D("empty message\n");

		sendRsp("empty message\n", 404);

		return -1;
	}

	trace_bin(buf, buf_len);

	{
		// ZInnerMsg *inner_msg = webMsg2InnerMsg(jobj);
		ZInnerMsg *inner_msg = decodePushMsg(buf, buf_len);
		if (inner_msg == NULL) {
			sendRsp("bad request\n", 400);
			return -1;
		}

		// set source address
		inner_msg->src_addr_ = addr_;

		// set destination address
		inner_msg->dst_addr_.module_type_ = Z_MODULE_SERIAL;
		inner_msg->dst_addr_.handler_id_ = ANY_ID;	// should have only one

		// ZDispatcher::instance()->sendMsg(inner_msg);
		ZDispatcher::instance()->sendDirect(inner_msg);
	}

	return 0;
}

int FGWClientHandler::onInnerMsg(ZInnerMsg *msg)
{
	Z_LOG_D("FGWClientHandler::onInnerMsg\n");

	json_t *jobj = NULL;

	switch (msg->msg_type_) {
		case Z_ZB_GET_DEV_LIST_RSP:
			{
				jobj = inner2Json((ZInnerGetDevListRsp*)msg);
				break;
			}
		case Z_ZB_GET_DEV_RSP:
			{
				jobj = inner2Json((ZInnerGetDevInfoRsp*)msg);
				break;
			}
		case Z_ZB_SET_DEV_RSP:
			{
				jobj = inner2Json((ZInnerSetDevInfoRsp*)msg);
				break;
			}
		default:
			Z_LOG_E("Unknown message type: %d\n", msg->msg_type_);
			return -1;
	}

	if (!jobj) {
		Z_LOG_D("Failed to convert ");
		return FAIL;
	}

	char *str_dump = json_dumps(jobj, 0);
	sendRsp(str_dump, 200);
	free(str_dump);
	json_decref(jobj);

	return OK;
}

void FGWClientHandler::routine(long delta)
{
}

int FGWClientHandler::send(const char *buf, uint32_t buf_len)
{
	Z_LOG_D("FGWClientHandler::send(%d)\n", fd_);
	return ::send(fd_, buf, buf_len, 0);
}

void FGWClientHandler::onConnected()
{
	Z_LOG_D("FGWClientHandler::onConnected()\n");
}

void FGWClientHandler::sendRsp(const char *text_msg, int status)
{
	Z_LOG_D("FGWClientHandler::sendRsp\n");
	int rv = send(text_msg, strlen(text_msg));
	Z_LOG_D("sent %d bytes\n", rv);
}


