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

#include "mam/v2/wots/wots.h"

bool_t wots_test(wots_t *w, prng_t *p) {
  bool_t r = 1;
  MAM2_TRITS_DEF0(N, 18);
  MAM2_TRITS_DEF0(pk, MAM2_WOTS_PK_SIZE);
  MAM2_TRITS_DEF0(H, MAM2_WOTS_HASH_SIZE);
  MAM2_TRITS_DEF0(sig, MAM2_WOTS_SIG_SIZE);
  /*MAM2_TRITS_DEF0(pkr, MAM2_WOTS_PK_SIZE);*/
  N = MAM2_TRITS_INIT(N, 18);
  pk = MAM2_TRITS_INIT(pk, MAM2_WOTS_PK_SIZE);
  H = MAM2_TRITS_INIT(H, MAM2_WOTS_HASH_SIZE);
  sig = MAM2_TRITS_INIT(sig, MAM2_WOTS_SIG_SIZE);
  /*pkr = MAM2_TRITS_INIT(pkr, MAM2_WOTS_PK_SIZE);*/

  trits_set_zero(N);
  trits_set_zero(H);
  trits_set_zero(wots_sk_trits(w));

  prng_gen(p, 7, N, H);
  wots_gen_sk(w, p, N);
  wots_calc_pk(w, pk);
  wots_sign(w, H, sig);
  /*wots_recover(w->sg, H, sig, pkr);*/
  r = r && wots_verify(w->sg, H, sig, pk);

  trits_put1(H, trit_add(trits_get1(H), 1));
  r = r && !wots_verify(w->sg, H, sig, pk);
  trits_put1(H, trit_sub(trits_get1(H), 1));

  trits_put1(sig, trit_add(trits_get1(sig), 1));
  r = r && !wots_verify(w->sg, H, sig, pk);
  trits_put1(sig, trit_sub(trits_get1(sig), 1));

  trits_put1(pk, trit_add(trits_get1(pk), 1));
  r = r && !wots_verify(w->sg, H, sig, pk);
  trits_put1(pk, trit_sub(trits_get1(pk), 1));

  return r;
}

int main(void) {
  UNITY_BEGIN();

  return UNITY_END();
}
