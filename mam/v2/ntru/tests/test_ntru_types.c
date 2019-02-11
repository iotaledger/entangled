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
#include "mam/v2/ntru/mam_ntru_pk_t_set.h"
#include "mam/v2/ntru/mam_ntru_sk_t_set.h"
#include "mam/v2/ntru/ntru.h"
#include "mam/v2/ntru/ntru_types.h"

static bool mam_ntru_pk_t_set_cmp(mam_ntru_pk_t_set_t const ntru_pk_set_1,
                                  mam_ntru_pk_t_set_t const ntru_pk_set_2) {
  mam_ntru_pk_t_set_entry_t *entry = NULL;
  mam_ntru_pk_t_set_entry_t *tmp = NULL;

  TEST_ASSERT_EQUAL_INT(mam_ntru_pk_t_set_size(ntru_pk_set_1),
                        mam_ntru_pk_t_set_size(ntru_pk_set_2));

  HASH_ITER(hh, ntru_pk_set_1, entry, tmp) {
    if (!mam_ntru_pk_t_set_contains(&ntru_pk_set_2, &entry->value)) {
      return false;
    }
  }

  return true;
}

static bool mam_ntru_sk_t_set_cmp(mam_ntru_sk_t_set_t const ntru_sk_set_1,
                                  mam_ntru_sk_t_set_t const ntru_sk_set_2) {
  mam_ntru_sk_t_set_entry_t *entry1 = NULL;
  mam_ntru_sk_t_set_entry_t *tmp1 = NULL;

  mam_ntru_sk_t_set_entry_t *entry2 = NULL;
  mam_ntru_sk_t_set_entry_t *tmp2 = NULL;

  size_t match = 0;

  TEST_ASSERT_EQUAL_INT(mam_ntru_sk_t_set_size(ntru_sk_set_1),
                        mam_ntru_sk_t_set_size(ntru_sk_set_2));

  // We have to loop in O(N^2) because mam_ntru_sk_t_set_contains compares hash
  // value based on
  // the address assigned into the "f" field in the ntru_sk_t type
  HASH_ITER(hh, ntru_sk_set_1, entry1, tmp1) {
    HASH_ITER(hh, ntru_sk_set_2, entry2, tmp2) {
      if (memcmp(&entry1->value, &entry2->value,
                 MAM2_NTRU_PK_SIZE + MAM2_NTRU_SK_SIZE) == 0) {
        match++;
        break;
      }
    }
  }

  return mam_ntru_sk_t_set_size(ntru_sk_set_1) == match;
}

static void test_ntru_pk_serialization(void) {
  mam_ntru_pk_t_set_t ntru_set_1 = NULL;
  mam_ntru_pk_t_set_t ntru_set_2 = NULL;
  mam_ntru_pk_t ntru;
  tryte_t ntru_pk[MAM2_NTRU_PK_SIZE / 3];

  for (size_t i = 0; i < 26; i++) {
    memset(ntru_pk, 'A' + i, MAM2_NTRU_PK_SIZE / 3);
    trytes_to_trits(ntru_pk, ntru.key, MAM2_NTRU_PK_SIZE / 3);
    TEST_ASSERT(mam_ntru_pk_t_set_add(&ntru_set_1, &ntru) == RC_OK);
  }

  size_t size = mam_ntru_pks_serialized_size(ntru_set_1);

  TEST_ASSERT_EQUAL_INT(size, 26 * MAM2_NTRU_PK_SIZE);

  trits_t trits = trits_alloc(size);

  TEST_ASSERT(mam_ntru_pks_serialize(ntru_set_1, trits) == RC_OK);

  TEST_ASSERT(mam_ntru_pks_deserialize(trits, &ntru_set_2) == RC_OK);

  TEST_ASSERT_TRUE(mam_ntru_pk_t_set_cmp(ntru_set_1, ntru_set_2));

  trits_free(trits);
  mam_ntru_pk_t_set_free(&ntru_set_1);
  mam_ntru_pk_t_set_free(&ntru_set_2);
}

static void test_ntru_sk_serialization(void) {
  mam_ntru_sk_t_set_t ntru_sk_set_1 = NULL;
  mam_ntru_sk_t_set_t ntru_sk_set_2 = NULL;
  mam_ntru_sk_t ntru_sk;

  // NOTE: this is not a good example for how to generate an NTRU key
  // since the public_key is derived from the secret key rather than
  // being set
  for (int i = -1; i <= 1; i++) {
    memset(ntru_sk.public_key.key, i, MAM2_NTRU_PK_SIZE);
    memset(ntru_sk.secret_key, i, MAM2_NTRU_SK_SIZE);
    TEST_ASSERT(mam_ntru_sk_t_set_add(&ntru_sk_set_1, &ntru_sk) == RC_OK);
  }

  size_t size1 = mam_ntru_sk_t_set_size(ntru_sk_set_1);
  size_t size = mam_ntru_sks_serialized_size(ntru_sk_set_1);

  TEST_ASSERT_EQUAL_INT(size, 3 * (MAM2_NTRU_PK_SIZE + MAM2_NTRU_SK_SIZE));

  trits_t trits = trits_alloc(size);

  TEST_ASSERT(mam_ntru_sks_serialize(ntru_sk_set_1, trits) == RC_OK);

  TEST_ASSERT(mam_ntru_sks_deserialize(trits, &ntru_sk_set_2) == RC_OK);

  TEST_ASSERT_TRUE(mam_ntru_sk_t_set_cmp(ntru_sk_set_1, ntru_sk_set_2));

  mam_ntru_sk_t_set_free(&ntru_sk_set_1);
  mam_ntru_sk_t_set_free(&ntru_sk_set_2);

  trits_free(trits);
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_ntru_pk_serialization);
  RUN_TEST(test_ntru_sk_serialization);

  return UNITY_END();
}
