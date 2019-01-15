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

#include "mam/v2/mss/mss.h"

bool_t mss_test_store(imss *m, imss *m2, iprng *p, ispongos *s, iwots *w,
                      mss_mt_height_t D) {
  bool_t r = 1;
  err_t e;
  mss_mt_height_t d;
  MAM2_TRITS_DEF0(K, MAM2_PRNG_KEY_SIZE);
  MAM2_TRITS_DEF0(N, 24);
  MAM2_TRITS_DEF0(H, MAM2_MSS_HASH_SIZE);
  MAM2_TRITS_DEF0(pk, MAM2_MSS_PK_SIZE);
  MAM2_TRITS_DEF0(pk2, MAM2_MSS_PK_SIZE);
  MAM2_TRITS_DEF0(sig_, MAM2_MSS_SIG_SIZE(5));
  MAM2_TRITS_DEF0(sig2_, MAM2_MSS_SIG_SIZE(5));
  MAM2_TRITS_DEF0(store_, MAM2_MSS_MAX_STORED_SIZE(5));
  trits_t sig, sig2, store;

  K = MAM2_TRITS_INIT(K, MAM2_PRNG_KEY_SIZE);
  N = MAM2_TRITS_INIT(N, 24);
  H = MAM2_TRITS_INIT(H, MAM2_MSS_HASH_SIZE);
  pk = MAM2_TRITS_INIT(pk, MAM2_MSS_PK_SIZE);
  pk2 = MAM2_TRITS_INIT(pk2, MAM2_MSS_PK_SIZE);
  sig_ = MAM2_TRITS_INIT(sig_, MAM2_MSS_SIG_SIZE(5));
  sig2_ = MAM2_TRITS_INIT(sig2_, MAM2_MSS_SIG_SIZE(5));
  store_ = MAM2_TRITS_INIT(store_, MAM2_MSS_MAX_STORED_SIZE(5));

  MAM2_ASSERT(D <= 5);

  trits_from_str(K,
                 "ABCNOABCNKOZWYKOZWYSDF9SDF9"
                 "YSDF9QABCNKOZWYSDF9ABCNKOZW"
                 "SDF9CABCABCNKOZWYNKOZWYSDF9");
  prng_init(p, p->s, K);
  trits_set_zero(N);
  trits_set_zero(H);
  trits_from_str(H,
                 "ABCNKOZWYSDF9OABCNKOZWYSDF9"
                 "ABCNKOZWYSDF9QABCNKOZWYSDF9"
                 "ABCNKOZWYSDF9CABCNKOZWYSDF9");

  for (d = 0; d <= D; ++d) {
    sig = trits_take(sig_, MAM2_MSS_SIG_SIZE(d));
    sig2 = trits_take(sig2_, MAM2_MSS_SIG_SIZE(d));

    mss_init(m, p, s->s, w, d, N, trits_null());
    mss_init(m2, p, s->s, w, d, N, trits_null());
    mss_gen(m, pk);

    do {
      mss_sign(m, H, sig);

      store = trits_take(store_, mss_stored_size(m));
      mss_save(m, store);
      e = mss_load(m2, &store);
      MAM2_ASSERT(err_ok == e);
      mss_sign(m2, H, sig2);

      r = r && trits_cmp_eq(sig, sig2);
    } while (mss_next(m));
  }

  return r;
}

