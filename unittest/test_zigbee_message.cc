
#include "gtest/gtest.h"

#include "zzigbee_message.h"


TEST(ZigBeeMessageTest, TestCase01)
{
  // ZigBeeMsg
  ZZigBeeMsg msg, msg1;
  char buf[128];
  int rv;

  rv = msg.encode(buf, sizeof(buf));

  EXPECT_GT(rv, 0);

  rv = msg1.decode(buf, rv);

  EXPECT_GT(rv, 0);

}


