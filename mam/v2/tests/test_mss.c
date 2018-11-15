
/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

#include "mam/v2/alloc.h"
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

void mss_test_do(mss_t *m, iprng *p, isponge *s, iwots *w, mss_mt_height_t D) {
  bool_t r = 1;
  MAM2_TRITS_DEF(K, MAM2_PRNG_KEY_SIZE);
  mss_mt_height_t d;
  MAM2_TRITS_DEF(N, 24);
  MAM2_TRITS_DEF(H, MAM2_MSS_HASH_SIZE);
  MAM2_TRITS_DEF(pk, MAM2_MSS_PK_SIZE);
  // MAM2_TRITS_DEF(sig_, MAM2_MSS_SIG_SIZE(MAM2_MSS_TEST_MAX_D));
  trits_t sig_ = trits_alloc(MAM2_MSS_SIG_SIZE(D));

  const char *k_str =
      "ABCNOABCNKOZWYKOZWYSDF9SDF9"
      "YSDF9QABCNKOZWYSDF9ABCNKOZW"
      "SDF9CABCABCNKOZWYNKOZWYSDF9";

  trytes_to_trits(k_str, K.p, MIN(strlen(k_str), K.n / RADIX));
  prng_init(p, p->s, K);
  trits_set_zero(N);
  trits_set_zero(H);
  const char *h_str =
      "ABCNKOZWYSDF9OABCNKOZWYSDF9"
      "ABCNKOZWYSDF9QABCNKOZWYSDF9"
      "ABCNKOZWYSDF9CABCNKOZWYSDF9";
  trytes_to_trits(h_str, H.p, MIN(strlen(h_str), H.n / RADIX));

  for (d = 1; r && d <= D; ++d) {
    trits_t sig = trits_take(sig_, MAM2_MSS_SIG_SIZE(d));
    trits_t sig_skn = trits_take(sig, MAM2_MSS_SKN_SIZE);
#if !defined(MAM2_MSS_DEBUG)
    trits_t sig_wots =
        trits_take(trits_drop(sig, MAM2_MSS_SKN_SIZE), MAM2_WOTS_SIG_SIZE);
    trits_t sig_apath = trits_drop(sig, MAM2_MSS_SKN_SIZE + MAM2_WOTS_SIG_SIZE);
#endif

    dbg_printf("========================\nD = %d\n", d);

    mss_init(m, p, s, w, d, N, trits_null());
    mss_gen(m, pk);

    dbg_printf("mss pk \t");
    trits_dbg_print(pk);
    dbg_printf("\n");

    do {
      dbg_printf("------------------------\nskn = %d\n", m->skn);
      mss_sign(m, H, sig);
      r = r && mss_verify(s, w->s, H, sig, pk);

#if !defined(MAM2_MSS_DEBUG)
      // H is ignored, makes no sense to modify and check
      trits_put1(H, trit_add(trits_get1(H), 1));
      r = r && !mss_verify(s, w->s, H, sig, pk);
      trits_put1(H, trit_sub(trits_get1(H), 1));
#endif

      trits_put1(sig_skn, trit_add(trits_get1(sig_skn), 1));
      r = r && !mss_verify(s, w->s, H, sig, pk);
      trits_put1(sig_skn, trit_sub(trits_get1(sig_skn), 1));

#if !defined(MAM2_MSS_DEBUG)
      // WOTS sig is ignored, makes no sense to modify and check
      trits_put1(sig_wots, trit_add(trits_get1(sig_wots), 1));
      r = r && !mss_verify(s, w->s, H, sig, pk);
      trits_put1(sig_wots, trit_sub(trits_get1(sig_wots), 1));

      if (!trits_is_empty(sig_apath)) {
        trits_put1(sig_apath, trit_add(trits_get1(sig_apath), 1));
        r = r && !mss_verify(s, w->s, H, sig, pk);
        trits_put1(sig_apath, trit_sub(trits_get1(sig_apath), 1));
      }
#endif

      TEST_ASSERT(
          !mss_verify(s, w->s, H, trits_take(sig, trits_size(sig) - 1), pk));
      trits_put1(pk, trit_add(trits_get1(pk), 1));
      TEST_ASSERT(!mss_verify(s, w->s, H, sig, pk));
      trits_put1(pk, trit_sub(trits_get1(pk), 1));

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

  isponge *s = test_sponge_init(_s);
  iprng *p = test_prng_init(_p, s);
  iwots *w = test_wots_init(_w, s);
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
