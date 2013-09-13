
#include "libframework/zframework.h"

#include <string.h>

#include "gtest/gtest.h"

TEST(ZDataBufferTest, TestCase01)
{
  int rv;
  ZDataBuffer<char> buffer(10);

  EXPECT_EQ(0, buffer.pos());
  EXPECT_EQ(10, buffer.limit());
  EXPECT_EQ(buffer.limit(), buffer.capability());

  buffer.put(0x13);

  EXPECT_EQ(1, buffer.pos());
  EXPECT_EQ(10, buffer.limit());
  EXPECT_EQ(buffer.limit(), buffer.capability());

  rv = buffer.put("1234", 4);

  EXPECT_EQ(4, rv);
  EXPECT_EQ(5, buffer.pos());
  EXPECT_EQ(10, buffer.limit());
  EXPECT_EQ(buffer.limit(), buffer.capability());

  rv = buffer.put("very long string........", 10);

  EXPECT_EQ(10 - 1- 4, rv);
  EXPECT_EQ(10, buffer.pos());
  EXPECT_EQ(10, buffer.limit());
  EXPECT_EQ(buffer.limit(), buffer.capability());

  buffer.clear();
  EXPECT_EQ(0, buffer.pos());
  EXPECT_EQ(10, buffer.limit());
  EXPECT_EQ(buffer.limit(), buffer.capability());
}

TEST(ZDataBufferTest, TestCase02)
{
  char str[32] = "hello, girl";
  int rv, str_len;

  str_len = (int)strlen(str);
  ZDataBuffer<char> buffer(str, str_len);

  EXPECT_EQ(0, buffer.pos());
  EXPECT_EQ(str_len, buffer.limit());
  EXPECT_EQ(buffer.limit(), buffer.capability());
  
  char buf[32];

  rv = buffer.get(buf, sizeof(buf));

  EXPECT_EQ(str_len, rv);
  EXPECT_EQ(str_len, buffer.pos());
  EXPECT_EQ(buffer.limit(), buffer.pos());
  EXPECT_EQ(buffer.limit(), buffer.capability());

  rv = memcmp(buf, str, str_len);
  EXPECT_EQ(0, rv);
}

TEST(ZDataBufferTest, TestCase03)
{
  int rv;
  ZDataBuffer<char> buffer(10);

  char c = 0x44;

  buffer.put(c);

  EXPECT_EQ(1, buffer.pos());
  EXPECT_EQ(10, buffer.limit());
  EXPECT_EQ(buffer.capability(), buffer.limit());

  buffer.flip();

  EXPECT_EQ(0, buffer.pos());
  EXPECT_EQ(1, buffer.limit());
  EXPECT_EQ(10, buffer.capability());

  buffer.clear();

  EXPECT_EQ(0, buffer.pos());
  EXPECT_EQ(10, buffer.limit());
  EXPECT_EQ(buffer.capability(), buffer.limit());

  const char *str = "one little two little three little endian";
  int str_len = (int)strlen(str);

  rv = buffer.put(str, str_len);

  EXPECT_EQ(10, rv);
  EXPECT_EQ(10, buffer.pos());
  EXPECT_EQ(10, buffer.limit());
  EXPECT_EQ(10, buffer.capability());

  buffer.flip();

  char buf[16];

  rv = buffer.get(buf, sizeof(buf));

  EXPECT_EQ(10, rv);
  EXPECT_EQ(10, buffer.pos());
  EXPECT_EQ(10, buffer.limit());

  rv = memcmp(buf, str, rv);
  EXPECT_EQ(0, rv);
}


