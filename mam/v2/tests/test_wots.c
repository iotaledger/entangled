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
  bool_t r = 1;

  TRIT_ARRAY_DECLARE(N, 18);
  trit_array_set_null(&N);
  MAM2_TRITS_DEF(pk, MAM2_WOTS_PK_SIZE);
  MAM2_TRITS_DEF(H, MAM2_WOTS_HASH_SIZE);
  MAM2_TRITS_DEF(sig, MAM2_WOTS_SIG_SIZE);
  // MAM2_TRITS_DEF(pkr, MAM2_WOTS_PK_SIZE);

  trits_set_zero(H);
  memset(w->sk, FLEX_TRIT_NULL_VALUE, MAM2_WOTS_SK_FLEX_SIZE);

  prng_gen(p, 7, &N, H);
  wots_gen_sk(w, p, &N);
  wots_calc_pk(w, pk);
  wots_sign(w, H, sig);
  TEST_ASSERT(wots_verify(w->sponge, H, sig, pk));

  trits_put1(H, trit_add(trits_get1(H), 1));
  r = r && !wots_verify(w->sponge, H, sig, pk);
  trits_put1(H, trit_sub(trits_get1(H), 1));

  trits_put1(sig, trit_add(trits_get1(sig), 1));
  TEST_ASSERT(!wots_verify(w->sponge, H, sig, pk));
  trits_put1(sig, trit_sub(trits_get1(sig), 1));

  trits_put1(pk, trit_add(trits_get1(pk), 1));
  r = r && !wots_verify(w->sponge, H, sig, pk);
  trits_put1(pk, trit_sub(trits_get1(pk), 1));

  TEST_ASSERT(r);
}

void wots_test() {
  test_sponge_t _s;
  test_prng_t _p;
  test_wots_t _w;

  isponge *s = test_sponge_init(&_s);
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
