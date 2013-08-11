#include "fgw_client_handler.h"

int FGWClientHandler::init()
{
	return OK;
}

void FGWClientHandler::close()
{
}

int FGWClientHandler::onRead(char *buf, uint32_t buf_len)
{
	return OK;
}

int FGWClientHandler::onInnerMsg(ZInnerMsg *msg)
{
	return OK;
}

void FGWClientHandler::routine(long delta)
{
}

int FGWClientHandler::send(const char *buf, uint32_t buf_len)
{
	return OK;
}

void FGWClientHandler::onConnected()
{
}


