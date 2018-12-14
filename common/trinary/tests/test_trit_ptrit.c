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
  ptrit_t ptrit[3];
  ptrit_t exp[] = {ptrit_EXP};
  trits_to_ptrits_fill(trits, ptrit, 3);
  TEST_ASSERT_EQUAL_MEMORY(exp, ptrit, sizeof(exp));
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_trit_to_ptrit);

  return UNITY_END();
}
