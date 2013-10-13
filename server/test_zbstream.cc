#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include <string.h>
#include "server/zb_stream.h"

TEST(ZStreamTest, TestCase01)
{
  ZBStream stream;
  int rv;
  int tmp;

  char buf1[] = "1234567890ZZ";
  char buf2[] = "ZZZZZZ\001\000\004abc12";
  char buf3[] = "341234567890ZZZ1234";
  char buf[32];

  // buf1
  rv = stream.feed(buf1, sizeof(buf1) - 1);
  ASSERT_EQ(rv, sizeof(buf1) - 1);
  tmp = ZBStream::STATE_INIT;
  ASSERT_EQ(tmp, stream.getState());

  // read it
  rv = stream.read(buf, sizeof(buf));
  ASSERT_EQ(0, rv);

  // buf2
  rv = stream.feed(buf2, sizeof(buf2) - 1);
  tmp = ZBStream::STATE_WAITING_FOR_DATA;
  ASSERT_EQ(tmp, stream.getState());
  ASSERT_EQ(rv, sizeof(buf2) - 1);

  // read it
  rv = stream.read(buf, sizeof(buf));
  ASSERT_EQ(0, rv);

  // buf3
  rv = stream.feed(buf3, sizeof(buf3) - 1);
  ASSERT_EQ(rv, sizeof(buf3) - 1);
  tmp = ZBStream::STATE_INIT;
  ASSERT_EQ(tmp, stream.getState());

  // read it
  rv = stream.read(buf, sizeof(buf));
  ASSERT_EQ(10, rv);

  ASSERT_EQ(0, memcmp("\001\000\004abc1234", buf, rv));

}
