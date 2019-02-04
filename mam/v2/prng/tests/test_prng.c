/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

#include <unity/unity.h>

#include "mam/v2/prng/prng.h"
#include "mam/v2/test_utils/test_utils.h"

static void prng_test(void) {
  test_prng_t test_prng;
  mam_prng_t *prng = test_prng_init(&test_prng);

  MAM2_TRITS_DEF0(K, MAM2_PRNG_KEY_SIZE);
  MAM2_TRITS_DEF0(N, 18);
  MAM2_TRITS_DEF0(Y1, 243 * 2 + 18);
  MAM2_TRITS_DEF0(Y2, 243 * 2 + 18);
  K = MAM2_TRITS_INIT(K, MAM2_PRNG_KEY_SIZE);
  N = MAM2_TRITS_INIT(N, 18);
  Y1 = MAM2_TRITS_INIT(Y1, 243 * 2 + 18);
  Y2 = MAM2_TRITS_INIT(Y2, 243 * 2 + 18);

  trits_set_zero(K);
  trits_set_zero(N);
  trits_from_str(K,
                 "NOPQRSTUVWXYZ9ABCDEFGHIJKLM"
                 "NOPQRSTUVWXYZ9ABCDEFGHIJKLM"
                 "NOPQRSTUVWXYZ9ABCDEFGHIJKLM");

  mam_prng_init(prng, K);
  mam_prng_gen(prng, 0, N, Y1);
  mam_prng_gen(prng, 1, N, Y2);

  TEST_ASSERT_TRUE(!trits_cmp_eq(Y1, Y2));
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(prng_test);

  return UNITY_END();
}
