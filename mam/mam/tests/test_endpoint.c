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
#include "mam/mam/endpoint.h"
#include "mam/mam/mam_endpoint_t_set.h"

#define CHANNEL_NAME_SIZE 27
#define ENDPOINT_NAME_SIZE 27

static bool mam_endpoint_t_set_cmp_test_endpoints(
    mam_endpoint_t_set_t const endpoints_1,
    mam_endpoint_t_set_t const endpoints_2) {
  mam_endpoint_t_set_entry_t *entry_1 = NULL;
  mam_endpoint_t_set_entry_t *tmp_1 = NULL;
  mam_endpoint_t_set_entry_t *entry_2 = NULL;
  mam_endpoint_t_set_entry_t *tmp_2 = NULL;
  size_t match = 0;
  MAM_TRITS_DEF0(hash, MAM_MSS_HASH_SIZE);
  hash = MAM_TRITS_INIT(hash, MAM_MSS_HASH_SIZE);
  trits_from_str(hash,
                 "ABCNKOZWYSDF9OABCNKOZWYSDF9"
                 "ABCNKOZWYSDF9QABCNKOZWYSDF9"
                 "ABCNKOZWYSDF9CABCNKOZWYSDF9");

  if (mam_endpoint_t_set_size(endpoints_1) !=
      mam_endpoint_t_set_size(endpoints_2)) {
    return false;
  }

  HASH_ITER(hh, endpoints_1, entry_1, tmp_1) {
    HASH_ITER(hh, endpoints_2, entry_2, tmp_2) {
      if (memcmp(entry_1->value.mss.root, entry_2->value.mss.root,
                 MAM_ENDPOINT_ID_SIZE) == 0 &&
          trits_cmp_eq(entry_1->value.name, entry_2->value.name)) {
        MAM_TRITS_DEF0(sig1, MAM_MSS_SIG_SIZE(entry_1->value.mss.height));
        MAM_TRITS_DEF0(sig2, MAM_MSS_SIG_SIZE(entry_2->value.mss.height));
        sig1 =
            MAM_TRITS_INIT(sig1, MAM_MSS_SIG_SIZE(entry_1->value.mss.height));
        sig2 =
            MAM_TRITS_INIT(sig2, MAM_MSS_SIG_SIZE(entry_2->value.mss.height));
        TEST_ASSERT_EQUAL_INT(RC_OK,
                              mam_mss_sign(&entry_1->value.mss, hash, sig1));
        TEST_ASSERT_EQUAL_INT(RC_OK,
                              mam_mss_sign(&entry_2->value.mss, hash, sig2));
        if (trits_cmp_eq(sig1, sig2)) {
          match++;
          break;
        }
      }
    }
  }

  return match == mam_endpoint_t_set_size(endpoints_1);
}

void test_endpoint(void) {
  mam_endpoint_t endpoint;
  mam_endpoint_t_set_t endpoints_1 = NULL;
  mam_endpoint_t_set_t endpoints_2 = NULL;

  mam_sponge_t sponge;
  mam_prng_t prng;
  tryte_t channel_name[CHANNEL_NAME_SIZE];
  trits_t channel_name_trits = trits_alloc(CHANNEL_NAME_SIZE * 3);
  tryte_t endpoint_name[ENDPOINT_NAME_SIZE];
  trits_t endpoint_name_trits = trits_alloc(ENDPOINT_NAME_SIZE * 3);

  MAM_TRITS_DEF0(prng_key, MAM_PRNG_KEY_SIZE);
  prng_key = MAM_TRITS_INIT(prng_key, MAM_PRNG_KEY_SIZE);
  trits_from_str(prng_key,
                 "NOPQRSTUVWXYZ9ABCDEFGHIJKLM"
                 "NOPQRSTUVWXYZ9ABCDEFGHIJKLM"
                 "NOPQRSTUVWXYZ9ABCDEFGHIJKLM");

  mam_sponge_init(&sponge);
  mam_prng_init(&prng, prng_key);

  for (size_t i = 1; i < 5; i++) {
    memset(channel_name, 'A' + 2 * i, CHANNEL_NAME_SIZE);
    trytes_to_trits(channel_name, channel_name_trits.p, CHANNEL_NAME_SIZE);
    memset(endpoint_name, 'A' + 2 * i + 1, ENDPOINT_NAME_SIZE);
    trytes_to_trits(endpoint_name, endpoint_name_trits.p, ENDPOINT_NAME_SIZE);
    TEST_ASSERT(mam_endpoint_create(&prng, i, channel_name_trits,
                                    endpoint_name_trits, &endpoint) == RC_OK);
    TEST_ASSERT(mam_endpoint_t_set_add(&endpoints_1, &endpoint) == RC_OK);
  }

  size_t size = mam_endpoints_serialized_size(endpoints_1);

  trits_t trits = trits_alloc(size);
  trits_t cpy = trits;

  mam_endpoints_serialize(endpoints_1, &trits);

  TEST_ASSERT(mam_endpoints_deserialize(&cpy, channel_name_trits, &prng,
                                        &endpoints_2) == RC_OK);

  TEST_ASSERT_TRUE(
      mam_endpoint_t_set_cmp_test_endpoints(endpoints_1, endpoints_2));

  mam_endpoints_destroy(&endpoints_1);
  mam_endpoints_destroy(&endpoints_2);
  trits_free(trits);
  trits_free(channel_name_trits);
  trits_free(endpoint_name_trits);
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_endpoint);

  return UNITY_END();
}
