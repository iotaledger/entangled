/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

#include <string.h>

#include <unity/unity.h>

#include "mam/v2/sponge/spongos.h"
#include "mam/v2/sponge/spongos_types.h"
#include "mam/v2/test_utils/test_utils.h"

static void mam_spongos_test(void) {
  test_mam_spongos_t test_spongos;
  mam_sponge_t sponge;
  mam_spongos_t *spongos = test_mam_spongos_init(&test_spongos, &sponge);
  mam_spongos_t deserialized_spongos;

  mam_sponge_init(&sponge);

  MAM2_TRITS_DEF0(x, 243);
  MAM2_TRITS_DEF0(y, 243);
  MAM2_TRITS_DEF0(z, 243);
  x = MAM2_TRITS_INIT(x, 243);
  y = MAM2_TRITS_INIT(y, 243);
  z = MAM2_TRITS_INIT(z, 243);

  trits_set_zero(x);

  mam_spongos_init(spongos);
  mam_spongos_absorb(spongos, x);
  mam_spongos_squeeze(spongos, y);

  mam_spongos_init(spongos);
  mam_spongos_absorb(spongos, x);
  mam_spongos_commit(spongos);
  mam_spongos_encr(spongos, x, z);

  TEST_ASSERT_TRUE(trits_cmp_eq(y, z));

  mam_spongos_init(spongos);
  mam_spongos_absorb(spongos, x);
  mam_spongos_commit(spongos);
  mam_spongos_decr(spongos, z, z);
  MAM2_TRITS_DEF0(spongos_trits, mam_spongos_serialized_size(spongos));
  spongos_trits =
      MAM2_TRITS_INIT(spongos_trits, mam_spongos_serialized_size(spongos));
  mam_spongos_serialize(spongos, spongos_trits);
  memset(deserialized_spongos.sponge.stack, 0, MAM2_SPONGE_WIDTH);
  memset(deserialized_spongos.sponge.state, 0, MAM2_SPONGE_WIDTH);
  TEST_ASSERT_EQUAL(
      RC_OK, mam_spongos_deserialize(&spongos_trits, &deserialized_spongos));

  TEST_ASSERT_EQUAL_INT(spongos->pos, deserialized_spongos.pos);
  TEST_ASSERT_EQUAL_MEMORY(spongos->sponge.state,
                           deserialized_spongos.sponge.state,
                           MAM2_SPONGE_WIDTH);

  TEST_ASSERT_TRUE(trits_cmp_eq(x, z));
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(mam_spongos_test);

  return UNITY_END();
}
