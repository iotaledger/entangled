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

#include "mam/v2/sponge/spongos.h"

bool_t spongos_test(ispongos *s) {
  bool_t r = 1;
  MAM2_TRITS_DEF0(x, 243);
  MAM2_TRITS_DEF0(y, 243);
  MAM2_TRITS_DEF0(z, 243);
  x = MAM2_TRITS_INIT(x, 243);
  y = MAM2_TRITS_INIT(y, 243);
  z = MAM2_TRITS_INIT(z, 243);

  trits_set_zero(x);

  spongos_init(s);
  spongos_absorb(s, x);
  spongos_squeeze(s, y);

  spongos_init(s);
  spongos_absorb(s, x);
  spongos_commit(s);
  spongos_encr(s, x, z);

  r = r && trits_cmp_eq(y, z);

  spongos_init(s);
  spongos_absorb(s, x);
  spongos_commit(s);
  spongos_decr(s, z, z);

  r = r && trits_cmp_eq(x, z);

  return r;
}

int main(void) {
  UNITY_BEGIN();

  return UNITY_END();
}
