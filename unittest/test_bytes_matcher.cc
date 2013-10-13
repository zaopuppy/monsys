
#include <stdio.h>

#include "server/syn_byte_matcher.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

TEST(SynBytesMatcherTest, TestCase01)
{
  const char *syn_bytes = "ZZZZZZZZ"; // 8 bytes
  const uint32_t syn_bytes_len = strlen(syn_bytes);
  SynBytesMatcher matcher(syn_bytes, syn_bytes_len);

  ASSERT_FALSE(matcher.finished());
  ASSERT_EQ(syn_bytes_len, matcher.remaining());
  ASSERT_EQ(0, matcher.feeded());

  uint32_t rv;

  rv = matcher.feed("ZZ", 2);

  ASSERT_FALSE(matcher.finished());
  ASSERT_EQ(syn_bytes_len - 2, matcher.remaining());
  ASSERT_EQ(2, matcher.feeded());
  ASSERT_EQ(2, rv);

  rv = matcher.feed("ZZZZZZ", 6);
  ASSERT_TRUE(matcher.finished());
  ASSERT_EQ(0, matcher.remaining());
  ASSERT_EQ(syn_bytes_len, matcher.feeded());
  ASSERT_EQ(6, rv);

  rv = matcher.feed("asdfasdf", 6);
  ASSERT_TRUE(matcher.finished());
  ASSERT_EQ(0, matcher.remaining());
  ASSERT_EQ(syn_bytes_len, matcher.feeded());
  ASSERT_EQ(0, rv);

}

TEST(SynBytesMatcherTest, TestCase02)
{
  const char *syn_bytes = "ZZZZZZZZ"; // 8 bytes
  const uint32_t syn_bytes_len = strlen(syn_bytes);
  SynBytesMatcher matcher(syn_bytes, syn_bytes_len);

  ASSERT_FALSE(matcher.finished());
  ASSERT_EQ(syn_bytes_len, matcher.remaining());

  int rv;

  rv = matcher.feed("ZZ", 2);

  ASSERT_FALSE(matcher.finished());
  ASSERT_EQ(syn_bytes_len - 2, matcher.remaining());
  ASSERT_EQ(2, rv);

  rv = matcher.feed("ZZZ1ZZ", 6);
  ASSERT_FALSE(matcher.finished());
  ASSERT_EQ(syn_bytes_len - 2, matcher.remaining());
  ASSERT_EQ(-1, rv);

  matcher.reset();
  ASSERT_FALSE(matcher.finished());
  ASSERT_EQ(syn_bytes_len, matcher.remaining());

  rv = matcher.feed("ZZZZZZZZ12345678", 16);
  ASSERT_TRUE(matcher.finished());
  ASSERT_EQ(0, matcher.remaining());
  ASSERT_EQ(8, rv);  

}
