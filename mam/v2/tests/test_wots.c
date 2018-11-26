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
#include <stdio.h>
#include <string.h>

#include <unity/unity.h>

#include "mam/v2/mam.h"
#include "mam/v2/mss.h"
#include "mam/v2/prng.h"
#include "mam/v2/sponge.h"
#include "mam/v2/tests/common.h"
#include "mam/v2/trits.h"
#include "mam/v2/wots.h"
#include "utils/macros.h"

bool_t wots_test_do(wots_t *w, prng_t *p) {
  TRIT_ARRAY_DECLARE(N, 18);
  trit_array_set_null(&N);
  MAM2_TRITS_DEF(pk, MAM2_WOTS_PK_SIZE);
  TRIT_ARRAY_DECLARE(H, MAM2_WOTS_HASH_SIZE);
  MAM2_TRITS_DEF(sig, MAM2_WOTS_SIG_SIZE);
  // MAM2_TRITS_DEF(pkr, MAM2_WOTS_PK_SIZE);

  trit_array_set_null(&H);
  memset(w->sk, FLEX_TRIT_NULL_VALUE, MAM2_WOTS_SK_FLEX_SIZE);

  prng_gen(p, 7, &N, &H);
  wots_gen_sk(w, p, &N);
  TRIT_ARRAY_MAKE_FROM_RAW(pk_trits_array, MAM2_WOTS_PK_SIZE, pk.p + pk.d);
  wots_calc_pk(w, &pk_trits_array);
  flex_trits_to_trits(pk.p + pk.d, MAM2_WOTS_PK_SIZE, pk_trits_array.trits,
                      MAM2_WOTS_PK_SIZE, MAM2_WOTS_PK_SIZE);

  TRIT_ARRAY_DECLARE(sk_sig_array, MAM2_WOTS_SK_SIZE);
  memcpy(sk_sig_array.trits, w->sk, MAM2_WOTS_SK_FLEX_SIZE);

  size_t hash_size = trit_array_bytes_for_trits(MAM2_WOTS_HASH_SIZE);
  TRIT_ARRAY_DECLARE(hash_array, MAM2_WOTS_HASH_SIZE);
  memcpy(hash_array.trits, H.trits, hash_size);
  wots_sign(w, &hash_array, &sk_sig_array);

  flex_trits_to_trits(sig.p + sig.d, MAM2_WOTS_SK_SIZE, sk_sig_array.trits,
                      MAM2_WOTS_SK_SIZE, MAM2_WOTS_SK_SIZE);

  // Clean
  TRIT_ARRAY_MAKE_FROM_RAW(sig_array, MAM2_WOTS_SIG_SIZE, sig.p + sig.d);
  TRIT_ARRAY_MAKE_FROM_RAW(pk_array, MAM2_WOTS_PK_SIZE, pk.p + pk.d);
  TEST_ASSERT(wots_verify(w->sponge, &hash_array, &sig_array, &pk_array));

  flex_trits_set_at(
      hash_array.trits, MAM2_WOTS_HASH_SIZE, 0,
      trit_sum(flex_trits_at(hash_array.trits, MAM2_WOTS_HASH_SIZE, 0), 1));

  // Mess up hash
  TEST_ASSERT(wots_verify(w->sponge, &hash_array, &sig_array, &pk_array) ==
              false);

  // Clean
  TRIT_ARRAY_MAKE_FROM_RAW(sig_array1, MAM2_WOTS_SIG_SIZE, sig.p + sig.d);
  TRIT_ARRAY_DECLARE(hash_array1, MAM2_WOTS_HASH_SIZE);
  memcpy(hash_array1.trits, H.trits, hash_size);
  TEST_ASSERT(wots_verify(w->sponge, &hash_array1, &sig_array1, &pk_array));

  // Mess up sk
  flex_trits_set_at(sig_array1.trits, MAM2_WOTS_HASH_SIZE, 0,
                    trit_sum(sig.p[sig.d], 1));
  TEST_ASSERT(wots_verify(w->sponge, &hash_array1, &sig_array1, &pk_array) ==
              false);

  // Clean
  TRIT_ARRAY_MAKE_FROM_RAW(sig_array2, MAM2_WOTS_SIG_SIZE, sig.p + sig.d);
  TRIT_ARRAY_DECLARE(hash_array2, MAM2_WOTS_HASH_SIZE);
  memcpy(hash_array2.trits, H.trits, hash_size);
  TEST_ASSERT(wots_verify(w->sponge, &hash_array2, &sig_array2, &pk_array));

  // Mess up pk
  flex_trits_set_at(pk_array.trits, MAM2_WOTS_HASH_SIZE, 0,
                    trit_sum(pk.p[pk.d], 1));

  TEST_ASSERT(wots_verify(w->sponge, &hash_array1, &sig_array1, &pk_array) ==
              false);
}

void wots_test() {
  test_sponge_t _s;
  test_prng_t _p;
  test_wots_t _w;

  sponge_t *s = test_sponge_init(&_s);
  prng_t *p = test_prng_init(&_p, s);
  wots_t *w = test_wots_init(&_w, s);

  flex_trit_t key[FLEX_TRIT_SIZE_243];
  tryte_t const *const key_trytes =
      "NOPQRSTUVWXYZ9ABCDEFGHIJKLMNOPQRSTUVWXYZ9ABCDEFGHIJKLMNOPQRSTUVWXYZ9ABCD"
      "EFGHIJKLM";
  flex_trits_from_trytes(key, MAM2_PRNG_KEY_SIZE, key_trytes, HASH_LENGTH_TRYTE,
                         HASH_LENGTH_TRYTE);
  prng_init(p, p->sponge, key);

  wots_test_do(w, p);
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(wots_test);

  return UNITY_END();
}
