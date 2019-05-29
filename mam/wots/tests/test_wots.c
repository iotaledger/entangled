/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

#include <unity/unity.h>

#include "mam/wots/wots.h"

static void mam_wots_test(void) {
  mam_prng_t prng;
  mam_wots_t wots;

  MAM_TRITS_DEF(N, 18);
  MAM_TRITS_DEF(pk, MAM_WOTS_PUBLIC_KEY_SIZE);
  MAM_TRITS_DEF(recovered_pk, MAM_WOTS_PUBLIC_KEY_SIZE);
  MAM_TRITS_DEF(H, MAM_WOTS_HASH_SIZE);
  MAM_TRITS_DEF(sig, MAM_WOTS_SIGNATURE_SIZE);
  MAM_TRITS_DEF(K, MAM_PRNG_SECRET_KEY_SIZE);

  N = MAM_TRITS_INIT(N, 18);
  pk = MAM_TRITS_INIT(pk, MAM_WOTS_PUBLIC_KEY_SIZE);
  recovered_pk = MAM_TRITS_INIT(recovered_pk, MAM_WOTS_PUBLIC_KEY_SIZE);
  H = MAM_TRITS_INIT(H, MAM_WOTS_HASH_SIZE);
  sig = MAM_TRITS_INIT(sig, MAM_WOTS_SIGNATURE_SIZE);
  K = MAM_TRITS_INIT(K, MAM_PRNG_SECRET_KEY_SIZE);

  trits_set_zero(N);
  trits_set_zero(H);
  trits_set_zero(K);

  mam_prng_init(&prng, K);
  mam_wots_reset(&wots);
  mam_prng_gen(&prng, 7, N, H);
  mam_wots_gen_sk(&wots, &prng, N);
  mam_wots_gen_pk(&wots, pk);
  mam_wots_sign(&wots, H, sig);

  mam_wots_recover(H, sig, recovered_pk);
  TEST_ASSERT_TRUE(trits_cmp_eq(pk, recovered_pk));

  trits_put1(H, trit_add(trits_get1(H), 1));
  mam_wots_recover(H, sig, recovered_pk);
  TEST_ASSERT_FALSE(trits_cmp_eq(pk, recovered_pk));
  trits_put1(H, trit_sub(trits_get1(H), 1));

  trits_put1(sig, trit_add(trits_get1(sig), 1));
  mam_wots_recover(H, sig, recovered_pk);
  TEST_ASSERT_FALSE(trits_cmp_eq(pk, recovered_pk));
  trits_put1(sig, trit_sub(trits_get1(sig), 1));

  trits_put1(pk, trit_add(trits_get1(pk), 1));
  mam_wots_recover(H, sig, recovered_pk);
  TEST_ASSERT_FALSE(trits_cmp_eq(pk, recovered_pk));
  trits_put1(pk, trit_sub(trits_get1(pk), 1));
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(mam_wots_test);

  return UNITY_END();
}
