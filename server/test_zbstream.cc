#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include <string.h>
#include "server/zb_stream.h"

TEST(ZStreamTest, TestCase01)
{
  ZBStream stream;
  int rv;

  char buf1[] = "1234567890ZZ";
  char buf2[] = "ZZZZZZ\001\000\004abc12";
  char buf3[] = "341234567890ZZZ1234";
  char buf[32];

  // buf1
  rv = stream.feed(buf1, sizeof(buf1) - 1);
  ASSERT_EQ(rv, sizeof(buf1) - 1);
  ASSERT_EQ(ZBStream::STATE_INIT, stream.getState());

  // read it
  rv = stream.read(buf, sizeof(buf));
  ASSERT_EQ(0, rv);

  // buf2
  rv = stream.feed(buf2, sizeof(buf2) - 1);
  ASSERT_EQ(ZBStream::STATE_WAITING_FOR_DATA, stream.getState());
  ASSERT_EQ(rv, sizeof(buf2) - 1);

  // read it
  rv = stream.read(buf, sizeof(buf));
  ASSERT_EQ(0, rv);

  // buf3
  rv = stream.feed(buf3, sizeof(buf3) - 1);
  ASSERT_EQ(rv, sizeof(buf3) - 1);
  ASSERT_EQ(ZBStream::STATE_INIT, stream.getState());

  // read it
  rv = stream.read(buf, sizeof(buf));
  ASSERT_EQ(10, rv);

  ASSERT_EQ(0, memcmp("\001\000\004abc1234", buf, rv));

}

TEST(ZStreamTest, TestCase02)
{
  // 000000  5A 5A 5A 5A 5A 5A 5A 5A 01                       ZZZZZZZZ.
  // 000000  00 01 83 03 0B 00                                ......
  // 2014-04-06_05:23:13|D|zb_stream.cc:170|decoded version: 0
  // 2014-04-06_05:23:13|D|zb_stream.cc:171|decoded message length: 387
  ZBStream stream;
  int rv;

  char buf1[] = "\x5A\x5A\x5A\x5A\x5A\x5A\x5A\x5A\x01";
  char buf2[] = "\x00\x01\x83\x03\x0B\x00";

  rv = stream.feed(buf1, sizeof(buf1) - 1);
  ASSERT_EQ(rv, sizeof(buf1) - 1);
  ASSERT_EQ(ZBStream::STATE_WAITING_FOR_HEAD, stream.getState());

  rv = stream.feed(buf2, sizeof(buf2) - 1);
  ASSERT_EQ(rv, sizeof(buf2) - 1);
  ASSERT_EQ(ZBStream::STATE_INIT, stream.getState());
}

