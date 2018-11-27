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

#include "mam/v2/mss.h"
#include "mam/v2/prng.h"
#include "mam/v2/sponge.h"
#include "mam/v2/tests/common.h"
#include "utils/macros.h"

void mss_test_do(mss_t *m, prng_t *p, sponge_t *s, wots_t *w,
                 mss_mt_height_t mt_height) {
  bool r = true;
  flex_trit_t key[FLEX_TRIT_SIZE_243];
  mss_mt_height_t d;

  TRIT_ARRAY_DECLARE(sig, MAM2_MSS_SIG_SIZE(mt_height));
  tryte_t const *const key_trytes =
      "ABCNOABCNKOZWYKOZWYSDF9SDF9YSDF9QABCNKOZWYSDF9ABCNKOZWSDF9CABCABCNKOZWYN"
      "KOZWYSDF9";

  flex_trits_from_trytes(key, MAM2_PRNG_KEY_SIZE, key_trytes, HASH_LENGTH_TRYTE,
                         HASH_LENGTH_TRYTE);
  prng_init(p, p->sponge, key);
  TRIT_ARRAY_DECLARE(nonce, 24);
  TRIT_ARRAY_DECLARE(nonce_null, 0);

  const char *h_str =
      "ABCNKOZWYSDF9OABCNKOZWYSDF9"
      "ABCNKOZWYSDF9QABCNKOZWYSDF9"
      "ABCNKOZWYSDF9CABCNKOZWYSDF9";

  TRIT_ARRAY_DECLARE(hash_array, MAM2_WOTS_HASH_SIZE);
  flex_trits_from_trytes(hash_array.trits, MAM2_WOTS_HASH_SIZE, h_str,
                         MAM2_WOTS_HASH_SIZE, strlen(h_str));

  for (d = 1; r && d <= mt_height; ++d) {
    TRIT_ARRAY_DECLARE(curr_sig, MAM2_MSS_SIG_SIZE(d));
    trit_array_insert_from_pos(&curr_sig, &sig, 0, 0, MAM2_MSS_SIG_SIZE(d));
    trit_t tmp[MAM2_MSS_SIG_SIZE(d)];
    flex_trits_to_trits(tmp, MAM2_MSS_SIG_SIZE(d), curr_sig.trits,
                        MAM2_MSS_SIG_SIZE(d), MAM2_MSS_SIG_SIZE(d));

    TRIT_ARRAY_DECLARE(curr_sig_skn, MAM2_MSS_SKN_SIZE);
    trit_array_insert_from_pos(&curr_sig_skn, &sig, 0, 0, MAM2_MSS_SKN_SIZE);
    TRIT_ARRAY_DECLARE(sig_wots, MAM2_WOTS_SIG_SIZE);
    trit_array_insert_from_pos(&sig_wots, &curr_sig, MAM2_MSS_SKN_SIZE, 0,
                               MAM2_WOTS_SIG_SIZE);
    TRIT_ARRAY_DECLARE(
        sig_apath, curr_sig.num_trits - MAM2_WOTS_SIG_SIZE - MAM2_MSS_SKN_SIZE);
    trit_array_insert_from_pos(
        &sig_apath, &curr_sig, MAM2_MSS_SKN_SIZE + MAM2_WOTS_SIG_SIZE, 0,
        curr_sig.num_trits - MAM2_WOTS_SIG_SIZE - MAM2_MSS_SKN_SIZE);

    dbg_printf("========================\nmt_height = %d\n", d);

    mss_init(m, p, s, w, d, &nonce, &nonce_null);
    TRIT_ARRAY_DECLARE(pk, MAM2_WOTS_PK_SIZE);
    mss_gen(m, &pk);

    dbg_printf("mss pk \t");
    // trits_dbg_print(pk);
    dbg_printf("\n");

    do {
      dbg_printf("------------------------\nskn = %d\n", m->skn);
      mss_sign(m, &hash_array, &curr_sig);
      TEST_ASSERT(mss_verify(s, w->sponge, &hash_array, &curr_sig, &pk));

      // H is ignored, makes no sense to modify and check
      trit_t modified_trit = trit_array_at(&hash_array, 0);
      trit_array_set_at(&hash_array, 0, trit_sum(modified_trit, 1));
      TEST_ASSERT(!mss_verify(s, w->sponge, &hash_array, &curr_sig, &pk));
      trit_array_set_at(&hash_array, 0, modified_trit);

      modified_trit = trit_array_at(&curr_sig, 0);
      trit_array_set_at(&curr_sig, 0, trit_sum(modified_trit, 1));
      TEST_ASSERT(!mss_verify(s, w->sponge, &hash_array, &curr_sig, &pk));
      trit_array_set_at(&curr_sig, 0, modified_trit);

      // WOTS sig is ignored, makes no sense to modify and check
      modified_trit = trit_array_at(&curr_sig, MAM2_MSS_SKN_SIZE);
      trit_array_set_at(&curr_sig, MAM2_MSS_SKN_SIZE,
                        trit_sum(modified_trit, 1));
      TEST_ASSERT(!mss_verify(s, w->sponge, &hash_array, &curr_sig, &pk));
      trit_array_set_at(&curr_sig, MAM2_MSS_SKN_SIZE, modified_trit);

      if (sig_apath.num_trits > 0) {
        modified_trit =
            trit_array_at(&curr_sig, MAM2_MSS_SKN_SIZE + MAM2_WOTS_SIG_SIZE);
        trit_array_set_at(&curr_sig, MAM2_MSS_SKN_SIZE,
                          trit_sum(modified_trit, 1));
        TEST_ASSERT(!mss_verify(s, w->sponge, &hash_array, &curr_sig, &pk));
        trit_array_set_at(&curr_sig, MAM2_MSS_SKN_SIZE + MAM2_WOTS_SIG_SIZE,
                          modified_trit);
      }

      modified_trit = trit_array_at(&pk, 0);
      trit_array_set_at(&pk, 0, trit_sum(modified_trit, 1));
      TEST_ASSERT(!mss_verify(s, w->sponge, &hash_array, &curr_sig, &pk));
      trit_array_set_at(&pk, 0, modified_trit);

    } while (mss_next(m));
  }
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
