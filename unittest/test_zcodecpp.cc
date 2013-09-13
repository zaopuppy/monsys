
#include <stdio.h>

#include "libbase/zcodecpp.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

TEST(ZCodeCPPTest, TestCase01)
{
  fixed_binary_t bin, bin1;
  bin.len = 10;
  bin.data = new char[bin.len];
  bin1.len = 10;
  bin1.data = new char[bin1.len];

  snprintf(bin.data, 10, "helloadasdfasdfa");

  char buf[32];

  int rv;

  EXPECT_EQ(10, getlen(bin));

  rv = encode(bin, buf, sizeof(buf));

  EXPECT_EQ(10, rv);

  rv = decode(bin1, buf, rv);

  EXPECT_EQ(10, rv);

  EXPECT_EQ(bin.len, bin1.len);

  rv = memcmp(bin.data, bin1.data, bin.len);

  EXPECT_EQ(0, rv);
}

