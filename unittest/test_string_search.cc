#include "gtest/gtest.h"


TEST(StringSearchTest, SimpleStringSearch_TestCase01)
{
  {
    const char *target = "12345678";
    const char *str = "1234";

    ASSERT_EQ(0, simple_string_search(target, strlen(target), str, strlen(str)));
  }

  {
    const char *target = "12345678";
    const char *str = "456789";

    ASSERT_EQ(-1, simple_string_search(target, strlen(target), str, strlen(str)));
  }

  {
    const char *target = "12345678";
    const char *str = "4567";

    ASSERT_EQ(3, simple_string_search(target, strlen(target), str, strlen(str)));
  }

}

