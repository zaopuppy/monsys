#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

/* for htons/htonl/ntohs/ntohl */
#include <arpa/inet.h>

#include "zlog.h"
#include "zmessage_codec.h"

///////////////////////////////////////

int
test_compare_header(const struct z_header *hdr1,
		    const struct z_header *hdr2)
{
  assert(hdr1->syn[0] == hdr2->syn[0]);
  assert(hdr1->syn[1] == hdr2->syn[1]);
  assert(hdr1->len == hdr2->len);
  assert(hdr1->cmd == hdr2->cmd);
  assert(hdr1->seq == hdr2->seq);
  return 0;
}

int
test_compare_dev_info(const struct z_dev_info *info1,
		      const struct z_dev_info *info2)
{
  assert(info1->id == info2->id);
  assert(info1->stat == info2->stat);
  assert(0 == (strncmp(info1->desc, info2->desc, strlen(info1->desc))));
  return 0;
}

int
test_compare_dev_info_list(const struct z_dev_info_list* list1,
			   const struct z_dev_info_list* list2)
{
  int i;
  assert(list1->count == list2->count);
  for (i = 0; i < list1->count; ++i) {
    assert(0 == test_compare_dev_info(&(list1->infos[i]), &(list2->infos[i])));
  }
  return 0;
}

int test_query_req()
{
  printf("-------------------------------------------\n");
  printf("test_query_req()\n");
  
  struct z_query_dev_req msg1, msg2;
  int rv1, rv2;
  char buf1[512]; // , buf2[512];

  // encode
  {
    printf("encoding...\n");
    msg1.hdr.len = 0x1234;
    msg1.hdr.cmd = 0x3456;
    msg1.hdr.seq = 0x7890;
    msg1.uid = "User ID";
    msg1.dev_id = 0x4444;
    
    rv1 = z_encode_query_dev_req(&msg1, buf1, sizeof(buf1));
    if (rv1 < 0) {
      return rv1;
    }

    trace_bin(buf1, rv1);
  }

  // decode
  {
    printf("decoding...\n");
    rv2 = z_decode_query_dev_req(&msg2, buf1, rv1);
    if (rv2 < 0 || rv1 != rv2) {
      printf("failed to decode z_query_dev_req: [%d, %d]\n", rv1, rv2);
      return -1;
    }
  }

  // compare
  {
    printf("comparing...\n");
    
    // header
    assert(0 == test_compare_header(&msg1.hdr, &msg2.hdr));

    assert(0 == strncmp(msg1.uid, msg2.uid, strlen(msg1.uid)));
    assert(msg1.dev_id == msg2.dev_id);
  }

  printf("Great, works well.\n");

  return 0;
}

int test_query_rsp()
{
  printf("-------------------------------------------\n");
  printf("test_query_rsp()\n");
  
  struct z_query_dev_rsp msg1, msg2;
  char buf1[512];
  int rv1, rv2;

  // encode
  {
    printf("encoding...\n");
    
    msg1.hdr.len = 0x1111;
    msg1.hdr.cmd = 0x2222;
    msg1.hdr.seq = 0x3333;
  
    msg1.code = 0x4444;
    msg1.reason = "Greate.";

    msg1.info_list.count = 2;
    msg1.info_list.infos = malloc(sizeof(struct z_dev_info) * 2);
    // 0
    msg1.info_list.infos[0].id = 0x5555;
    msg1.info_list.infos[0].stat = 0x6666;
    msg1.info_list.infos[0].desc = "1st";
    // 1
    msg1.info_list.infos[1].id = 0x5556;
    msg1.info_list.infos[1].stat = 0x6667;
    msg1.info_list.infos[1].desc = "2nd";

    rv1 = z_encode_query_dev_rsp(&msg1, buf1, sizeof(buf1));
    if (rv1 < 0) {
      return rv1;
    }

    trace_bin(buf1, rv1);
  }

  // decode
  {
    printf("decoding...\n");
    rv2 = z_decode_query_dev_rsp(&msg2, buf1, rv1);
    if (rv2 < 0 || rv1 != rv2) {
      printf("failed to decode z_query_dev_rsp: [%d, %d]\n", rv1, rv2);
      return -1;
    }
  }

  // compare
  {
    printf("comparing...\n");
    // header
    test_compare_header(&msg1.hdr, &msg2.hdr);

    assert(msg1.code == msg2.code);
    assert(0 == strncmp(msg1.reason, msg2.reason, strlen(msg1.reason)));

    assert(0 == test_compare_dev_info_list(&msg1.info_list, &msg2.info_list));
  }

  printf("Great, works well.\n");

  return 0;
}

int main(int argc, char *argv[])
{
  printf("Just a test.\n");

  if (test_query_req() < 0) {
    printf("failed for test_query_req()\n");
    return -1;
  }
  
  if (test_query_rsp() < 0) {
    printf("failed for test_query_rsp()\n");
    return -1;
  }
  
  return 0;
}

