
#include "gtest/gtest.h"

#include "zzigbee_message.h"


TEST(ZigBeeMessageTest, TestCase01)
{
  // ZigBeeMsg
  ZZigBeeMsg msg, msg1;
  char buf[128];
  int rv;

  rv = msg.encode(buf, sizeof(buf));

  ASSERT_GT(rv, 0);

  rv = msg1.decode(buf, rv);

  ASSERT_GT(rv, 0);

}

TEST(ZigBeeMessageTest, TestCase02)
{
  ZZBBroadcastInd msg1, msg2;
  char buf[128];
  int rv1, rv2;

  msg1.what_ = 0x4478;

  rv1 = msg1.encode(buf, sizeof(buf));
  ASSERT_GE(rv1, 0);

  // XXX: bad, should complete trasnfer layer as soon as possible
  rv2 = msg2.decode(buf + 8, rv1 - 8);
  ASSERT_EQ(rv2, rv1 - 8);

  ASSERT_EQ(msg1.what_, msg2.what_);
}

