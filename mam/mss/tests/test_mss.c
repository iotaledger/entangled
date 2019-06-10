/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

#include <unity/unity.h>

#include "mam/mss/mss.h"
#include "utils/handles/rand.h"

static void mss_store_test(mam_mss_t *mss1, mam_mss_t *mss2, mam_prng_t *prng, mss_mt_height_t max_height) {
  mss_mt_height_t curr_height;
  MAM_TRITS_DEF(key, MAM_PRNG_SECRET_KEY_SIZE);
  MAM_TRITS_DEF(nonce, 24);
  MAM_TRITS_DEF(hash, MAM_MSS_HASH_SIZE);
  MAM_TRITS_DEF(sig_, MAM_MSS_SIG_SIZE(5));
  MAM_TRITS_DEF(sig2_, MAM_MSS_SIG_SIZE(5));
  MAM_TRITS_DEF(store_, MAM_MSS_MAX_STORED_SIZE(5));
  trits_t sig, sig2, store;
  size_t sks = 0;

  key = MAM_TRITS_INIT(key, MAM_PRNG_SECRET_KEY_SIZE);
  nonce = MAM_TRITS_INIT(nonce, 24);
  hash = MAM_TRITS_INIT(hash, MAM_MSS_HASH_SIZE);
  sig_ = MAM_TRITS_INIT(sig_, MAM_MSS_SIG_SIZE(5));
  sig2_ = MAM_TRITS_INIT(sig2_, MAM_MSS_SIG_SIZE(5));
  store_ = MAM_TRITS_INIT(store_, MAM_MSS_MAX_STORED_SIZE(5));

  trits_from_str(key, "ABCNOABCNKOZWYKOZWYSDF9SDF9YSDF9QABCNKOZWYSDF9ABCNKOZWSDF9CABCABCNKOZWYNKOZWYSDF9");
  mam_prng_init(prng, key);
  trits_set_zero(nonce);
  trits_set_zero(hash);
  trits_from_str(hash, "ABCNKOZWYSDF9OABCNKOZWYSDF9ABCNKOZWYSDF9QABCNKOZWYSDF9ABCNKOZWYSDF9CABCNKOZWYSDF9");

  for (curr_height = 0; curr_height <= max_height; ++curr_height) {
    sks = 0;
    sig = trits_take(sig_, MAM_MSS_SIG_SIZE(curr_height));
    sig2 = trits_take(sig2_, MAM_MSS_SIG_SIZE(curr_height));

    TEST_ASSERT(mam_mss_create(mss1, curr_height) == RC_OK);
    mam_mss_init(mss1, prng, curr_height, nonce, trits_null(), trits_null(), trits_null());
    TEST_ASSERT(mam_mss_create(mss2, curr_height) == RC_OK);
    mam_mss_init(mss2, prng, curr_height, nonce, trits_null(), trits_null(), trits_null());
    mam_mss_gen(mss1);

    while (mam_mss_remaining_sks(mss1) > 0) {
      store = trits_take(store_, mam_mss_serialized_size(mss1));
      mam_mss_serialize(mss1, &store);
      store = trits_pickup_all(store);
      TEST_ASSERT(mam_mss_deserialize(&store, mss2) == RC_OK);

      TEST_ASSERT(mam_mss_sign_and_next(mss1, hash, sig) == RC_OK);
      TEST_ASSERT(mam_mss_sign_and_next(mss2, hash, sig2) == RC_OK);
      TEST_ASSERT_TRUE(trits_cmp_eq(sig, sig2));
      sks++;
    }

    TEST_ASSERT_EQUAL_INT(sks, 1 << curr_height);
    TEST_ASSERT_EQUAL_INT(mam_mss_remaining_sks(mss1), 0);
    TEST_ASSERT(mam_mss_sign_and_next(mss1, hash, sig) == RC_MAM_MSS_EXHAUSTED);
    TEST_ASSERT(mam_mss_sign_and_next(mss2, hash, sig2) == RC_MAM_MSS_EXHAUSTED);

    mam_mss_destroy(mss1);
    mam_mss_destroy(mss2);
  }
}

