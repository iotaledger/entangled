/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <string.h>
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

#define TEST_N 6
#define TEST_M ((3 * 3 * 3 * 3 * 3 * 3 - 1) / 2)

void test_ptrits_set_iota(void) {
  ptrit_t p[TEST_N];
  trit_t value[TEST_N];
  size_t i;
  int s;

  memset(value, -1, sizeof(value));
  ptrit_set_iota(TEST_N, p, value);
  ptrits_get_slice(TEST_N, value, p, 0);
  s = -TEST_M;
  TEST_ASSERT_EQUAL_INT(s, trits_to_int(TEST_N, value));

  for (i = 1; i < PTRIT_SIZE; ++i) {
    ptrits_get_slice(TEST_N, value, p, i);
    ++s;
    TEST_ASSERT_EQUAL_INT(s, trits_to_int(TEST_N, value));
  }
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_ptrits_set_iota);

  return UNITY_END();
}
