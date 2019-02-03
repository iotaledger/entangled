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
  test_mam_sponge_t test_sponge;
  test_prng_t test_prng;
  test_mam_wots_t test_wots;
  mam_sponge_t *sponge = test_mam_sponge_init(&test_sponge);
  mam_prng_t *prng = test_prng_init(&test_prng, sponge);
  mam_wots_t *wots = test_mam_wots_init(&test_wots, sponge);
  mam_wots_t deserialized_wots;

  deserialized_wots.spongos.sponge = test_create_sponge();

  MAM2_TRITS_DEF0(N, 18);
  MAM2_TRITS_DEF0(pk, MAM2_WOTS_PK_SIZE);
  MAM2_TRITS_DEF0(H, MAM2_WOTS_HASH_SIZE);
  MAM2_TRITS_DEF0(sig, MAM2_WOTS_SIG_SIZE);
  MAM2_TRITS_DEF0(serialized_wots_trits, mam_wots_serialized_size(wots));
  /*MAM2_TRITS_DEF0(pkr, MAM2_WOTS_PK_SIZE);*/
  N = MAM2_TRITS_INIT(N, 18);
  pk = MAM2_TRITS_INIT(pk, MAM2_WOTS_PK_SIZE);
  H = MAM2_TRITS_INIT(H, MAM2_WOTS_HASH_SIZE);
  sig = MAM2_TRITS_INIT(sig, MAM2_WOTS_SIG_SIZE);
  serialized_wots_trits =
      MAM2_TRITS_INIT(serialized_wots_trits, mam_wots_serialized_size(wots));
  /*pkr = MAM2_TRITS_INIT(pkr, MAM2_WOTS_PK_SIZE);*/

  trits_set_zero(N);
  trits_set_zero(H);
  trits_set_zero(trits_from_rep(MAM2_WOTS_SK_SIZE, wots->secret_key));

  mam_prng_gen(prng, 7, N, H);
  mam_wots_gen_sk(wots, prng, N);
  mam_wots_calc_pk(wots, pk);
  mam_wots_sign(wots, H, sig);

  /*mam_wots_recover(&wots->spongos, H, sig, pkr);*/

  TEST_ASSERT_TRUE(mam_wots_verify(&wots->spongos, H, sig, pk));

  trits_put1(H, trit_add(trits_get1(H), 1));
  TEST_ASSERT_TRUE(!mam_wots_verify(&wots->spongos, H, sig, pk));
  trits_put1(H, trit_sub(trits_get1(H), 1));

  trits_put1(sig, trit_add(trits_get1(sig), 1));
  TEST_ASSERT_TRUE(!mam_wots_verify(&wots->spongos, H, sig, pk));
  trits_put1(sig, trit_sub(trits_get1(sig), 1));

  trits_put1(pk, trit_add(trits_get1(pk), 1));
  TEST_ASSERT_TRUE(!mam_wots_verify(&wots->spongos, H, sig, pk));
  trits_put1(pk, trit_sub(trits_get1(pk), 1));

  // Test Serialization

  mam_wots_serialize(wots, serialized_wots_trits);
  mam_mam_wots_deserialize(&serialized_wots_trits, &deserialized_wots);

  TEST_ASSERT_EQUAL_MEMORY(wots->secret_key, deserialized_wots.secret_key,
                           MAM2_WOTS_SK_SIZE);
  TEST_ASSERT_EQUAL_INT(wots->spongos.pos, deserialized_wots.spongos.pos);
  TEST_ASSERT_EQUAL_MEMORY(wots->spongos.sponge->state,
                           deserialized_wots.spongos.sponge->state,
                           MAM2_SPONGE_WIDTH);

  test_delete_sponge(deserialized_wots.spongos.sponge);
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(mam_wots_test);

  return UNITY_END();
}
