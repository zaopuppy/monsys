#include <stdio.h>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

extern void test_case01();
extern void test_case02();
extern void test_case03();

int main(int argc, char const *argv[])
{
  printf("Starting unit test...\n");

  test_case01();

  printf("End of unit test.\n");
  return 0;
}

