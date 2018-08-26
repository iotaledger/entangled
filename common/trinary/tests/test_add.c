/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdio.h>
#include <unity/unity.h>

#include "common/trinary/add.h"

void test_add(void) {
  trit_t h[3] = {-1, 0, 1};
  trit_t g[3] = {1, -1, 1};
  trit_t z[3] = {-1, 0, 0};

  add_trits(h, z, 3);
  TEST_ASSERT_EQUAL_MEMORY(g, z, sizeof(z));
  add_assign(z, 3, 1);
  TEST_ASSERT_EQUAL_MEMORY(h, z, sizeof(z));
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_add);

  return UNITY_END();
}
