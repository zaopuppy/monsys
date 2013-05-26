#include "zmon_message.h"

#define ZMON_MSG_ENCODE(_field) \
do { \
	rv = ::encode(_field, buf, buf_len); \
	if (rv < 0) { \
		return rv; \
	} \
	buf += rv; \
	buf_len -= rv; \
	encode_len += rv; \
} while (false)

#define ZMON_MSG_DECODE(_field) \
do { \
	rv = ::decode(_field, buf, buf_len); \
	if (rv < 0) { \
		return rv; \
	} \
	buf += rv; \
	buf_len -= rv; \
	decode_len += rv; \
} while (false)

int ZMonMsg::encode(char *buf, uint32_t buf_len)
{
	int rv;
	int encode_len;

	ZMON_MSG_ENCODE(len_);
	ZMON_MSG_ENCODE(cmd_);
	ZMON_MSG_ENCODE(seq_);

	return encode_len;
}

int ZMonMsg::decode(char *buf, uint32_t buf_len)
{
	int rv;
	int decode_len;

	ZMON_MSG_DECODE(len_);
	ZMON_MSG_DECODE(cmd_);
	ZMON_MSG_DECODE(seq_);

	return decode_len;
}


