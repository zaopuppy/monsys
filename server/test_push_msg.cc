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

  data.putInt32(0x12345678);
  ASSERT_EQ(0x12345678, data.getInt32());
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

  // String
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

TEST(TestPushMsg, TestCase04)
{
  push::GetReq msg1, msg2;
  int rv1, rv2;

  {
    msg1.header_.seq = 0x00;
    msg1.id_list_.push_back(4);
    msg1.id_list_.push_back(7);
    msg1.id_list_.push_back(18);
    msg1.id_list_.push_back(44);
  }

  z::ZByteBuffer buf(1024);

  rv1 = msg1.encode(buf);
  buf.flip();
  trace_bin(buf.getArray(), buf.limit());
  ASSERT_GT(rv1, 0);

  rv2 = msg2.decode(buf);
  ASSERT_EQ(rv1, rv2);

  //
  ASSERT_EQ(msg1.header_.seq, msg2.header_.seq);
  ASSERT_EQ(msg1.header_.type, msg2.header_.type);

  ASSERT_EQ(msg1.id_list_.size(), msg2.id_list_.size());

  for (int i = 0; i < msg1.id_list_.size(); ++i) {
    ASSERT_EQ(msg1.id_list_[i], msg2.id_list_[i]);
  }
}

TEST(TestPushMsg, TestCase05)
{
  push::GetRsp msg1, msg2;
  int rv1, rv2;

  {
    msg1.header_.seq = 0x00;
    push::TLV *tlv;

    tlv = new push::TLV();
    tlv->setTag(0x01);
    tlv->getValue().putInt32(0x5678);
    msg1.value_list_.push_back(tlv);

    tlv = new push::TLV();
    tlv->setTag(0x02);
    tlv->getValue().putInt32(0xAAAA);
    msg1.value_list_.push_back(tlv);

    tlv = new push::TLV();
    tlv->setTag(0x03);
    tlv->getValue().putInt32(0xBBBB);
    msg1.value_list_.push_back(tlv);

    tlv = new push::TLV();
    tlv->setTag(0x04);
    tlv->getValue().putInt32(0xCCCC);
    msg1.value_list_.push_back(tlv);
  }

  z::ZByteBuffer buf(1024);

  rv1 = msg1.encode(buf);
  buf.flip();
  Z_LOG_E("GetRsp");
  trace_bin(buf.getArray(), buf.limit());
  ASSERT_GT(rv1, 0);

  rv2 = msg2.decode(buf);
  ASSERT_EQ(rv1, rv2);

  //
  ASSERT_EQ(msg1.header_.seq, msg2.header_.seq);
  ASSERT_EQ(msg1.header_.type, msg2.header_.type);

  ASSERT_EQ(msg1.value_list_.size(), msg2.value_list_.size());

  int rv;
  for (int i = 0; i < msg1.value_list_.size(); ++i) {
    ASSERT_EQ(msg1.value_list_[i]->getTag(),
      msg2.value_list_[i]->getTag());
    const push::VData &data1 = msg1.value_list_[i]->getValue();
    const push::VData &data2 = msg2.value_list_[i]->getValue();
    ASSERT_EQ(data1.getType(), data2.getType());
    ASSERT_EQ(data1.getDataLen(), data2.getDataLen());
    const char* ptr1 = data1.getData();
    const char* ptr2 = data2.getData();
    rv = memcmp(ptr1, ptr2, data1.getDataLen());
    ASSERT_EQ(rv, 0);
  }
}

TEST(TestPushMsg, TestCase06)
{
  push::SetReq msg1, msg2;
  int rv1, rv2;

  {
    msg1.header_.seq = 0x00;
    push::TLV *tlv;

    tlv = new push::TLV();
    tlv->setTag(0x01);
    tlv->getValue().putInt32(0x5678);
    msg1.value_list_.push_back(tlv);

    tlv = new push::TLV();
    tlv->setTag(0x02);
    tlv->getValue().putInt32(0xAAAA);
    msg1.value_list_.push_back(tlv);

    tlv = new push::TLV();
    tlv->setTag(0x03);
    tlv->getValue().putInt32(0xBBBB);
    msg1.value_list_.push_back(tlv);

    tlv = new push::TLV();
    tlv->setTag(0x04);
    tlv->getValue().putInt32(0xCCCC);
    msg1.value_list_.push_back(tlv);
  }

  z::ZByteBuffer buf(1024);

  rv1 = msg1.encode(buf);
  buf.flip();
  Z_LOG_E("GetRsp");
  trace_bin(buf.getArray(), buf.limit());
  ASSERT_GT(rv1, 0);

  rv2 = msg2.decode(buf);
  ASSERT_EQ(rv1, rv2);

  //
  ASSERT_EQ(msg1.header_.seq, msg2.header_.seq);
  ASSERT_EQ(msg1.header_.type, msg2.header_.type);

  ASSERT_EQ(msg1.value_list_.size(), msg2.value_list_.size());

  int rv;
  for (int i = 0; i < msg1.value_list_.size(); ++i) {
    ASSERT_EQ(msg1.value_list_[i]->getTag(),
      msg2.value_list_[i]->getTag());
    const push::VData &data1 = msg1.value_list_[i]->getValue();
    const push::VData &data2 = msg2.value_list_[i]->getValue();
    ASSERT_EQ(data1.getType(), data2.getType());
    ASSERT_EQ(data1.getDataLen(), data2.getDataLen());
    const char* ptr1 = data1.getData();
    const char* ptr2 = data2.getData();
    rv = memcmp(ptr1, ptr2, data1.getDataLen());
    ASSERT_EQ(rv, 0);
  }
}

