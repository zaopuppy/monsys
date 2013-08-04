#include "fgw_handler.h"

#include <unistd.h>

#include "fgw_server.h"

int FGWHandler::init()
{
	return OK;
}

// TODO: move to upper class
void FGWHandler::close()
{
	super_::close();
	server_->removeHandler(this);
}

int FGWHandler::onRead(char *buf, uint32_t buf_len)
{
	Z_LOG_D("FGWHandler::onRead()\n");
	trace_bin(buf, buf_len);
	return OK;
}

int FGWHandler::onInnerMsg(ZInnerMsg *msg)
{
	return OK;
}

void FGWHandler::routine(long delta)
{
}

int FGWHandler::send(const char *buf, uint32_t buf_len)
{
	return OK;
}
