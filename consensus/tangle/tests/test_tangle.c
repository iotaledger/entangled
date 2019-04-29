/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <unity/unity.h>

#include "consensus/test_utils/bundle.h"
#include "consensus/test_utils/tangle.h"

static char *test_db_path = "consensus/tangle/tests/test.db";
static char *tangle_db_path = "consensus/tangle/tests/tangle.db";
static connection_config_t config;
static tangle_t tangle;

void setUp(void) { TEST_ASSERT(tangle_setup(&tangle, &config, test_db_path, tangle_db_path) == RC_OK); }

void tearDown(void) { TEST_ASSERT(tangle_cleanup(&tangle, test_db_path) == RC_OK); }

void test_find_tail(void) {
  iota_transaction_t *txs[4];
  flex_trit_t hash[FLEX_TRIT_SIZE_243];
  flex_trit_t tail[FLEX_TRIT_SIZE_243];
  flex_trit_t expected_tail[FLEX_TRIT_SIZE_243];
  tryte_t const *const txs_trytes[4] = {TX_1_OF_4_VALUE_BUNDLE_TRYTES, TX_2_OF_4_VALUE_BUNDLE_TRYTES,
                                        TX_3_OF_4_VALUE_BUNDLE_TRYTES, TX_4_OF_4_VALUE_BUNDLE_TRYTES};
  tryte_t const *const hashes_trytes[4] = {TX_1_OF_4_HASH, TX_2_OF_4_HASH, TX_3_OF_4_HASH, TX_4_OF_4_HASH};
  bool found = false;

  transactions_deserialize(txs_trytes, txs, 4, true);
  TEST_ASSERT(build_tangle(&tangle, txs, 4) == RC_OK);

  flex_trits_from_trytes(expected_tail, HASH_LENGTH_TRIT, hashes_trytes[0], HASH_LENGTH_TRYTE, HASH_LENGTH_TRYTE);

  for (size_t i = 0; i < 4; i++) {
    found = false;
    flex_trits_from_trytes(hash, HASH_LENGTH_TRIT, hashes_trytes[i], HASH_LENGTH_TRYTE, HASH_LENGTH_TRYTE);
    TEST_ASSERT(iota_tangle_find_tail(&tangle, hash, tail, &found) == RC_OK);
    TEST_ASSERT_TRUE(found);
    TEST_ASSERT_EQUAL_MEMORY(expected_tail, tail, FLEX_TRIT_SIZE_243);
  }

  transactions_free(txs, 4);
}

void test_find_tail_unknown_tx(void) {
  iota_transaction_t *txs[4];
  flex_trit_t hash[FLEX_TRIT_SIZE_243];
  flex_trit_t tail[FLEX_TRIT_SIZE_243];
  tryte_t const *const txs_trytes[4] = {TX_1_OF_4_VALUE_BUNDLE_TRYTES, TX_2_OF_4_VALUE_BUNDLE_TRYTES,
                                        TX_3_OF_4_VALUE_BUNDLE_TRYTES, TX_4_OF_4_VALUE_BUNDLE_TRYTES};
  bool found = true;

  transactions_deserialize(txs_trytes, txs, 4, true);
  TEST_ASSERT(build_tangle(&tangle, txs, 4) == RC_OK);

  memset(hash, FLEX_TRIT_NULL_VALUE, FLEX_TRIT_SIZE_243);
  TEST_ASSERT(iota_tangle_find_tail(&tangle, hash, tail, &found) == RC_OK);
  TEST_ASSERT_FALSE(found);

  transactions_free(txs, 4);
}

void test_find_tail_broken_link(void) {
  iota_transaction_t *txs[4];
  flex_trit_t hash[FLEX_TRIT_SIZE_243];
  flex_trit_t tail[FLEX_TRIT_SIZE_243];
  tryte_t broken_link[NUM_TRYTES_SERIALIZED_TRANSACTION];
  memcpy(broken_link, TX_2_OF_4_VALUE_BUNDLE_TRYTES, NUM_TRYTES_SERIALIZED_TRANSACTION);
  broken_link[0] = '9';
  tryte_t const *const txs_trytes[4] = {TX_1_OF_4_VALUE_BUNDLE_TRYTES, broken_link, TX_3_OF_4_VALUE_BUNDLE_TRYTES,
                                        TX_4_OF_4_VALUE_BUNDLE_TRYTES};
  bool found = true;

  transactions_deserialize(txs_trytes, txs, 4, true);
  TEST_ASSERT(build_tangle(&tangle, txs, 4) == RC_OK);

  flex_trits_from_trytes(hash, HASH_LENGTH_TRIT, TX_4_OF_4_HASH, HASH_LENGTH_TRYTE, HASH_LENGTH_TRYTE);
  TEST_ASSERT(iota_tangle_find_tail(&tangle, hash, tail, &found) == RC_OK);
  TEST_ASSERT_FALSE(found);

  transactions_free(txs, 4);
}

int main(void) {
  UNITY_BEGIN();
  TEST_ASSERT(storage_init() == RC_OK);

  config.db_path = test_db_path;

  RUN_TEST(test_find_tail);
  RUN_TEST(test_find_tail_unknown_tx);
  RUN_TEST(test_find_tail_broken_link);

  TEST_ASSERT(storage_destroy() == RC_OK);
  return UNITY_END();
}
