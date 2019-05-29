/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

#include <unity/unity.h>

#include "common/trinary/trit_tryte.h"
#include "mam/prng/prng.h"

static void test_prng(void) {
  mam_prng_t prng;

  MAM_TRITS_DEF(K, MAM_PRNG_SECRET_KEY_SIZE);
  MAM_TRITS_DEF(N, 18);
  MAM_TRITS_DEF(Y1, 243 * 2 + 18);
  MAM_TRITS_DEF(Y2, 243 * 2 + 18);
  K = MAM_TRITS_INIT(K, MAM_PRNG_SECRET_KEY_SIZE);
  N = MAM_TRITS_INIT(N, 18);
  Y1 = MAM_TRITS_INIT(Y1, 243 * 2 + 18);
  Y2 = MAM_TRITS_INIT(Y2, 243 * 2 + 18);

  trits_set_zero(K);
  trits_set_zero(N);
  trits_from_str(K,
                 "NOPQRSTUVWXYZ9ABCDEFGHIJKLM"
                 "NOPQRSTUVWXYZ9ABCDEFGHIJKLM"
                 "NOPQRSTUVWXYZ9ABCDEFGHIJKLM");

  mam_prng_init(&prng, K);
  mam_prng_gen(&prng, 0, N, Y1);
  mam_prng_gen(&prng, 1, N, Y2);

  TEST_ASSERT_TRUE(!trits_cmp_eq(Y1, Y2));
}

static void test_prng_serialization(void) {
  mam_prng_t prng1;
  mam_prng_t prng2;
  tryte_t secret_key_str[MAM_PRNG_SECRET_KEY_SIZE / 3];

  memset(secret_key_str, 'M', MAM_PRNG_SECRET_KEY_SIZE / 3);
  trytes_to_trits(secret_key_str, prng1.secret_key, MAM_PRNG_SECRET_KEY_SIZE / 3);

  size_t size = mam_prng_serialized_size();

  TEST_ASSERT_EQUAL_INT(size, MAM_PRNG_SECRET_KEY_SIZE);

  trits_t trits = trits_alloc(size);

  mam_prng_serialize(&prng1, &trits);
  trits = trits_pickup_all(trits);

  TEST_ASSERT(mam_prng_deserialize(&trits, &prng2) == RC_OK);

  TEST_ASSERT_EQUAL_MEMORY(prng1.secret_key, prng2.secret_key, MAM_PRNG_SECRET_KEY_SIZE);

  trits_free(trits);
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_prng);
  RUN_TEST(test_prng_serialization);

  return UNITY_END();
}
