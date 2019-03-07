/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

#include "mam/test_utils/test_utils.h"

void prng_gen_str(mam_prng_t *p, trint3_t d, char const *nonce, trits_t Y) {
  size_t n;
  MAM_TRITS_DEF0(N, MAM_SPONGE_RATE);
  N = MAM_TRITS_INIT(N, MAM_SPONGE_RATE);

  n = strlen(nonce) * 3;
  N = trits_take_min(N, n);
  trits_from_str(N, nonce);

  mam_prng_gen(p, d, N, Y);
}
