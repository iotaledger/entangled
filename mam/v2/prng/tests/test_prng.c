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

#include "mam/v2/prng/prng.h"

bool_t prng_test(iprng *p) {
  bool_t r = 1;
  MAM2_TRITS_DEF0(K, MAM2_PRNG_KEY_SIZE);
  MAM2_TRITS_DEF0(N, 18);
  MAM2_TRITS_DEF0(Y1, 243 * 2 + 18);
  MAM2_TRITS_DEF0(Y2, 243 * 2 + 18);
  K = MAM2_TRITS_INIT(K, MAM2_PRNG_KEY_SIZE);
  N = MAM2_TRITS_INIT(N, 18);
  Y1 = MAM2_TRITS_INIT(Y1, 243 * 2 + 18);
  Y2 = MAM2_TRITS_INIT(Y2, 243 * 2 + 18);

  /* init K */
  trits_set_zero(K);
  trits_from_str(K,
                 "NOPQRSTUVWXYZ9ABCDEFGHIJKLM"
                 "NOPQRSTUVWXYZ9ABCDEFGHIJKLM"
                 "NOPQRSTUVWXYZ9ABCDEFGHIJKLM");
  sponge_init(p->s);
  sponge_absorb(p->s, MAM2_SPONGE_CTL_KEY, K);
  sponge_squeeze(p->s, MAM2_SPONGE_CTL_KEY, K);
  /* init N */
  trits_set_zero(N);

  prng_init(p, p->s, K);
  prng_gen(p, 0, N, Y1);
  prng_gen(p, 1, N, Y2);
  r = r && !trits_cmp_eq(Y1, Y2);

  return r;
}

int main(void) {
  UNITY_BEGIN();

  return UNITY_END();
}
