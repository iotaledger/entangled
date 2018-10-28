/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <unity/unity.h>

#include "utils/handles/cas.h"

void test_cas_pointers_simple() {
  int* p;
  int q;

  TEST_ASSERT(p != &q);
  TEST_ASSERT(compare_and_swap(&p, p, &q));
  TEST_ASSERT(p == &q);
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_cas_pointers_simple);

  return UNITY_END();
}
