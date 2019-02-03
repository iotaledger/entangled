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
#include "mam/v2/test_utils/test_utils.h"

static void spongos_test(void) {
  test_mam_sponge_t test_sponge;
  test_spongos_t test_spongos;
  mam_sponge_t *sponge = test_sponge_init(&test_sponge);
  spongos_t *spongos = test_spongos_init(&test_spongos, sponge);

  MAM2_TRITS_DEF0(x, 243);
  MAM2_TRITS_DEF0(y, 243);
  MAM2_TRITS_DEF0(z, 243);
  x = MAM2_TRITS_INIT(x, 243);
  y = MAM2_TRITS_INIT(y, 243);
  z = MAM2_TRITS_INIT(z, 243);

  trits_set_zero(x);

  spongos_init(spongos);
  spongos_absorb(spongos, x);
  spongos_squeeze(spongos, y);

  spongos_init(spongos);
  spongos_absorb(spongos, x);
  spongos_commit(spongos);
  spongos_encr(spongos, x, z);

  TEST_ASSERT_TRUE(trits_cmp_eq(y, z));

  spongos_init(spongos);
  spongos_absorb(spongos, x);
  spongos_commit(spongos);
  spongos_decr(spongos, z, z);

  TEST_ASSERT_TRUE(trits_cmp_eq(x, z));
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(spongos_test);

  return UNITY_END();
}
