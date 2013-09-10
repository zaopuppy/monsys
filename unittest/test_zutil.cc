
#include "libframework/zframework.h"

#include <string.h>
#include <assert.h>

void test_case01()
{
  int rv;
  ZDataBuffer<char> buffer(10);
  // printf("----------------\n");
  // printf("before\n");
  // printf("pos: %d\n", buffer.pos());
  // printf("limit: %d\n", buffer.limit());
  // printf("cap: %d\n", buffer.capability());

  assert(buffer.pos() == 0);
  assert(buffer.limit() == 10);
  assert(buffer.limit() == buffer.capability());

  buffer.put(0x13);

  assert(buffer.pos() == 1);
  assert(buffer.limit() == 10);
  assert(buffer.limit() == buffer.capability());

  rv = buffer.put("1234", 4);

  assert(rv == 4);
  assert(buffer.pos() == 5);
  assert(buffer.limit() == 10);
  assert(buffer.limit() == buffer.capability());

  rv = buffer.put("very long string........", 10);

  assert(rv == (10 - 1 - 4));
  assert(buffer.pos() == 10);
  assert(buffer.limit() == 10);
  assert(buffer.limit() == buffer.capability());

  buffer.clear();
  assert(buffer.pos() == 0);
  assert(buffer.limit() == 10);
  assert(buffer.limit() == buffer.capability());
}

void test_case02()
{
  const char *str = "hello, girl";
  int rv, str_len;

  str_len = (int)strlen(str);
  ZDataBuffer<char> buffer(str, str_len);

  assert(buffer.pos() == 0);
  assert(buffer.limit() == str_len);
  assert(buffer.limit() == buffer.capability());
  
  char buf[32];

  rv = buffer.get(buf, sizeof(buf));

  assert(rv == str_len);
  assert(buffer.pos() == str_len);
  assert(buffer.limit() == buffer.pos());
  assert(buffer.limit() == buffer.capability());

  rv = memcmp(buf, str, str_len);
  assert(rv == 0);
}

void test_case03()
{
  int rv;
  ZDataBuffer<char> buffer(10);

  char c = 0x44;

  buffer.put(c);

  assert(buffer.pos() == 1);
  assert(buffer.limit() == 10);
  assert(buffer.capability() == buffer.limit());

  buffer.flip();

  assert(buffer.pos() == 0);
  assert(buffer.limit() == 1);
  assert(buffer.capability() == 10);

  buffer.clear();

  assert(buffer.pos() == 0);
  assert(buffer.limit() == 10);
  assert(buffer.capability() == buffer.limit());

  const char *str = "one little two little three little endian";
  int str_len = (int)strlen(str);

  rv = buffer.put(str, str_len);

  assert(rv == 10);
  assert(buffer.pos() == 10);
  assert(buffer.limit() == 10);
  assert(buffer.capability() == 10);

  buffer.flip();

  char buf[16];

  rv = buffer.get(buf, sizeof(buf));

  assert(rv == 10);
  assert(buffer.pos() == 10);
  assert(buffer.limit() == 10);

  rv = memcmp(buf, str, rv);
  assert(rv == 0);
}


