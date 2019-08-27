/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <unity/unity.h>

#include "common/trinary/ptrit.h"
#include "common/trinary/ptrit_incr.h"

int trits_to_int(size_t n, trit_t const *t) {
  int s = 0;
  for (t += n; n--;) {
    s = s * 3 + (int)*--t;
  }
  return s;
}

void test_ptrits_set_iota(void) {
  ptrit_t p[PTRIT_SIZE_LOG3];
  trit_t pi[PTRIT_SIZE_LOG3];
  size_t i;
  int s;

  ptrits_set_iota(p);
  ptrits_get_slice(PTRIT_SIZE_LOG3, pi, p, 0);
  s = trits_to_int(PTRIT_SIZE_LOG3, pi);

  for (i = 1; i < PTRIT_SIZE; ++i) {
    ptrits_get_slice(PTRIT_SIZE_LOG3, pi, p, i);
    ++s;
    TEST_ASSERT_EQUAL_INT(s, trits_to_int(PTRIT_SIZE_LOG3, pi));
  }
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_ptrits_set_iota);

  return UNITY_END();
}
