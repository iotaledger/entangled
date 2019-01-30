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
#include "mam/v2/mam/mam_ntru_pk_t_set.h"
#include "mam/v2/mam/mam_pre_shared_key_t_set.h"
#include "mam/v2/mam/mam_types.h"

static bool mam_pre_shared_key_t_set_cmp(
    mam_pre_shared_key_t_set_t const psks_1,
    mam_pre_shared_key_t_set_t const psks_2) {
  mam_pre_shared_key_t_set_entry_t *entry = NULL;
  mam_pre_shared_key_t_set_entry_t *tmp = NULL;

  TEST_ASSERT_EQUAL_INT(mam_pre_shared_key_t_set_size(psks_1),
                        mam_pre_shared_key_t_set_size(psks_2));

  HASH_ITER(hh, psks_1, entry, tmp) {
    if (!mam_pre_shared_key_t_set_contains(&psks_2, &entry->value)) {
      return false;
    }
  }

  return true;
}

static bool mam_ntru_pk_t_set_cmp(mam_ntru_pk_t_set_t const ntru_set_1,
                                  mam_ntru_pk_t_set_t const ntru_set_2) {
  mam_ntru_pk_t_set_entry_t *entry = NULL;
  mam_ntru_pk_t_set_entry_t *tmp = NULL;

  TEST_ASSERT_EQUAL_INT(mam_ntru_pk_t_set_size(ntru_set_1),
                        mam_ntru_pk_t_set_size(ntru_set_2));

  HASH_ITER(hh, ntru_set_1, entry, tmp) {
    if (!mam_ntru_pk_t_set_contains(&ntru_set_2, &entry->value)) {
      return false;
    }
  }

  return true;
}

static void test_psks_serialization(void) {
  mam_pre_shared_key_t_set_t psks_1 = NULL;
  mam_pre_shared_key_t_set_t psks_2 = NULL;
  mam_pre_shared_key_t psk;
  tryte_t id[MAM2_PSK_ID_SIZE / 3];
  tryte_t pre_shared_key[MAM2_PSK_SIZE / 3];

  for (size_t i = 0; i < 26; i++) {
    memset(id, 'A' + i, MAM2_PSK_ID_SIZE / 3);
    trytes_to_trits(id, psk.id, MAM2_PSK_ID_SIZE / 3);
    memset(pre_shared_key, 'A' + i, MAM2_PSK_SIZE / 3);
    trytes_to_trits(pre_shared_key, psk.pre_shared_key, MAM2_PSK_SIZE / 3);
    TEST_ASSERT(mam_pre_shared_key_t_set_add(&psks_1, &psk) == RC_OK);
  }

  size_t size = psks_serialized_size(psks_1);

  TEST_ASSERT_EQUAL_INT(size, 26 * (MAM2_PSK_ID_SIZE + MAM2_PSK_SIZE));

  trits_t trits = trits_alloc(size);

  TEST_ASSERT(psks_serialize(psks_1, trits) == RC_OK);

  TEST_ASSERT(psks_deserialize(trits, &psks_2) == RC_OK);

  TEST_ASSERT_TRUE(mam_pre_shared_key_t_set_cmp(psks_1, psks_2));

  trits_free(trits);
  mam_pre_shared_key_t_set_free(&psks_1);
  mam_pre_shared_key_t_set_free(&psks_2);
}

static void test_ntru_serialization(void) {
  mam_ntru_pk_t_set_t ntru_set_1 = NULL;
  mam_ntru_pk_t_set_t ntru_set_2 = NULL;
  mam_ntru_pk_t ntru;
  tryte_t ntru_pk[MAM2_NTRU_PK_SIZE / 3];

  for (size_t i = 0; i < 26; i++) {
    memset(ntru_pk, 'A' + i, MAM2_NTRU_PK_SIZE / 3);
    trytes_to_trits(ntru_pk, ntru.pk, MAM2_NTRU_PK_SIZE / 3);
    TEST_ASSERT(mam_ntru_pk_t_set_add(&ntru_set_1, &ntru) == RC_OK);
  }

  size_t size = ntru_serialized_size(ntru_set_1);

  TEST_ASSERT_EQUAL_INT(size, 26 * MAM2_NTRU_PK_SIZE);

  trits_t trits = trits_alloc(size);

  TEST_ASSERT(ntru_serialize(ntru_set_1, trits) == RC_OK);

  TEST_ASSERT(ntru_deserialize(trits, &ntru_set_2) == RC_OK);

  TEST_ASSERT_TRUE(mam_ntru_pk_t_set_cmp(ntru_set_1, ntru_set_2));

  trits_free(trits);
  mam_ntru_pk_t_set_free(&ntru_set_1);
  mam_ntru_pk_t_set_free(&ntru_set_2);
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_psks_serialization);
  RUN_TEST(test_ntru_serialization);

  return UNITY_END();
}