static void mss_test(mam_mss_t *mss, mam_prng_t *prng, mam_spongos_t *spongos, mss_mt_height_t max_height) {
  MAM_TRITS_DEF(key, MAM_PRNG_SECRET_KEY_SIZE);
  mss_mt_height_t curr_height;
  MAM_TRITS_DEF(nonce, 24);
  MAM_TRITS_DEF(hash, MAM_MSS_HASH_SIZE);
  trits_t sig_;
  size_t sks = 0;

  key = MAM_TRITS_INIT(key, MAM_PRNG_SECRET_KEY_SIZE);
  nonce = MAM_TRITS_INIT(nonce, 24);
  hash = MAM_TRITS_INIT(hash, MAM_MSS_HASH_SIZE);
  sig_ = trits_alloc(MAM_MSS_SIG_SIZE(max_height));

  trits_from_str(key,
                 "ABCNOABCNKOZWYKOZWYSDF9SDF9"
                 "YSDF9QABCNKOZWYSDF9ABCNKOZW"
                 "SDF9CABCABCNKOZWYNKOZWYSDF9");
  mam_prng_init(prng, key);
  trits_set_zero(nonce);
  trits_set_zero(hash);
  trits_from_str(hash,
                 "ABCNKOZWYSDF9OABCNKOZWYSDF9"
                 "ABCNKOZWYSDF9QABCNKOZWYSDF9"
                 "ABCNKOZWYSDF9CABCNKOZWYSDF9");

  for (curr_height = 1; curr_height <= max_height; ++curr_height) {
    sks = 0;
    trits_t sig = trits_take(sig_, MAM_MSS_SIG_SIZE(curr_height));
    trits_t sig_skn = trits_take(sig, MAM_MSS_SKN_SIZE);
    trits_t sig_wots = trits_take(trits_drop(sig, MAM_MSS_SKN_SIZE), MAM_WOTS_SIGNATURE_SIZE);
    trits_t sig_apath = trits_drop(sig, MAM_MSS_SKN_SIZE + MAM_WOTS_SIGNATURE_SIZE);

    TEST_ASSERT(mam_mss_create(mss, curr_height) == RC_OK);
    mam_mss_init(mss, prng, curr_height, nonce, trits_null(), trits_null(), trits_null());
    mam_mss_gen(mss);

    trits_t pk = trits_from_rep(MAM_MSS_PK_SIZE, mss->root);

    while (mam_mss_remaining_sks(mss) > 0) {
      TEST_ASSERT(mam_mss_sign_and_next(mss, hash, sig) == RC_OK);

      TEST_ASSERT_TRUE(mam_mss_verify(spongos, hash, sig, pk));

      /* H is ignored, makes no sense to modify and check */
      trits_put1(hash, trit_add(trits_get1(hash), 1));
      TEST_ASSERT_FALSE(mam_mss_verify(spongos, hash, sig, pk));
      trits_put1(hash, trit_sub(trits_get1(hash), 1));

      trits_put1(sig_skn, trit_add(trits_get1(sig_skn), 1));
      TEST_ASSERT_FALSE(mam_mss_verify(spongos, hash, sig, pk));
      trits_put1(sig_skn, trit_sub(trits_get1(sig_skn), 1));

      /* WOTS sig is ignored, makes no sense to modify and check */
      trits_put1(sig_wots, trit_add(trits_get1(sig_wots), 1));
      TEST_ASSERT_FALSE(mam_mss_verify(spongos, hash, sig, pk));
      trits_put1(sig_wots, trit_sub(trits_get1(sig_wots), 1));

      if (!trits_is_empty(sig_apath)) {
        trits_put1(sig_apath, trit_add(trits_get1(sig_apath), 1));
        TEST_ASSERT_FALSE(mam_mss_verify(spongos, hash, sig, pk));
        trits_put1(sig_apath, trit_sub(trits_get1(sig_apath), 1));
      }

      TEST_ASSERT_FALSE(mam_mss_verify(spongos, hash, trits_take(sig, trits_size(sig) - 1), pk));

      trits_put1(pk, trit_add(trits_get1(pk), 1));
      TEST_ASSERT_FALSE(mam_mss_verify(spongos, hash, sig, pk));
      trits_put1(pk, trit_sub(trits_get1(pk), 1));
      sks++;
    }

    TEST_ASSERT_EQUAL_INT(sks, 1 << curr_height);
    TEST_ASSERT_EQUAL_INT(mam_mss_remaining_sks(mss), 0);
    TEST_ASSERT(mam_mss_sign_and_next(mss, hash, sig) == RC_MAM_MSS_EXHAUSTED);

    mam_mss_destroy(mss);
  }

  trits_free(sig_);
}

static void mss_meta_test(void) {
  mam_spongos_t spongos;
  mam_prng_t prng;
  mam_mss_t mss1;
  mam_mss_t mss2;

  mam_spongos_init(&spongos);

  mss_test(&mss1, &prng, &spongos, 3);
  mss_store_test(&mss1, &mss2, &prng, 2);
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(mss_meta_test);

  return UNITY_END();
}
