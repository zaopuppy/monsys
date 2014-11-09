#ifndef _PROTOCOL_HELPER_H__
#define _PROTOCOL_HELPER_H__ value

#include <google/protobuf/message.h>
#include <google/protobuf/io/coded_stream.h>

int protobuf_encode(::google::protobuf::Message *msg, char *buf, int buf_len);

int protobuf_decode(::google::protobuf::Message *msg, const char *buf, int buf_len);


#endif

