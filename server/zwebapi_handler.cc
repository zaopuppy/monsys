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

	switch (msg->msg_type_) {
		case Z_TRANSPORT_MSG:
			{
				Z_LOG_D("Z_TRANSPORT_MSG");
				ZTransportMsg *m = (ZTransportMsg*)msg;
				int rv = send(m->data_, m->data_len_);
				Z_LOG_D("Sent %d bytes", rv);
				break;
			}
		default:
			Z_LOG_E("Unknow message type: %d", msg->msg_type_);
			return -1;
	}

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
	// Z_LOG_D("ZWebApiModule::onRead(fd_=%d)", getFd());
	// if (buf_len <= 0) { // MIN_MSG_LEN(header length)
	// 	Z_LOG_D("empty message");

	// 	sendRsp("empty message", 404);

	// 	return -1;
	// }

	// trace_bin(buf, buf_len);

	// {
	// 	// ZInnerMsg *inner_msg = webMsg2InnerMsg(jobj);
	// 	ZInnerMsg *inner_msg = decodePushMsg(buf, buf_len);
	// 	if (inner_msg == NULL) {
	// 		sendRsp("bad request", 400);
	// 		return -1;
	// 	}

	// 	// set source address
	// 	inner_msg->src_addr_ = addr_;

	// 	// set destination address
	// 	inner_msg->dst_addr_.module_type_ = MODULE_FGW_SERVER;
	// 	inner_msg->dst_addr_.handler_id_ = ANY_ID;	// should have only one

	// 	ZDispatcher::instance()->sendDirect(inner_msg);
	// }

	// return 0;

	Z_LOG_D("ZWebApiModule::onRead(fd_=%d)", getFd());

	if (buf_len <= 0) { // MIN_MSG_LEN(header length)
		Z_LOG_D("empty message");

		sendRsp("empty message\n", 404);

		return -1;
	}

	trace_bin(buf, buf_len);

	{
		ZTransportMsg *inner_msg = new ZTransportMsg();
		inner_msg->data_ = new char[buf_len];
		inner_msg->data_len_ = buf_len;
		memcpy(inner_msg->data_, buf, buf_len);

		// set source and destination address
		inner_msg->src_addr_ = addr_;
		inner_msg->dst_addr_.module_type_ = MODULE_FGW_SERVER;
		inner_msg->dst_addr_.handler_id_ = ANY_ID;

		ZDispatcher::instance()->sendDirect(inner_msg);

	}

	return 0;
}


