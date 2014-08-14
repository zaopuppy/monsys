#include "protobuf_helper.h"

int protobuf_encode(::google::protobuf::Message *msg, char *buf, int buf_len)
{
  const int body_len = msg->ByteSize();
  const int head_len =
    ::google::protobuf::io::CodedOutputStream::VarintSize32(body_len);
  const int encode_len = head_len + body_len;

  if (buf_len < encode_len) {
    return -1;
  }

  ::google::protobuf::io::CodedOutputStream::WriteVarint32ToArray(body_len, (unsigned char*)buf);

  if (!msg->SerializeToArray(buf + head_len, body_len)) {
    return -1;
  }

  return encode_len;
}

int protobuf_decode(::google::protobuf::Message *msg, const char *buf, int buf_len)
{
  ::google::protobuf::io::CodedInputStream in((const unsigned char*)buf, buf_len);

  uint32_t tmp;
  if (!in.ReadVarint32(&tmp)) {
    return -1;
  }

  int head_len = in.CurrentPosition();
  int body_len = (int)tmp;

  if (!msg->ParseFromArray(buf + head_len, buf_len - head_len)) {
    return -1;
  }

  return head_len + body_len;
}