bool_t mss_test(imss *m, iprng *p, ispongos *s, iwots *w, mss_mt_height_t D) {
  bool_t r = 1;
  MAM2_TRITS_DEF0(K, MAM2_PRNG_KEY_SIZE);
  mss_mt_height_t d;
  MAM2_TRITS_DEF0(N, 24);
  MAM2_TRITS_DEF0(H, MAM2_MSS_HASH_SIZE);
  MAM2_TRITS_DEF0(pk, MAM2_MSS_PK_SIZE);
  /*MAM2_TRITS_DEF0(sig_, MAM2_MSS_SIG_SIZE(MAM2_MSS_TEST_MAX_D)); */
  ialloc *a = 0;
  trits_t sig_;

  K = MAM2_TRITS_INIT(K, MAM2_PRNG_KEY_SIZE);
  N = MAM2_TRITS_INIT(N, 24);
  H = MAM2_TRITS_INIT(H, MAM2_MSS_HASH_SIZE);
  pk = MAM2_TRITS_INIT(pk, MAM2_MSS_PK_SIZE);
  /*sig_ = MAM2_TRITS_INIT(sig_, MAM2_MSS_SIG_SIZE(MAM2_MSS_TEST_MAX_D)); */
  sig_ = trits_alloc(a, MAM2_MSS_SIG_SIZE(D));

  trits_from_str(K,
                 "ABCNOABCNKOZWYKOZWYSDF9SDF9"
                 "YSDF9QABCNKOZWYSDF9ABCNKOZW"
                 "SDF9CABCABCNKOZWYNKOZWYSDF9");
  prng_init(p, p->s, K);
  trits_set_zero(N);
  trits_set_zero(H);
  trits_from_str(H,
                 "ABCNKOZWYSDF9OABCNKOZWYSDF9"
                 "ABCNKOZWYSDF9QABCNKOZWYSDF9"
                 "ABCNKOZWYSDF9CABCNKOZWYSDF9");

  for (d = 1; r && d <= D; ++d) {
    trits_t sig = trits_take(sig_, MAM2_MSS_SIG_SIZE(d));
    trits_t sig_skn = trits_take(sig, MAM2_MSS_SKN_SIZE);
#if !defined(MAM2_MSS_DEBUG)
    trits_t sig_wots =
        trits_take(trits_drop(sig, MAM2_MSS_SKN_SIZE), MAM2_WOTS_SIG_SIZE);
    trits_t sig_apath = trits_drop(sig, MAM2_MSS_SKN_SIZE + MAM2_WOTS_SIG_SIZE);
#endif

    dbg_printf("========================\nD = %d\n", d);

    mss_init(m, p, s->s, w, d, N, trits_null());
    mss_gen(m, pk);

    dbg_printf("mss pk \t");
    trits_dbg_print(pk);
    dbg_printf("\n");

    do {
      dbg_printf("------------------------\nskn = %d\n", m->skn);
      mss_sign(m, H, sig);
      r = r && mss_verify(s, w->sg, H, sig, pk);

#if !defined(MAM2_MSS_DEBUG)
      /* H is ignored, makes no sense to modify and check */
      trits_put1(H, trit_add(trits_get1(H), 1));
      r = r && !mss_verify(s, w->sg, H, sig, pk);
      trits_put1(H, trit_sub(trits_get1(H), 1));
#endif

      trits_put1(sig_skn, trit_add(trits_get1(sig_skn), 1));
      r = r && !mss_verify(s, w->sg, H, sig, pk);
      trits_put1(sig_skn, trit_sub(trits_get1(sig_skn), 1));

#if !defined(MAM2_MSS_DEBUG)
      /* WOTS sig is ignored, makes no sense to modify and check */
      trits_put1(sig_wots, trit_add(trits_get1(sig_wots), 1));
      r = r && !mss_verify(s, w->sg, H, sig, pk);
      trits_put1(sig_wots, trit_sub(trits_get1(sig_wots), 1));

      if (!trits_is_empty(sig_apath)) {
        trits_put1(sig_apath, trit_add(trits_get1(sig_apath), 1));
        r = r && !mss_verify(s, w->sg, H, sig, pk);
        trits_put1(sig_apath, trit_sub(trits_get1(sig_apath), 1));
      }
#endif

      r = r &&
          !mss_verify(s, w->sg, H, trits_take(sig, trits_size(sig) - 1), pk);

      trits_put1(pk, trit_add(trits_get1(pk), 1));
      r = r && !mss_verify(s, w->sg, H, sig, pk);
      trits_put1(pk, trit_sub(trits_get1(pk), 1));

    } while (mss_next(m));

#if defined(MAM2_MSS_DEBUG)
    dbg_printf("\nd = %d\n", m->d);
    dbg_printf("  leafs = %d\n", m->gen_leaf_count);
    dbg_printf("  nodes = %d\n", m->hash_node_count);
#endif
  }

  trits_free(a, sig_);

  return r;
}

int main(void) {
  UNITY_BEGIN();

  return UNITY_END();
}
