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

#include "mam/v2/test_utils/test_utils.h"
#include "mam/v2/wots/wots.h"

static void mam_wots_test(void) {
  mam_prng_t prng;
  mam_wots_t wots;

  MAM_TRITS_DEF0(N, 18);
  MAM_TRITS_DEF0(pk, MAM_WOTS_PK_SIZE);
  MAM_TRITS_DEF0(H, MAM_WOTS_HASH_SIZE);
  MAM_TRITS_DEF0(sig, MAM_WOTS_SIG_SIZE);
  MAM_TRITS_DEF0(K, MAM_PRNG_KEY_SIZE);

  /*MAM_TRITS_DEF0(pkr, MAM_WOTS_PK_SIZE);*/
  N = MAM_TRITS_INIT(N, 18);
  pk = MAM_TRITS_INIT(pk, MAM_WOTS_PK_SIZE);
  H = MAM_TRITS_INIT(H, MAM_WOTS_HASH_SIZE);
  sig = MAM_TRITS_INIT(sig, MAM_WOTS_SIG_SIZE);
  K = MAM_TRITS_INIT(K, MAM_PRNG_KEY_SIZE);
  /*pkr = MAM_TRITS_INIT(pkr, MAM_WOTS_PK_SIZE);*/

  trits_set_zero(N);
  trits_set_zero(H);
  trits_set_zero(K);
  trits_set_zero(trits_from_rep(MAM_WOTS_SK_SIZE, wots.secret_key));

  // mam_sponge_init(&sponge);
  mam_prng_init(&prng, K);
  mam_wots_init(&wots);
  mam_prng_gen(&prng, 7, N, H);
  mam_wots_gen_sk(&wots, &prng, N);
  mam_wots_calc_pk(&wots, pk);
  mam_wots_sign(&wots, H, sig);

  mam_spongos_t wots_spongos;
  mam_spongos_init(&wots_spongos);
  /*mam_wots_recover(&wots.spongos, H, sig, pkr);*/

  TEST_ASSERT_TRUE(mam_wots_verify(&wots_spongos, H, sig, pk));

  trits_put1(H, trit_add(trits_get1(H), 1));
  TEST_ASSERT_TRUE(!mam_wots_verify(&wots_spongos, H, sig, pk));
  trits_put1(H, trit_sub(trits_get1(H), 1));

  trits_put1(sig, trit_add(trits_get1(sig), 1));
  TEST_ASSERT_TRUE(!mam_wots_verify(&wots_spongos, H, sig, pk));
  trits_put1(sig, trit_sub(trits_get1(sig), 1));

  trits_put1(pk, trit_add(trits_get1(pk), 1));
  TEST_ASSERT_TRUE(!mam_wots_verify(&wots_spongos, H, sig, pk));
  trits_put1(pk, trit_sub(trits_get1(pk), 1));
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(mam_wots_test);

  return UNITY_END();
}
