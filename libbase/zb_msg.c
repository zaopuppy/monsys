#include "zb_msg.h"

#include "z_codec.h"

int
zb_encode_header(struct zb_header *val, char *buf, uint32_t buf_len)
{
	uint32_t orig_len = buf_len;
	int rv;

	/* fixed values */
	val->syn = 0xFF;

	ENCODE_WITH_METHOD(z_encode_byte, val->syn, buf, buf_len, rv);
	ENCODE_WITH_METHOD(z_encode_integer16, val->len, buf, buf_len, rv);
	ENCODE_WITH_METHOD(z_encode_byte, val->cmd, buf, buf_len, rv);

	return orig_len - buf_len;
}

int
zb_decode_header(struct zb_header *val, char *buf, uint32_t buf_len)
{
	uint32_t orig_len = buf_len;
	int rv;

	/* fixed values */
	DECODE_WITH_METHOD(z_decode_byte, (char*)&(val->syn), buf, buf_len, rv);
	if (val->syn != 0xFF) {
		return -1;
	}

	DECODE_WITH_METHOD(z_decode_integer16, &val->len, buf, buf_len, rv);
	DECODE_WITH_METHOD(z_decode_byte, (char*)&val->cmd, buf, buf_len, rv);

	return orig_len - buf_len;
}

uint32_t
zb_getlen_header(struct zb_header *val)
{
	uint32_t len = 0;
	
	len += z_getlen_byte();
	len += z_getlen_integer16();
	len += z_getlen_byte();

	return len;
}



