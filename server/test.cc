#include <stdio.h>

#include "push.pb.h"

using namespace com::letsmidi::monsys::protocol;

int main(int argc, char *argv[])
{
  PushMsg *msg = new PushMsg();
  msg->set_type(PushMsg::LOGIN);

  return 0;
}


