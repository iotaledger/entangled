/*
 * Copyright (c) 2018 IOTA Stiftung
 * https:github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

#include <string.h>

#include <unity/unity.h>

#include "common/trinary/trit_tryte.h"
#include "mam/v2/mam/endpoint.h"
#include "mam/v2/mam/mam.h"
#include "mam/v2/mam/mam_endpoint_t_set.h"
#include "mam/v2/test_utils/test_utils.h"

static bool mam_endpoint_t_set_cmp(mam_endpoint_t_set_t const endpoints_1,
                                   mam_endpoint_t_set_t const endpoints_2) {
  mam_endpoint_t_set_entry_t *entry_1 = NULL;
  mam_endpoint_t_set_entry_t *tmp_1 = NULL;
  mam_endpoint_t_set_entry_t *entry_2 = NULL;
  mam_endpoint_t_set_entry_t *tmp_2 = NULL;

  if (mam_endpoint_t_set_size(endpoints_1) !=
      mam_endpoint_t_set_size(endpoints_2)) {
    return false;
  }

  HASH_ITER(hh, endpoints_1, entry_1, tmp_1) {
    HASH_ITER(hh, endpoints_2, entry_2, tmp_2) {}
  }

  return true;
}

void test_endpoint(void) {
  mam_endpoint_t_set_t endpoints_1 = NULL;
  mam_endpoint_t_set_t endpoints_2 = NULL;

  test_sponge_t test_sponge;
  sponge_t *sponge = test_sponge_init(&test_sponge);
  test_prng_t test_prng;
  prng_t *prng = test_prng_init(&test_prng, sponge);
  tryte_t channel_name[27];
  trits_t channel_name_trits = trits_alloc(81);
  tryte_t endpoint_name[27];
  trits_t endpoint_name_trits = trits_alloc(81);
  mam_ialloc_t allocator = {.create_sponge = test_create_sponge,
                            .destroy_sponge = test_delete_sponge};

  MAM2_TRITS_DEF0(prng_key, MAM2_PRNG_KEY_SIZE);
  prng_key = MAM2_TRITS_INIT(prng_key, MAM2_PRNG_KEY_SIZE);
  trits_from_str(prng_key,
                 "NOPQRSTUVWXYZ9ABCDEFGHIJKLM"
                 "NOPQRSTUVWXYZ9ABCDEFGHIJKLM"
                 "NOPQRSTUVWXYZ9ABCDEFGHIJKLM");

  sponge_init(sponge);
  prng_init(prng, prng->sponge, prng_key);

  for (size_t i = 1; i < 5; i++) {
    mam_endpoint_t endpoint;

    memset(channel_name, 'A' + 2 * i, 27);
    trytes_to_trits(channel_name, channel_name_trits.p, 27);
    memset(endpoint_name, 'A' + 2 * i + 1, 27);
    trytes_to_trits(endpoint_name, endpoint_name_trits.p, 27);
    mam_endpoint_create(&allocator, prng, i, channel_name_trits,
                        endpoint_name_trits, &endpoint);
    TEST_ASSERT(mam_endpoint_t_set_add(&endpoints_1, &endpoint) == RC_OK);
  }

  size_t size = mam_endpoints_serialized_size(endpoints_1);

  trits_t trits = trits_alloc(size);
  trits_t cpy = trits;

  TEST_ASSERT(mam_endpoints_serialize(endpoints_1, &trits) == RC_OK);

  TEST_ASSERT(mam_endpoints_deserialize(&cpy, &endpoints_2) == RC_OK);

  TEST_ASSERT_TRUE(mam_endpoint_t_set_cmp(endpoints_1, endpoints_2));

  trits_free(trits);
  trits_free(channel_name_trits);
  trits_free(endpoint_name_trits);
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_endpoint);

  return UNITY_END();
}
