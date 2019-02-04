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
#define def_test_mss(D, sfx)                           \
  typedef struct test_mss##sfx##_s {                   \
    mss_t m;                                           \
    trit_t auth_path[MAM2_MSS_MT_AUTH_WORDS(D)];       \
    uint32_t ap_check;                                 \
    trit_t nodes_hashes[MAM2_MSS_MT_HASH_WORDS(D, 1)]; \
    uint32_t hs_check;                                 \
    mss_mt_node_t nodes[MAM2_MSS_MT_NODES(D) + 1];     \
    uint32_t ns_check;                                 \
    mss_mt_stack_t stacks[MAM2_MSS_MT_STACKS(D)];      \
    uint32_t ss_check;                                 \
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
    m->m.auth_path = m->auth_path;                         \
    m->ap_check = 0xdeadbeef;                              \
    m->m.nodes_hashes = m->nodes_hashes;                   \
    m->hs_check = 0xdeadbeef;                              \
    m->m.nodes = m->nodes;                                 \
    m->ns_check = 0xdeadbeef;                              \
    m->m.stacks = m->stacks;                               \
    m->ss_check = 0xdeadbeef;                              \
    return &m->m;                                          \
  }
#else
#define def_test_mss_init(D, sfx)                          \
  static mss_t *test_mss_init##sfx(test_mss##sfx##_t *m) { \
    m->m.mt = m->mt;                                       \
    m->mt_check = 0xdeadbeef;                              \
    return &m->m;                                          \
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
// def_test_mss(5, 5);
// def_test_mss(10, x);
// def_test_mss(MAM2_MSS_TEST_MAX_D, );

def_test_mss_init(1, 1);
def_test_mss_init(2, 2);
def_test_mss_init(3, 3);
def_test_mss_init(4, 4);
// def_test_mss_init(5, 5);
// def_test_mss_init(10, x);
// def_test_mss_init(MAM2_MSS_TEST_MAX_D, );

def_test_mss_check(1, 1);
def_test_mss_check(2, 2);
def_test_mss_check(3, 3);
def_test_mss_check(4, 4);
// def_test_mss_check(5, 5);
// def_test_mss_check(10, x);
// def_test_mss_check(MAM2_MSS_TEST_MAX_D, );

static bool mss_store_test(mss_t *mss1, mss_t *mss2, mam_prng_t *prng,
                           mam_spongos_t *spongos, mam_wots_t *wots,
                           mss_mt_height_t max_height) {
  bool r = true;
  retcode_t e;
  mss_mt_height_t curr_height;
  MAM2_TRITS_DEF0(key, MAM2_PRNG_KEY_SIZE);
  MAM2_TRITS_DEF0(nonce, 24);
  MAM2_TRITS_DEF0(hash, MAM2_MSS_HASH_SIZE);
  MAM2_TRITS_DEF0(pk, MAM2_MSS_PK_SIZE);
  MAM2_TRITS_DEF0(pk2, MAM2_MSS_PK_SIZE);
  MAM2_TRITS_DEF0(sig_, MAM2_MSS_SIG_SIZE(5));
  MAM2_TRITS_DEF0(sig2_, MAM2_MSS_SIG_SIZE(5));
  MAM2_TRITS_DEF0(store_, MAM2_MSS_MAX_STORED_SIZE(5));
  trits_t sig, sig2, store;

  key = MAM2_TRITS_INIT(key, MAM2_PRNG_KEY_SIZE);
  nonce = MAM2_TRITS_INIT(nonce, 24);
  hash = MAM2_TRITS_INIT(hash, MAM2_MSS_HASH_SIZE);
  pk = MAM2_TRITS_INIT(pk, MAM2_MSS_PK_SIZE);
  pk2 = MAM2_TRITS_INIT(pk2, MAM2_MSS_PK_SIZE);
  sig_ = MAM2_TRITS_INIT(sig_, MAM2_MSS_SIG_SIZE(5));
  sig2_ = MAM2_TRITS_INIT(sig2_, MAM2_MSS_SIG_SIZE(5));
  store_ = MAM2_TRITS_INIT(store_, MAM2_MSS_MAX_STORED_SIZE(5));

  MAM2_ASSERT(max_height <= 5);

  trits_from_str(key,
                 "ABCNOABCNKOZWYKOZWYSDF9SDF9"
                 "YSDF9QABCNKOZWYSDF9ABCNKOZW"
                 "SDF9CABCABCNKOZWYNKOZWYSDF9");
  mam_prng_init(prng, key);
  trits_set_zero(nonce);
  trits_set_zero(hash);
  trits_from_str(hash,
                 "ABCNKOZWYSDF9OABCNKOZWYSDF9"
                 "ABCNKOZWYSDF9QABCNKOZWYSDF9"
                 "ABCNKOZWYSDF9CABCNKOZWYSDF9");

  for (curr_height = 0; curr_height <= max_height; ++curr_height) {
    sig = trits_take(sig_, MAM2_MSS_SIG_SIZE(curr_height));
    sig2 = trits_take(sig2_, MAM2_MSS_SIG_SIZE(curr_height));

    mss_init(mss1, prng, curr_height, nonce, trits_null());
    mss_init(mss2, prng, curr_height, nonce, trits_null());
    mss_gen(mss1, pk);

    do {
      mss_sign(mss1, hash, sig);

      store = trits_take(store_, mss_stored_size(mss1));
      mss_save(mss1, store);
      e = mss_load(mss2, &store);
      MAM2_ASSERT(RC_OK == e);
      mss_sign(mss2, hash, sig2);

      r = r && trits_cmp_eq(sig, sig2);
    } while (mss_next(mss1));
  }

  return r;
}

