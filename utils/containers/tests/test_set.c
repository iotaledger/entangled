/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdbool.h>
#include <unity/unity.h>
#include "utils/containers/int_set.h"

void test_ints_set() {
  int_set_t set = NULL;
  for (int i = 0; i < 100; ++i) {
    int_set_add(&set, &i);
  }

  for (int i = 0; i < 100; ++i) {
    int_set_add(&set, &i);
  }

  TEST_ASSERT_EQUAL_INT(int_set_size(set), 100);

  for (int i = 0; i < 100; ++i) {
    TEST_ASSERT(int_set_contains(set, &i));
  }

  int_set_free(&set);
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_ints_set);

  return UNITY_END();
}
