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
#include "mam/v2/test_utils/test_utils.h"

#if !defined(MAM2_MSS_TEST_MAX_D)
#define MAM2_MSS_TEST_MAX_D 3
#endif

#if defined(MAM2_MSS_TRAVERSAL)
#define def_test_mss(D, sfx)                    \
  typedef struct test_mss##sfx##_s {            \
    mss_t m;                                    \
    trit_t ap[MAM2_MSS_MT_AUTH_WORDS(D)];       \
    uint32_t ap_check;                          \
    trit_t hs[MAM2_MSS_MT_HASH_WORDS(D, 1)];    \
    uint32_t hs_check;                          \
    mss_mt_node_t ns[MAM2_MSS_MT_NODES(D) + 1]; \
    uint32_t ns_check;                          \
    mss_mt_stack_t ss[MAM2_MSS_MT_STACKS(D)];   \
    uint32_t ss_check;                          \
  } test_mss##sfx##_t
#else
#define def_test_mss(D, sfx)         \
  typedef struct test_mss##sfx##_s { \
    mss_t m;                         \
    trit_t mt[MAM2_MSS_MT_WORDS(D)]; \
    uint32_t mt_check;               \
  } test_mss##sfx##_t
#endif

#if defined(MAM2_MSS_TRAVERSAL)
#define def_test_mss_init(D, sfx)                          \
  static mss_t *test_mss_init##sfx(test_mss##sfx##_t *m) { \
    m->m.ap = m->ap;                                       \
    m->ap_check = 0xdeadbeef;                              \
    m->m.hs = m->hs;                                       \
    m->hs_check = 0xdeadbeef;                              \
    m->m.ns = m->ns;                                       \
    m->ns_check = 0xdeadbeef;                              \
    m->m.ss = m->ss;                                       \
    m->ss_check = 0xdeadbeef;                              \
    return &m->m;                                          \
  }
#else
#define def_test_mss_init(D, sfx)                                     \
  static mss_t *test_mss_init##sfx(test_mss##sfx##_t *m, wots_t *w) { \
    m->m.d = D;                                                       \
    m->m.spongos_wots = w;                                            \
    m->m.mt = m->mt;                                                  \
    m->mt_check = 0xdeadbeef;                                         \
    return &m->m;                                                     \
  }
#endif

#if defined(MAM2_MSS_TRAVERSAL)
#define def_test_mss_check(D, sfx)                                        \
  static bool test_mss_check##sfx(test_mss##sfx##_t *m) {                 \
    return 1 && m->ap_check == 0xdeadbeef && m->hs_check == 0xdeadbeef && \
           m->ns_check == 0xdeadbeef && m->ss_check == 0xdeadbeef;        \
  }
#else
#define def_test_mss_check(D, sfx)                        \
  static bool test_mss_check##sfx(test_mss##sfx##_t *m) { \
    return 1 && m->mt_check == 0xdeadbeef;                \
  }
#endif

def_test_mss(1, 1);
def_test_mss(2, 2);
def_test_mss(3, 3);
def_test_mss(4, 4);
def_test_mss(5, 5);
def_test_mss(10, x);
def_test_mss(MAM2_MSS_TEST_MAX_D, );

def_test_mss_init(1, 1);
def_test_mss_init(2, 2);
def_test_mss_init(3, 3);
def_test_mss_init(4, 4);
def_test_mss_init(5, 5);
def_test_mss_init(10, x);
def_test_mss_init(MAM2_MSS_TEST_MAX_D, );

def_test_mss_check(1, 1);
def_test_mss_check(2, 2);
def_test_mss_check(3, 3);
def_test_mss_check(4, 4);
def_test_mss_check(5, 5);
def_test_mss_check(10, x);
def_test_mss_check(MAM2_MSS_TEST_MAX_D, );

static bool mss_store_test(mss_t *m, mss_t *m2, prng_t *p, spongos_t *s,
                           wots_t *w, mss_mt_height_t D) {
  bool r = true;
  retcode_t e;
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
  prng_init(p, p->sponge, K);
  trits_set_zero(N);
  trits_set_zero(H);
  trits_from_str(H,
                 "ABCNKOZWYSDF9OABCNKOZWYSDF9"
                 "ABCNKOZWYSDF9QABCNKOZWYSDF9"
                 "ABCNKOZWYSDF9CABCNKOZWYSDF9");

  for (d = 0; d <= D; ++d) {
    sig = trits_take(sig_, MAM2_MSS_SIG_SIZE(d));
    sig2 = trits_take(sig2_, MAM2_MSS_SIG_SIZE(d));

    mss_init(m, p, s->sponge, w, d, N, trits_null());
    mss_init(m2, p, s->sponge, w, d, N, trits_null());
    mss_gen(m, pk);

    do {
      mss_sign(m, H, sig);

      store = trits_take(store_, mss_stored_size(m));
      mss_save(m, store);
      e = mss_load(m2, &store);
      MAM2_ASSERT(RC_OK == e);
      mss_sign(m2, H, sig2);

      r = r && trits_cmp_eq(sig, sig2);
    } while (mss_next(m));
  }

  return r;
}