static bool mss_test(mss_t *mss, mam_prng_t *prng, mam_spongos_t *spongos,
                     mam_wots_t *wots, mss_mt_height_t max_height) {
  bool r = true;
  MAM2_TRITS_DEF0(key, MAM2_PRNG_KEY_SIZE);
  mss_mt_height_t curr_height;
  MAM2_TRITS_DEF0(nonce, 24);
  MAM2_TRITS_DEF0(hash, MAM2_MSS_HASH_SIZE);
  MAM2_TRITS_DEF0(pk, MAM2_MSS_PK_SIZE);
  /*MAM2_TRITS_DEF0(sig_, MAM2_MSS_SIG_SIZE(MAM2_MSS_TEST_MAX_D)); */
  trits_t sig_;

  key = MAM2_TRITS_INIT(key, MAM2_PRNG_KEY_SIZE);
  nonce = MAM2_TRITS_INIT(nonce, 24);
  hash = MAM2_TRITS_INIT(hash, MAM2_MSS_HASH_SIZE);
  pk = MAM2_TRITS_INIT(pk, MAM2_MSS_PK_SIZE);
  /*sig_ = MAM2_TRITS_INIT(sig_, MAM2_MSS_SIG_SIZE(MAM2_MSS_TEST_MAX_D)); */
  sig_ = trits_alloc(MAM2_MSS_SIG_SIZE(max_height));

  trits_from_str(key,
                 "ABCNOABCNKOZWYKOZWYSDF9SDF9"
                 "YSDF9QABCNKOZWYSDF9ABCNKOZW"
                 "SDF9CABCABCNKOZWYNKOZWYSDF9");
  mam_prng_init(prng, key);
  trits_set_zero(nonce);
  trits_set_zero(hash);
  trits_from_str(hash,
                 "ABCNKOZWYSDF9OABCNKOZWYSDF9"
                 "ABCNKOZWYSDF9QABCNKOZWYSDF9"
                 "ABCNKOZWYSDF9CABCNKOZWYSDF9");

  for (curr_height = 1; r && curr_height <= max_height; ++curr_height) {
    trits_t sig = trits_take(sig_, MAM2_MSS_SIG_SIZE(curr_height));
    trits_t sig_skn = trits_take(sig, MAM2_MSS_SKN_SIZE);
#if !defined(MAM2_MSS_DEBUG)
    trits_t sig_wots =
        trits_take(trits_drop(sig, MAM2_MSS_SKN_SIZE), MAM2_WOTS_SIG_SIZE);
    trits_t sig_apath = trits_drop(sig, MAM2_MSS_SKN_SIZE + MAM2_WOTS_SIG_SIZE);
#endif

    dbg_printf("========================\ncurr_height = %d\n", curr_height);

    mss_init(mss, prng, curr_height, nonce, trits_null());

    mss_gen(mss, pk);

    dbg_printf("mss pk \t");
    trits_dbg_print(pk);
    dbg_printf("\n");

    do {
      dbg_printf("------------------------\nskn = %d\n", mss->skn);
      mss_sign(mss, hash, sig);
      r = r && mss_verify(spongos, &wots->spongos, hash, sig, pk);

#if !defined(MAM2_MSS_DEBUG)
      /* H is ignored, makes no sense to modify and check */
      trits_put1(hash, trit_add(trits_get1(hash), 1));
      r = r && !mss_verify(spongos, &wots->spongos, hash, sig, pk);
      trits_put1(hash, trit_sub(trits_get1(hash), 1));
#endif

      trits_put1(sig_skn, trit_add(trits_get1(sig_skn), 1));
      r = r && !mss_verify(spongos, &wots->spongos, hash, sig, pk);
      trits_put1(sig_skn, trit_sub(trits_get1(sig_skn), 1));

#if !defined(MAM2_MSS_DEBUG)
      /* WOTS sig is ignored, makes no sense to modify and check */
      trits_put1(sig_wots, trit_add(trits_get1(sig_wots), 1));
      r = r && !mss_verify(spongos, &wots->spongos, hash, sig, pk);
      trits_put1(sig_wots, trit_sub(trits_get1(sig_wots), 1));

      if (!trits_is_empty(sig_apath)) {
        trits_put1(sig_apath, trit_add(trits_get1(sig_apath), 1));
        r = r && !mss_verify(spongos, &wots->spongos, hash, sig, pk);
        trits_put1(sig_apath, trit_sub(trits_get1(sig_apath), 1));
      }
#endif

      r = r && !mss_verify(spongos, &wots->spongos, hash,
                           trits_take(sig, trits_size(sig) - 1), pk);

      trits_put1(pk, trit_add(trits_get1(pk), 1));
      r = r && !mss_verify(spongos, &wots->spongos, hash, sig, pk);
      trits_put1(pk, trit_sub(trits_get1(pk), 1));

    } while (mss_next(mss));

#if defined(MAM2_MSS_DEBUG)
    dbg_printf("\nd = %d\n", mss->height);
    dbg_printf("  leafs = %d\n", mss->gen_leaf_count);
    dbg_printf("  nodes = %d\n", mss->hash_node_count);
#endif
  }

  trits_free(sig_);

  return r;
}