TEST(TestPushMsg, TestCase07)
{
  push::SetRsp msg1, msg2;
  int rv1, rv2;

  {
    msg1.header_.seq = 0x47;
    msg1.header_.type = 0x89;
    msg1.status_ = 0xAB;
  }

  z::ZByteBuffer buf(1024);

  rv1 = msg1.encode(buf);
  buf.flip();
  Z_LOG_E("SetRsp");
  trace_bin(buf.getArray(), buf.limit());
  ASSERT_GT(rv1, 0);

  rv2 = msg2.decode(buf);
  ASSERT_EQ(rv1, rv2);

  //
  ASSERT_EQ(msg1.header_.seq, msg2.header_.seq);
  ASSERT_EQ(msg1.header_.type, msg2.header_.type);

  ASSERT_EQ(msg1.status_, msg2.status_);
}

TEST(TestPushMsg, TestCase08)
{
  push::GetDevListReq msg1, msg2;
  int rv1, rv2;

  {
    msg1.header_.seq = 0x47;
    msg1.header_.type = 0x89;
  }

  z::ZByteBuffer buf(1024);

  rv1 = msg1.encode(buf);
  buf.flip();
  Z_LOG_E("SetRsp");
  trace_bin(buf.getArray(), buf.limit());
  ASSERT_GT(rv1, 0);

  rv2 = msg2.decode(buf);
  ASSERT_EQ(rv1, rv2);

  //
  ASSERT_EQ(msg1.header_.seq, msg2.header_.seq);
  ASSERT_EQ(msg1.header_.type, msg2.header_.type);
}

TEST(TestPushMsg, TestCase09)
{
  push::GetDevListRsp msg1, msg2;
  int rv1, rv2;

  {
    msg1.header_.seq = 0x47;
    msg1.header_.type = 0x89;

    push::dev_info_t *dev_info = NULL;

    // 1
    dev_info = new push::dev_info_t;
    dev_info->addr = 0x41;
    dev_info->name = "41";
    dev_info->state = 0x31;
    dev_info->type = 0x01;
    msg1.info_list_.push_back(dev_info);

    // 2
    dev_info = new push::dev_info_t;
    dev_info->addr = 0x42;
    dev_info->name = "42";
    dev_info->state = 0x32;
    dev_info->type = 0x02;
    msg1.info_list_.push_back(dev_info);

    // 3
    dev_info = new push::dev_info_t;
    dev_info->addr = 0x43;
    dev_info->name = "43";
    dev_info->state = 0x33;
    dev_info->type = 0x03;
    msg1.info_list_.push_back(dev_info);
  }

  z::ZByteBuffer buf(1024);

  rv1 = msg1.encode(buf);
  buf.flip();
  Z_LOG_E("SetRsp");
  trace_bin(buf.getArray(), buf.limit());
  ASSERT_GT(rv1, 0);

  rv2 = msg2.decode(buf);
  ASSERT_EQ(rv1, rv2);

  //
  ASSERT_EQ(msg1.header_.seq, msg2.header_.seq);
  ASSERT_EQ(msg1.header_.type, msg2.header_.type);

  ASSERT_EQ(msg1.info_list_.size(), msg2.info_list_.size());

  uint32_t list_len = msg1.info_list_.size();
  push::dev_info_t *info1 = NULL;
  push::dev_info_t *info2 = NULL;
  for (uint32_t i = 0; i < list_len; ++i) {
    info1 = msg1.info_list_[i];
    info2 = msg1.info_list_[i];
    ASSERT_EQ(info1->addr, info2->addr);
    ASSERT_TRUE(info1->name == info2->name);
    ASSERT_EQ(info1->state, info2->state);
    ASSERT_EQ(info1->type, info2->type);
  }
}


