/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

#include <memory.h>
#include <unity/unity.h>

#include "mam/ntru/ntru.h"
#include "mam/ntru/poly.h"
#include "mam/test_utils/test_utils.h"

static void ntru_test(void) {
  mam_spongos_t spongos;
  mam_prng_t prng;
  mam_ntru_sk_t ntru;

  size_t i;
  size_t test_count = 30;
  MAM_POLY_DEF(f);
  MAM_POLY_DEF(f0);
  MAM_TRITS_DEF0(nonce, 3 * 10);
  MAM_TRITS_DEF0(key, MAM_PRNG_KEY_SIZE);
  MAM_TRITS_DEF0(ekey, MAM_NTRU_EKEY_SIZE);
  MAM_TRITS_DEF0(dekey, MAM_NTRU_KEY_SIZE);
  nonce = MAM_TRITS_INIT(nonce, 3 * 10);
  key = MAM_TRITS_INIT(key, MAM_PRNG_KEY_SIZE);
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
  ntru_init(&ntru);
  trits_t pk = ntru_pk_trits(&ntru);

  i = 0;
  trits_set_zero(key);

  do {
    TEST_ASSERT_TRUE(trits_from_str(nonce, "NONCE9PK99"));
    trits_set_zero(pk);
    trits_put1(pk, 1);
    poly_small_from_trits(f, trits_take(pk, MAM_NTRU_SK_SIZE));
    ntru_gen(&ntru, &prng, nonce);
    memcpy(f0, ntru.f, sizeof(poly_t));
    poly_add(f, f0, f);

    do {
      TEST_ASSERT_TRUE(trits_from_str(nonce, "NONCE9ENC9"));
      ntru_encr(pk, &prng, &spongos, key, nonce, ekey);

      TEST_ASSERT_TRUE(ntru_decr(&ntru, &spongos, ekey, dekey));
      TEST_ASSERT_TRUE(trits_cmp_eq(key, dekey));

      trits_put1(ekey, trit_add(trits_get1(ekey), 1));
      TEST_ASSERT_TRUE(!ntru_decr(&ntru, &spongos, ekey, dekey));
      TEST_ASSERT_TRUE(!trits_cmp_eq(key, dekey));
      trits_put1(ekey, trit_sub(trits_get1(ekey), 1));

      memcpy(ntru.f, f, sizeof(poly_t));
      TEST_ASSERT_TRUE(!ntru_decr(&ntru, &spongos, ekey, dekey));
      TEST_ASSERT_TRUE(!trits_cmp_eq(key, dekey));
      memcpy(ntru.f, f0, sizeof(poly_t));

      trits_from_str(nonce, "NONCE9KEY9");
      mam_prng_gen(&prng, MAM_PRNG_DST_SEC_KEY, nonce, key);
    } while (0 != (++i % (test_count / 10)));
  } while (++i < test_count);
  ntru_destroy(&ntru);
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(ntru_test);

  return UNITY_END();
}
