#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "libbase/zlog.h"

#include "push_msg.h"
#include "push_codec.h"

TEST(TestPushMsg, TestCase01)
{
  push::Heartbeat msg1, msg2;
  int rv1, rv2;

  z::ZByteBuffer buf(1024);

  msg1.header_.seq = 0x44;
  msg1.header_.type = 8;

  // encode it first;
  rv1 = msg1.encode(buf);
  ASSERT_GT(rv1, 0);
  ASSERT_EQ(rv1, buf.pos());

  buf.flip();

  // decode
  rv2 = msg2.decode(buf);
  ASSERT_EQ(rv2, rv1);

  // header
  ASSERT_EQ(msg1.header_.seq, msg2.header_.seq);
  ASSERT_EQ(msg1.header_.type, msg2.header_.type);
}

TEST(TestPushMsg, TestCase02)
{
  push::VData data;

  data.putByte(5);
  ASSERT_EQ(5, data.getByte());
}

TEST(TestPushMsg, TestCase03)
{
  // int16
  {
    push::TLV tlv1, tlv2;
    z::ZByteBuffer buf(1024);
    int rv1, rv2;

    tlv1.getValue().putInt16(0x87);

    rv1 = push::encode(tlv1, buf);
    buf.flip();
    trace_bin(buf.getArray(), buf.limit());
    ASSERT_GT(rv1, 0);


    rv2 = push::decode(tlv2, buf);
    ASSERT_EQ(rv1, rv2);

    ASSERT_EQ(tlv1.getTag(), tlv2.getTag());
    ASSERT_EQ(tlv1.getValue().getType(), tlv2.getValue().getType());

    ASSERT_EQ(tlv1.getValue().getDataLen(), tlv2.getValue().getDataLen());
    int rv = memcmp(
      tlv1.getValue().getData(),
      tlv2.getValue().getData(),
      tlv1.getValue().getDataLen());
    ASSERT_EQ(rv, 0);
  }

  // int16
  {
    push::TLV tlv1, tlv2;
    z::ZByteBuffer buf(1024);
    int rv1, rv2;

    tlv1.getValue().putStr("he\0\0llo", sizeof("he\0\0llo") - 1);

    rv1 = push::encode(tlv1, buf);
    buf.flip();
    trace_bin(buf.getArray(), buf.limit());
    ASSERT_GT(rv1, 0);


    rv2 = push::decode(tlv2, buf);
    ASSERT_EQ(rv1, rv2);

    ASSERT_EQ(tlv1.getTag(), tlv2.getTag());
    ASSERT_EQ(tlv1.getValue().getType(), tlv2.getValue().getType());

    ASSERT_EQ(tlv1.getValue().getDataLen(), tlv2.getValue().getDataLen());
    int rv = memcmp(
      tlv1.getValue().getData(),
      tlv2.getValue().getData(),
      tlv1.getValue().getDataLen());
    ASSERT_EQ(rv, 0);
  }
}

