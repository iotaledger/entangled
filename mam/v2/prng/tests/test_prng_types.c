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
#include "mam/v2/prng/mam_prng_t_set.h"
#include "mam/v2/prng/prng.h"
#include "mam/v2/prng/prng_types.h"

static bool mam_prng_t_set_cmp(mam_prng_t_set_t const set1,
                               mam_prng_t_set_t const set2) {
  mam_prng_t_set_entry_t *entry1 = NULL;
  mam_prng_t_set_entry_t *tmp1 = NULL;

  mam_prng_t_set_entry_t *entry2 = NULL;
  mam_prng_t_set_entry_t *tmp2 = NULL;

  size_t equal_elements_count = 0;

  TEST_ASSERT_EQUAL_INT(mam_prng_t_set_size(set1), mam_prng_t_set_size(set2));

  // We have to loop in O(N^2) because mam_ntru_sk_t_set_contains compares hash
  // value based on
  // the address assigned into the "f" field in the ntru_sk_t type
  HASH_ITER(hh, set1, entry1, tmp1) {
    HASH_ITER(hh, set2, entry2, tmp2) {
      if (!memcmp(&entry1->value.secret_key, &entry2->value.secret_key,
                  mam_prng_serialized_size())) {
        ++equal_elements_count;
      }
    }
  }

  return mam_prng_t_set_size(set1) == equal_elements_count;
}

static void test_prng_serialization(void) {
  mam_prng_t_set_t set1 = NULL;
  mam_prng_t_set_t set2 = NULL;

  tryte_t secret_key_str[MAM_PRNG_KEY_SIZE / NUMBER_OF_TRITS_IN_A_TRYTE];

  mam_prng_t prngs[26];

  for (size_t i = 0; i < 26; i++) {
    memset(secret_key_str, 'A' + i,
           MAM_PRNG_KEY_SIZE / NUMBER_OF_TRITS_IN_A_TRYTE);
    trytes_to_trits(secret_key_str, prngs[i].secret_key,
                    MAM_PRNG_KEY_SIZE / NUMBER_OF_TRITS_IN_A_TRYTE);
    TEST_ASSERT(mam_prng_t_set_add(&set1, &prngs[i]) == RC_OK);
  }

  size_t size = mam_prngs_serialized_size(set1);

  TEST_ASSERT_EQUAL_INT(size, 26 * (MAM_PRNG_KEY_SIZE));

  trits_t trits = trits_alloc(size);

  TEST_ASSERT(mam_prngs_serialize(set1, trits) == RC_OK);

  TEST_ASSERT(mam_prngs_deserialize(trits, &set2) == RC_OK);

  TEST_ASSERT_TRUE(mam_prng_t_set_cmp(set1, set2));

  mam_prng_t_set_free(&set1);
  mam_prng_t_set_free(&set2);

  trits_free(trits);
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_prng_serialization);

  return UNITY_END();
}
