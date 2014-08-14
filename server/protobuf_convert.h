#include "push.pb.h"

#include "zinner_message_ex.h"

using namespace com::letsmidi::monsys::protocol;

ZInnerMsg* push2inner(const PushMsg &push_msg);
ZInnerMsg* push2inner(const GetDevList &msg);
ZInnerMsg* push2inner(const GetDevInfo &msg);
ZInnerMsg* push2inner(const SetDevInfo &msg);

PushMsg* inner2push(const ZInnerMsg &inner_msg);
PushMsg* inner2push(const ZInnerGetDevListRsp &inner_msg);
PushMsg* inner2push(const ZInnerGetDevInfoRsp &inner_msg);
PushMsg* inner2push(const ZInnerSetDevInfoRsp &inner_msg);

