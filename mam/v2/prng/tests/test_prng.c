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
  test_sponge_t test_sponge;
  test_prng_t test_prng;
  sponge_t *sponge = test_sponge_init(&test_sponge);
  prng_t *prng = test_prng_init(&test_prng, sponge);

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

  sponge_init(prng->sponge);
  sponge_absorb(prng->sponge, MAM2_SPONGE_CTL_KEY, K);
  sponge_squeeze(prng->sponge, MAM2_SPONGE_CTL_KEY, K);

  prng_init(prng, prng->sponge, K);
  prng_gen(prng, 0, N, Y1);
  prng_gen(prng, 1, N, Y2);

  TEST_ASSERT_TRUE(!trits_cmp_eq(Y1, Y2));
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(prng_test);

  return UNITY_END();
}
