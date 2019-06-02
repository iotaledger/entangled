/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <unity/unity.h>

#include "common/trinary/trit_ptrit.h"

#define TRITS_IN -1, 0, 1
#define ptrit_EXP \
  {HIGH_BITS, LOW_BITS}, {HIGH_BITS, HIGH_BITS}, { LOW_BITS, HIGH_BITS }

void test_trit_to_ptrit(void) {
  trit_t trits[] = {TRITS_IN};
  ptrit_t exp[] = {ptrit_EXP};

  ptrit_t ptrit[3] = {{0, 0}};
  trit_t trit_o[3] = {0};

  for (size_t i = 0; i < 64; i++) {
    trits_to_ptrits(trits, ptrit, i, 3);
    ptrits_to_trits(ptrit, trit_o, i, 3);

    TEST_ASSERT_EQUAL_MEMORY(trits, trit_o, sizeof(trits));
  }

  trits_to_ptrits_fill(trits, ptrit, 3);
  TEST_ASSERT_EQUAL_MEMORY(exp, ptrit, sizeof(exp));
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_trit_to_ptrit);

  return UNITY_END();
}
