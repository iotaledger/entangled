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

#include "common/trinary/trit_tryte.h"
#include "mam/ntru/ntru.h"
#include "mam/ntru/poly.h"
#include "mam/pb3/pb3.h"

static void ntru_test(void) {
  mam_spongos_t spongos;
  mam_prng_t prng;
  mam_ntru_sk_t ntru;

  size_t i;
  size_t test_count = 30;
  MAM_POLY_DEF(f);
  MAM_POLY_DEF(f0);
  MAM_TRITS_DEF(nonce, 3 * 10);
  MAM_TRITS_DEF(key, MAM_PRNG_SECRET_KEY_SIZE);
  MAM_TRITS_DEF(ekey, MAM_NTRU_EKEY_SIZE);
  MAM_TRITS_DEF(dekey, MAM_NTRU_KEY_SIZE);
  nonce = MAM_TRITS_INIT(nonce, 3 * 10);
  key = MAM_TRITS_INIT(key, MAM_PRNG_SECRET_KEY_SIZE);
  ekey = MAM_TRITS_INIT(ekey, MAM_NTRU_EKEY_SIZE);
  dekey = MAM_TRITS_INIT(dekey, MAM_NTRU_KEY_SIZE);

  trits_from_str(key,
                 "AAABBBCCCAAABBBCCCAAABBBCCC"
                 "AAABBBCCCAAABBBCCCAAABBBCCC"
                 "AAABBBCCCAAABBBCCCAAABBBCCC");
  /* it'spongos safe to reuse sponge from spongos for prng */
  /* as spongos is exclusively used in ntru_encr/ntru_decr. */
  mam_spongos_init(&spongos);
  mam_prng_init(&prng, key);
  ntru_sk_reset(&ntru);
  trits_t pk = ntru_sk_pk_key(&ntru);

  i = 0;
  trits_set_zero(key);

  do {
    TEST_ASSERT_TRUE(trits_from_str(nonce, "NONCE9PK99"));
    trits_set_zero(pk);
    trits_put1(pk, 1);
    poly_small_from_trits(f, trits_take(pk, MAM_NTRU_SK_SIZE));
    ntru_sk_gen(&ntru, &prng, nonce);
    memcpy(f0, ntru.f, sizeof(poly_t));
    poly_add(f, f0, f);

    do {
      TEST_ASSERT_TRUE(trits_from_str(nonce, "NONCE9ENC9"));
      TEST_ASSERT(ntru_pk_encr(&ntru.public_key, &prng, &spongos, nonce, key, ekey) == RC_OK);

      TEST_ASSERT_TRUE(ntru_sk_decr(&ntru, &spongos, ekey, dekey));
      TEST_ASSERT_TRUE(trits_cmp_eq(key, dekey));

      trits_put1(ekey, trit_add(trits_get1(ekey), 1));
      TEST_ASSERT_TRUE(!ntru_sk_decr(&ntru, &spongos, ekey, dekey));
      TEST_ASSERT_TRUE(!trits_cmp_eq(key, dekey));
      trits_put1(ekey, trit_sub(trits_get1(ekey), 1));

      memcpy(ntru.f, f, sizeof(poly_t));
      TEST_ASSERT_TRUE(!ntru_sk_decr(&ntru, &spongos, ekey, dekey));
      TEST_ASSERT_TRUE(!trits_cmp_eq(key, dekey));
      memcpy(ntru.f, f0, sizeof(poly_t));

      trits_from_str(nonce, "NONCE9KEY9");
      mam_prng_gen(&prng, MAM_PRNG_DST_SEC_KEY, nonce, key);
    } while (0 != (++i % (test_count / 10)));
  } while (++i < test_count);
  ntru_sk_reset(&ntru);
}

static void test_ntru_pk_serialization(void) {
  mam_ntru_pk_t_set_t ntru_set_1 = NULL;
  mam_ntru_pk_t_set_t ntru_set_2 = NULL;
  mam_ntru_pk_t ntru;
  tryte_t ntru_pk[MAM_NTRU_PK_SIZE / 3];

  for (size_t i = 0; i < 26; i++) {
    memset(ntru_pk, 'A' + i, MAM_NTRU_PK_SIZE / 3);
    trytes_to_trits(ntru_pk, ntru.key, MAM_NTRU_PK_SIZE / 3);
    TEST_ASSERT(mam_ntru_pk_t_set_add(&ntru_set_1, &ntru) == RC_OK);
  }

  size_t size = mam_ntru_pks_serialized_size(ntru_set_1);

  TEST_ASSERT_EQUAL_INT(size, 26 * MAM_NTRU_PK_SIZE + pb3_sizeof_size_t(26));

  trits_t trits = trits_alloc(size + pb3_sizeof_size_t(26));

  TEST_ASSERT(mam_ntru_pks_serialize(ntru_set_1, &trits) == RC_OK);

  trits = trits_pickup_all(trits);

  TEST_ASSERT(mam_ntru_pks_deserialize(&trits, &ntru_set_2) == RC_OK);

  TEST_ASSERT_TRUE(mam_ntru_pk_t_set_cmp(ntru_set_1, ntru_set_2));

  trits_free(trits);
  mam_ntru_pk_t_set_free(&ntru_set_1);
  mam_ntru_pk_t_set_free(&ntru_set_2);
}

static void test_ntru_sk_serialization(void) {
  mam_ntru_sk_t_set_t ntru_sk_set_1 = NULL;
  mam_ntru_sk_t_set_t ntru_sk_set_2 = NULL;
  mam_ntru_sk_t ntru_sk;
  mam_prng_t prng;
  MAM_TRITS_DEF(key, MAM_PRNG_SECRET_KEY_SIZE);
  MAM_TRITS_DEF(nonce, 3 * 10);
  key = MAM_TRITS_INIT(key, MAM_PRNG_SECRET_KEY_SIZE);
  nonce = MAM_TRITS_INIT(nonce, 3 * 10);

  trits_from_str(key,
                 "AAABBBCCCAAABBBCCCAAABBBCCC"
                 "AAABBBCCCAAABBBCCCAAABBBCCC"
                 "AAABBBCCCAAABBBCCCAAABBBCCC");

  mam_prng_init(&prng, key);
  ntru_sk_reset(&ntru_sk);

  for (int i = -1; i <= 1; i++) {
    memset(nonce.p, i, 3 * 10);
    ntru_sk_gen(&ntru_sk, &prng, nonce);
    TEST_ASSERT(mam_ntru_sk_t_set_add(&ntru_sk_set_1, &ntru_sk) == RC_OK);
  }

  size_t size = mam_ntru_sks_serialized_size(ntru_sk_set_1);

  TEST_ASSERT_EQUAL_INT(size, 3 * (MAM_NTRU_PK_SIZE + MAM_NTRU_SK_SIZE) + pb3_sizeof_size_t(3));

  trits_t trits = trits_alloc(size);

  TEST_ASSERT(mam_ntru_sks_serialize(ntru_sk_set_1, &trits) == RC_OK);
  trits = trits_pickup_all(trits);
  TEST_ASSERT(mam_ntru_sks_deserialize(&trits, &ntru_sk_set_2) == RC_OK);

  TEST_ASSERT_TRUE(mam_ntru_sk_t_set_cmp(ntru_sk_set_1, ntru_sk_set_2));

  mam_ntru_sk_t_set_free(&ntru_sk_set_1);
  mam_ntru_sk_t_set_free(&ntru_sk_set_2);

  trits_free(trits);
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(ntru_test);
  RUN_TEST(test_ntru_pk_serialization);
  RUN_TEST(test_ntru_sk_serialization);

  return UNITY_END();
}
