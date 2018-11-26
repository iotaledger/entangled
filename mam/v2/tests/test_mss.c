
/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

#include "mam/v2/buffers.h"
#include "mam/v2/curl.h"
#include "mam/v2/mam.h"
#include "mam/v2/mss.h"
#include "mam/v2/ntru.h"
#include "mam/v2/pb3.h"
#include "mam/v2/prng.h"
#include "mam/v2/sponge.h"
#include "mam/v2/tests/common.h"
#include "mam/v2/trits.h"
#include "mam/v2/wots.h"

#include "utils/macros.h"

#include <string.h>
#include <unity/unity.h>

#include <memory.h>
#include <stdio.h>

void mss_test_do(mss_t *m, prng_t *p, sponge_t *s, wots_t *w,
                 mss_mt_height_t mt_height) {
  bool_t r = 1;
  flex_trit_t key[FLEX_TRIT_SIZE_243];
  // TODO Remove when sponge handles flex_trit_t
  MAM2_TRITS_DEF(K, MAM2_PRNG_KEY_SIZE);
  mss_mt_height_t d;
  MAM2_TRITS_DEF(nonce, 24);
  MAM2_TRITS_DEF(hash, MAM2_MSS_HASH_SIZE);
  trits_t sig_ = trits_alloc(MAM2_MSS_SIG_SIZE(mt_height));

  tryte_t const *const key_trytes =
      "ABCNOABCNKOZWYKOZWYSDF9SDF9YSDF9QABCNKOZWYSDF9ABCNKOZWSDF9CABCABCNKOZWYN"
      "KOZWYSDF9";

  trytes_to_trits(key_trytes, K.p, MIN(strlen(key_trytes), K.n / RADIX));
  flex_trits_from_trytes(key, MAM2_PRNG_KEY_SIZE, key_trytes, HASH_LENGTH_TRYTE,
                         HASH_LENGTH_TRYTE);
  prng_init(p, p->sponge, key);
  trits_set_zero(nonce);
  trits_set_zero(hash);
  const char *h_str =
      "ABCNKOZWYSDF9OABCNKOZWYSDF9"
      "ABCNKOZWYSDF9QABCNKOZWYSDF9"
      "ABCNKOZWYSDF9CABCNKOZWYSDF9";
  trytes_to_trits(h_str, hash.p, MIN(strlen(h_str), hash.n / RADIX));

  for (d = 1; r && d <= mt_height; ++d) {
    trits_t sig = trits_take(sig_, MAM2_MSS_SIG_SIZE(d));
    trits_t sig_skn = trits_take(sig, MAM2_MSS_SKN_SIZE);
#if !defined(MAM2_MSS_DEBUG)
    trits_t sig_wots =
        trits_take(trits_drop(sig, MAM2_MSS_SKN_SIZE), MAM2_WOTS_SIG_SIZE);
    trits_t sig_apath = trits_drop(sig, MAM2_MSS_SKN_SIZE + MAM2_WOTS_SIG_SIZE);
#endif

    dbg_printf("========================\nmt_height = %d\n", d);

    mss_init(m, p, s, w, d, nonce, trits_null());
    TRIT_ARRAY_DECLARE(pk, MAM2_WOTS_PK_SIZE);
    mss_gen(m, &pk);

    dbg_printf("mss pk \t");
    // trits_dbg_print(pk);
    dbg_printf("\n");

    do {
      dbg_printf("------------------------\nskn = %d\n", m->skn);
      mss_sign(m, hash, sig);
      TEST_ASSERT(mss_verify(s, w->sponge, hash, sig, &pk));

#if !defined(MAM2_MSS_DEBUG)
      // H is ignored, makes no sense to modify and check
      trits_put1(hash, trit_add(trits_get1(hash), 1));
      TEST_ASSERT(!mss_verify(s, w->sponge, hash, sig, &pk));
      trits_put1(hash, trit_sub(trits_get1(hash), 1));
#endif

      trits_put1(sig_skn, trit_add(trits_get1(sig_skn), 1));
      TEST_ASSERT(!mss_verify(s, w->sponge, hash, sig, &pk));
      trits_put1(sig_skn, trit_sub(trits_get1(sig_skn), 1));

#if !defined(MAM2_MSS_DEBUG)
      // WOTS sig is ignored, makes no sense to modify and check
      trits_put1(sig_wots, trit_add(trits_get1(sig_wots), 1));
      TEST_ASSERT(!mss_verify(s, w->sponge, hash, sig, &pk));
      trits_put1(sig_wots, trit_sub(trits_get1(sig_wots), 1));

      if (!trits_is_empty(sig_apath)) {
        trits_put1(sig_apath, trit_add(trits_get1(sig_apath), 1));
        TEST_ASSERT(!mss_verify(s, w->sponge, hash, sig, &pk));
        trits_put1(sig_apath, trit_sub(trits_get1(sig_apath), 1));
      }
#endif

      TEST_ASSERT(!mss_verify(s, w->sponge, hash,
                              trits_take(sig, trits_size(sig) - 1), &pk));
      trit_t orig = trit_array_at(&pk, 0);
      trit_array_set_at(&pk, 0, trit_sum(orig, 1));
      TEST_ASSERT(!mss_verify(s, w->sponge, hash, sig, &pk));
      trit_array_set_at(&pk, 0, orig);

    } while (mss_next(m));
  }

  trits_free(sig_);
}

void mss_test() {
  test_sponge_t _s[1];
  test_prng_t _p[1];
  test_wots_t _w[1];
  test_mss1 _m1[1];
  test_mss2 _m2[1];
  test_mss3 _m3[1];
  test_mss4 _m4[1];
  test_mss _m[1];

  sponge_t *s = test_sponge_init(_s);
  prng_t *p = test_prng_init(_p, s);
  wots_t *w = test_wots_init(_w, s);
  mss_t *m1 = test_mss_init1(_m1);
  mss_t *m2 = test_mss_init2(_m2);
  mss_t *m3 = test_mss_init3(_m3);
  mss_t *m4 = test_mss_init4(_m4);
  mss_t *m = test_mss_init(_m);
  mss_test_do(m1, p, s, w, 1);
  mss_test_do(m2, p, s, w, 2);
  mss_test_do(m3, p, s, w, 3);
  mss_test_do(m4, p, s, w, 4);
  mss_test_do(m, p, s, w, MAM2_MSS_TEST_MAX_D);
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(mss_test);

  return UNITY_END();
}
