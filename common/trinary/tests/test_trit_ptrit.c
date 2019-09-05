/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <unity/unity.h>

#include "common/trinary/trit_ptrit.h"

void test_trit_to_ptrit(void) {
  trit_t trits[] = {-1, 0, 1};
  trit_t trits2[] = {0, 1, -1};
  size_t i;

  ptrit_t ptrit[3];
  trit_t trit_o[3];

  trits_to_ptrits_fill(trits, ptrit, 3);
  for (i = 0; i < PTRIT_SIZE; ++i) {
    ptrits_to_trits(ptrit, trit_o, i, 3);
    TEST_ASSERT_EQUAL_MEMORY(trits, trit_o, sizeof(trits));
  }

  trits_to_ptrits_fill(trits2, ptrit, 3);
  for (i = 0; i < PTRIT_SIZE; i++) {
    trits_to_ptrits(trits, ptrit, i, 3);
    ptrits_to_trits(ptrit, trit_o, i, 3);

    TEST_ASSERT_EQUAL_MEMORY(trits, trit_o, sizeof(trits));
  }
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_trit_to_ptrit);

  return UNITY_END();
}
