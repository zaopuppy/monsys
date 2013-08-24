#include "zwebapi_handler.h"

#include <unistd.h>
#include <assert.h>
#include <errno.h>

#include "libframework/zframework.h"
#include "libzigbee/zzigbee_message.h"
#include "libbase/zlog.h"

#include "module.h"

#include "push_message.h"

//////////////////////////////////////////////////////
int ZWebApiHandler::init() {
	printf("[%p] Oops, client's coming\n", this);

	// init address here
	addr_.module_type_ = getModuleType();
	addr_.handler_id_ = getId();

	return 0;
}

void ZWebApiHandler::close() {
	::close(fd_);
	fd_ = -1;
	event_free(read_event_);
	read_event_ = NULL;
	// XXX: don't forget to delete ourself
	// add to a free list, do it in routine
}

int ZWebApiHandler::onInnerMsg(ZInnerMsg *msg)
{
	printf("ZWebApiHandler::onInnerMsg()\n");

	switch (msg->msg_type_) {
		case Z_TRANSPORT_MSG:
			{
				ZTransportMsg *m = (ZTransportMsg*)msg;
				send(m->data_, m->data_len_);
				break;
			}
		default:
			Z_LOG_E("Unknow message type: %d\n", msg->msg_type_);
			return -1;
	}

	return OK;
}

void ZWebApiHandler::routine(long delta)
{
}


void ZWebApiHandler::sendRsp(const char *text_msg, int status)
{
	printf("ZWebApiHandler::sendRsp\n");
	int rv = send(text_msg, strlen(text_msg));
	printf("sent %d bytes\n", rv);
}

int ZWebApiHandler::onRead(char *buf, uint32_t buf_len)
{
	printf("ZWebApiModule::onRead()\n");

	if (buf_len <= 0) { // MIN_MSG_LEN(header length)
		printf("empty message\n");

		sendRsp("empty message\n", 404);

		return -1;
	}

	trace_bin(buf, buf_len);

	{
		ZTransportMsg *inner_msg = new ZTransportMsg();
		inner_msg->data_ = new char[buf_len];
		inner_msg->data_len_ = buf_len;
		memcpy(inner_msg->data_, buf, buf_len);

		// set destination address
		inner_msg->dst_addr_.module_type_ = MODULE_FGW_SERVER;
		inner_msg->dst_addr_.handler_id_ = ANY_ID;	// should have only one

		ZDispatcher::instance()->sendDirect(inner_msg);

	}
	// {
	// 	// ZInnerMsg *inner_msg = webMsg2InnerMsg(jobj);
	// 	ZInnerMsg *inner_msg = decodePushMsg(buf, buf_len);
	// 	if (inner_msg == NULL) {
	// 		sendRsp("bad request\n", 400);
	// 		return -1;
	// 	}

	// 	// set source address
	// 	inner_msg->src_addr_ = addr_;

	// 	// set destination address
	// 	inner_msg->dst_addr_.module_type_ = MODULE_FGW_SERVER;
	// 	inner_msg->dst_addr_.handler_id_ = ANY_ID;	// should have only one

	// 	// ZDispatcher::instance()->sendMsg(inner_msg);
	// 	ZDispatcher::instance()->sendDirect(inner_msg);
	// }

	return 0;
}


