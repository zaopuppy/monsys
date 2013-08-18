#include "fgw_handler.h"

#include <unistd.h>

#include "fgw_server.h"
#include "zinner_message_ex.h"

int FGWHandler::init()
{
	// addr_.module_type_ = getModuleType();
	// addr_.handler_id_ = getId();
	Z_LOG_D("FGWHandler::init()\n");
	Z_LOG_D("fd: %d\n", fd_);
	return OK;
}

// TODO: move to upper class
void FGWHandler::close()
{
	super_::close();
	// XXX
	// server_->removeHandler(this);
	((FGWServer*)getModule())->removeHandler(this);
}

int FGWHandler::onRead(char *buf, uint32_t buf_len)
{
	Z_LOG_D("FGWHandler::onRead()\n");
	trace_bin(buf, buf_len);

	ZTransportMsg *m = new ZTransportMsg();
	m->set(buf, buf_len);
	m->dst_addr_.module_type_ = MODULE_WEBAPI;
	m->dst_addr_.module_id_ = ANY_ID;
	m->dst_addr_.handler_id_ = ANY_ID;

	ZDispatcher::instance()->sendDirect(m);

	return OK;
}

int FGWHandler::onInnerMsg(ZInnerMsg *msg)
{
	Z_LOG_D("FGWHandler::onInnerMsg\n");
	if (msg->msg_type_ != Z_TRANSPORT_MSG) {
		Z_LOG_E("Unknow message type: %d\n", msg->msg_type_);
		return FAIL;
	}

	ZTransportMsg *m = (ZTransportMsg*)msg;

	send(m->data_, m->data_len_);

	return OK;
}

void FGWHandler::routine(long delta)
{
}

int FGWHandler::send(const char *buf, uint32_t buf_len)
{
	Z_LOG_D("FGWHandler::send(%d)\n", fd_);
	int rv = ::send(fd_, buf, buf_len, 0);
	Z_LOG_D("rv: %d\n", rv);
	return OK;
}
