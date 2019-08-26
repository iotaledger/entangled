/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <unity/unity.h>

#include "common/trinary/ptrit.h"
#include "common/trinary/ptrit_incr.h"

#include <stdio.h>
void test_ptrit_offset(void) {
  ptrit_t p[PTRIT_SIZE_LOG3];

  ptrit_offset(p, PTRIT_SIZE_LOG3);
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_ptrit_offset);

  return UNITY_END();
}
