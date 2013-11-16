#include <stdio.h>
#include <stdint.h>
#include <string.h>

namespace base64 {

void encode(const char *input, uint32_t input_len, char *buf, uint32_t buf_len)
{
  printf("input: [%s], length: [%u]\n", input, input_len);
  static const char *dic_str =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

  char b1;
  char b2;
  char b3;
  uint32_t idx = 0;
  uint32_t i = 0;
  if (input_len >= 3) {
    for (/* */; i < input_len - 2; i += 3) {
      b1 = input[i];
      b2 = input[i + 1];
      b3 = input[i + 2];
      buf[idx++] = dic_str[(b1 >> 2) & 0x3F];
      buf[idx++] = dic_str[((b2 >> 4) & 0x0F) | ((b1 & 0x03) << 4)];
      buf[idx++] = dic_str[((b2 & 0x0F) << 2) | ((b3 >> 6) & 0x03)];
      buf[idx++] = dic_str[b3 & 0x3F];
    }
  }

  // left part
  if ((input_len - i) == 1) {
    b1 = input[i++];
    buf[idx++] = dic_str[(b1 >> 2) & 0x3F];
    buf[idx++] = dic_str[(b1 & 0x03) << 4];
    buf[idx++] = '=';
    buf[idx++] = '=';
  } else if ((input_len - i) == 2) {
		b1 = input[i++];
		b2 = input[i++];
    buf[idx++] = dic_str[(b1 >> 2) & 0x3F];
    buf[idx++] = dic_str[((b2 >> 4) & 0x0F) | ((b1 & 0x03) << 4)];
    buf[idx++] = dic_str[(b2 & 0x0F) << 2];
    buf[idx++] = '=';
  }

  // XXX: useless
  buf[idx] = 0x00;
}

}

int main(int argc, char *argv[])
{
  char buf[1024 + 1];
  const char *str = argc >=2 ? argv[1] : "";
  base64::encode(str, strlen(str), buf, 1024);
  printf("result: [%s]\n", buf);
  return 0;
}


