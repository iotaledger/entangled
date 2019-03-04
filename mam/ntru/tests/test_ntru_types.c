/*
 * Copyright (c) 2018 IOTA Stiftung
 * https:github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

#include <string.h>

#include <unity/unity.h>

#include "common/trinary/trit_tryte.h"
#include "mam/ntru/mam_ntru_pk_t_set.h"
#include "mam/ntru/mam_ntru_sk_t_set.h"
#include "mam/ntru/ntru.h"
#include "mam/ntru/ntru_types.h"

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

  TEST_ASSERT_EQUAL_INT(size, 26 * MAM_NTRU_PK_SIZE);

  trits_t trits = trits_alloc(size);

  TEST_ASSERT(mam_ntru_pks_serialize(ntru_set_1, trits) == RC_OK);

  TEST_ASSERT(mam_ntru_pks_deserialize(trits, &ntru_set_2) == RC_OK);

  TEST_ASSERT_TRUE(mam_ntru_pk_t_set_cmp(&ntru_set_1, &ntru_set_2));

  trits_free(trits);
  mam_ntru_pk_t_set_free(&ntru_set_1);
  mam_ntru_pk_t_set_free(&ntru_set_2);
}

static void test_ntru_sk_serialization(void) {
  mam_ntru_sk_t_set_t ntru_sk_set_1 = NULL;
  mam_ntru_sk_t_set_t ntru_sk_set_2 = NULL;
  mam_ntru_sk_t ntru_sk;
  mam_prng_t prng;
  MAM_TRITS_DEF0(key, MAM_PRNG_KEY_SIZE);
  MAM_TRITS_DEF0(nonce, 3 * 10);
  key = MAM_TRITS_INIT(key, MAM_PRNG_KEY_SIZE);
  nonce = MAM_TRITS_INIT(nonce, 3 * 10);

  trits_from_str(key,
                 "AAABBBCCCAAABBBCCCAAABBBCCC"
                 "AAABBBCCCAAABBBCCCAAABBBCCC"
                 "AAABBBCCCAAABBBCCCAAABBBCCC");

  mam_prng_init(&prng, key);
  ntru_init(&ntru_sk);

  for (int i = -1; i <= 1; i++) {
    memset(nonce.p, i, 3 * 10);
    ntru_gen(&ntru_sk, &prng, nonce);
    TEST_ASSERT(mam_ntru_sk_t_set_add(&ntru_sk_set_1, &ntru_sk) == RC_OK);
  }

  size_t size = mam_ntru_sks_serialized_size(ntru_sk_set_1);

  TEST_ASSERT_EQUAL_INT(size, 3 * (MAM_NTRU_PK_SIZE + MAM_NTRU_SK_SIZE));

  trits_t trits = trits_alloc(size);

  TEST_ASSERT(mam_ntru_sks_serialize(ntru_sk_set_1, trits) == RC_OK);

  TEST_ASSERT(mam_ntru_sks_deserialize(trits, &ntru_sk_set_2) == RC_OK);

  TEST_ASSERT_TRUE(mam_ntru_sk_t_set_cmp(&ntru_sk_set_1, &ntru_sk_set_2));

  mam_ntru_sk_t_set_free(&ntru_sk_set_1);
  mam_ntru_sk_t_set_free(&ntru_sk_set_2);

  trits_free(trits);
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_ntru_pk_serialization);
  RUN_TEST(test_ntru_sk_serialization);

  return UNITY_END();
}