static bool mss_test(mss_t *m, prng_t *p, spongos_t *s, wots_t *w,
                     mss_mt_height_t D) {
  bool r = true;
  MAM2_TRITS_DEF0(K, MAM2_PRNG_KEY_SIZE);
  mss_mt_height_t d;
  MAM2_TRITS_DEF0(N, 24);
  MAM2_TRITS_DEF0(H, MAM2_MSS_HASH_SIZE);
  MAM2_TRITS_DEF0(pk, MAM2_MSS_PK_SIZE);
  /*MAM2_TRITS_DEF0(sig_, MAM2_MSS_SIG_SIZE(MAM2_MSS_TEST_MAX_D)); */
  trits_t sig_;

  K = MAM2_TRITS_INIT(K, MAM2_PRNG_KEY_SIZE);
  N = MAM2_TRITS_INIT(N, 24);
  H = MAM2_TRITS_INIT(H, MAM2_MSS_HASH_SIZE);
  pk = MAM2_TRITS_INIT(pk, MAM2_MSS_PK_SIZE);
  /*sig_ = MAM2_TRITS_INIT(sig_, MAM2_MSS_SIG_SIZE(MAM2_MSS_TEST_MAX_D)); */
  sig_ = trits_alloc(MAM2_MSS_SIG_SIZE(D));

  trits_from_str(K,
                 "ABCNOABCNKOZWYKOZWYSDF9SDF9"
                 "YSDF9QABCNKOZWYSDF9ABCNKOZW"
                 "SDF9CABCABCNKOZWYNKOZWYSDF9");
  prng_init(p, p->sponge, K);
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

    mss_init(m, p, s->sponge, w, d, N, trits_null());
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

  trits_free(sig_);

  return r;
}

static void mss_meta_test(void) {
  test_sponge_t _s[1];
  test_spongos_t _sg[1];
  test_prng_t _p[1];
  test_wots_t _w[1];
  test_mss1_t _m1[1];
  test_mss2_t _m2[1];
  test_mss3_t _m3[1];
  test_mss4_t _m4[1];
  test_mss4_t _m42[1];
  test_mss5_t _m5[1];
  test_mssx_t _mx[1];
  test_mss_t _m[1];

  sponge_t *spongos = test_sponge_init(_s);
  spongos_t *sg = test_spongos_init(_sg, spongos);
  prng_t *p = test_prng_init(_p, spongos);
  wots_t *w = test_wots_init(_w, spongos);
  mss_t *m1 = test_mss_init1(_m1);
  mss_t *m2 = test_mss_init2(_m2);
  mss_t *m3 = test_mss_init3(_m3);
  mss_t *m4 = test_mss_init4(_m4);
  mss_t *m42 = test_mss_init4(_m42);
  mss_t *m5 = test_mss_init5(_m5);
  mss_t *mx = test_mss_initx(_mx);
  mss_t *m = test_mss_init(_m);

  TEST_ASSERT_TRUE(mss_test(m1, p, sg, w, 1) && test_mss_check1(_m1));
  TEST_ASSERT_TRUE(mss_test(m2, p, sg, w, 2) && test_mss_check2(_m2));
  TEST_ASSERT_TRUE(mss_test(m3, p, sg, w, 3) && test_mss_check3(_m3));
  // TEST_ASSERT_TRUE(mss_test(m4, p, sg, w, 4) && test_mss_check4(_m4));
  TEST_ASSERT_TRUE(mss_store_test(m4, m42, p, sg, w, 4) &&
                   test_mss_check4(_m4) && test_mss_check4(_m42));
  // #if 0
  //   TEST_ASSERT_TRUE(mss_test(m5, p, sg, w, 5) && test_mss_check5(_m5));
  //   TEST_ASSERT_TRUE(mss_test(mx, p, sg, w, 10) && test_mss_checkx(_mx));
  // #endif
  // TEST_ASSERT_TRUE(mss_test(m, p, sg, w, MAM2_MSS_TEST_MAX_D) &&
  //                  test_mss_check(_m));
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(mss_meta_test);

  return UNITY_END();
}