static void mss_meta_test(void) {
  test_mam_spongos_t _sg[1];
  test_mss1_t _m1[1];
  test_mss2_t _m2[1];
  test_mss3_t _m3[1];
  test_mss4_t _m4[1];
  test_mss4_t _m42[1];
  // test_mss5_t _m5[1];
  // test_mssx_t _mx[1];
  // test_mss_t _m[1];

  mam_sponge_t sponge;
  mam_spongos_t *sg = test_mam_spongos_init(_sg, &sponge);

  mam_prng_t p;
  mam_wots_t w;
  mss_t *m1 = test_mss_init1(_m1);
  mss_t *m2 = test_mss_init2(_m2);
  mss_t *m3 = test_mss_init3(_m3);
  mss_t *m4 = test_mss_init4(_m4);
  mss_t *m42 = test_mss_init4(_m42);
  // mss_t *m5 = test_mss_init5(_m5);
  // mss_t *mx = test_mss_initx(_mx);
  // mss_t *m = test_mss_init(_m);

  mam_wots_init(&w);

  TEST_ASSERT_TRUE(mss_test(m1, &p, sg, &w, 1) && test_mss_check1(_m1));
  TEST_ASSERT_TRUE(mss_test(m2, &p, sg, &w, 2) && test_mss_check2(_m2));
  TEST_ASSERT_TRUE(mss_test(m3, &p, sg, &w, 3) && test_mss_check3(_m3));
  // TEST_ASSERT_TRUE(mss_test(m4, p, sg, w, 4) && test_mss_check4(_m4));
  TEST_ASSERT_TRUE(mss_store_test(m4, m42, &p, sg, &w, 4) &&
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
