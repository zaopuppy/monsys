#include "zserial.h"

#include "zerrno.h"

int Zserial::init()
{
  return OK;
}

void Zserial::close()
{
}

void Zserial::event(evutil_socket_t fd, short events)
{
}

void Zserial::doTimeout()
{
}

bool Zserial::isComplete()
{
}

int Zserial::onInnerMsg(ZInnerMsg *msg)
{
}


