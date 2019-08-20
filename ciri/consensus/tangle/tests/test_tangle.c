/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <unity/unity.h>

#include "ciri/consensus/test_utils/bundle.h"
#include "ciri/consensus/test_utils/tangle.h"

static char *tangle_test_db_path = "ciri/consensus/tangle/tests/test.db";
static char *tangle_db_path = "common/storage/tangle.db";
static connection_config_t config;
static tangle_t tangle;

void setUp(void) { TEST_ASSERT(tangle_setup(&tangle, &config, tangle_test_db_path, tangle_db_path) == RC_OK); }

void tearDown(void) { TEST_ASSERT(tangle_cleanup(&tangle, tangle_test_db_path) == RC_OK); }

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

void test_bundle_load(void) {
  size_t i = 0;
  iota_transaction_t *iter;
  iota_transaction_t *txs[4];
  flex_trit_t hash[FLEX_TRIT_SIZE_243];
  tryte_t const *const txs_trytes[4] = {TX_1_OF_4_VALUE_BUNDLE_TRYTES, TX_2_OF_4_VALUE_BUNDLE_TRYTES,
                                        TX_3_OF_4_VALUE_BUNDLE_TRYTES, TX_4_OF_4_VALUE_BUNDLE_TRYTES};
  tryte_t const *const hashes_trytes[4] = {TX_1_OF_4_HASH, TX_2_OF_4_HASH, TX_3_OF_4_HASH, TX_4_OF_4_HASH};
  bundle_transactions_t *bundle;

  transactions_deserialize(txs_trytes, txs, 4, true);
  TEST_ASSERT(build_tangle(&tangle, txs, 4) == RC_OK);

  flex_trits_from_trytes(hash, HASH_LENGTH_TRIT, hashes_trytes[0], HASH_LENGTH_TRYTE, HASH_LENGTH_TRYTE);

  bundle_transactions_new(&bundle);

  TEST_ASSERT(iota_tangle_bundle_load(&tangle, hash, bundle) == RC_OK);

  TEST_ASSERT_EQUAL_INT(bundle_transactions_size(bundle), 4);

  BUNDLE_FOREACH(bundle, iter) {
    flex_trits_from_trytes(hash, HASH_LENGTH_TRIT, hashes_trytes[i++], HASH_LENGTH_TRYTE, HASH_LENGTH_TRYTE);
    TEST_ASSERT_EQUAL_MEMORY(transaction_hash(iter), hash, FLEX_TRIT_SIZE_243);
  }

  bundle_transactions_free(&bundle);

  transactions_free(txs, 4);
}

void test_bundle_load_unknown_tx(void) {
  iota_transaction_t *txs[4];
  flex_trit_t hash[FLEX_TRIT_SIZE_243];
  tryte_t const *const txs_trytes[4] = {TX_1_OF_4_VALUE_BUNDLE_TRYTES, TX_2_OF_4_VALUE_BUNDLE_TRYTES,
                                        TX_3_OF_4_VALUE_BUNDLE_TRYTES, TX_4_OF_4_VALUE_BUNDLE_TRYTES};
  bundle_transactions_t *bundle;

  transactions_deserialize(txs_trytes, txs, 4, true);
  TEST_ASSERT(build_tangle(&tangle, txs, 4) == RC_OK);

  memset(hash, FLEX_TRIT_NULL_VALUE, FLEX_TRIT_SIZE_243);

  bundle_transactions_new(&bundle);

  TEST_ASSERT(iota_tangle_bundle_load(&tangle, hash, bundle) == RC_TANGLE_TAIL_NOT_FOUND);

  TEST_ASSERT_EQUAL_INT(bundle_transactions_size(bundle), 0);

  bundle_transactions_free(&bundle);

  transactions_free(txs, 4);
}

void test_bundle_load_not_a_tail(void) {
  iota_transaction_t *txs[4];
  flex_trit_t hash[FLEX_TRIT_SIZE_243];
  tryte_t const *const txs_trytes[4] = {TX_1_OF_4_VALUE_BUNDLE_TRYTES, TX_2_OF_4_VALUE_BUNDLE_TRYTES,
                                        TX_3_OF_4_VALUE_BUNDLE_TRYTES, TX_4_OF_4_VALUE_BUNDLE_TRYTES};
  bundle_transactions_t *bundle;

  transactions_deserialize(txs_trytes, txs, 4, true);
  TEST_ASSERT(build_tangle(&tangle, txs, 4) == RC_OK);

  flex_trits_from_trytes(hash, HASH_LENGTH_TRIT, TX_3_OF_4_HASH, HASH_LENGTH_TRYTE, HASH_LENGTH_TRYTE);

  bundle_transactions_new(&bundle);

  TEST_ASSERT(iota_tangle_bundle_load(&tangle, hash, bundle) == RC_TANGLE_NOT_A_TAIL);

  TEST_ASSERT_EQUAL_INT(bundle_transactions_size(bundle), 0);

  bundle_transactions_free(&bundle);

  transactions_free(txs, 4);
}

int main(void) {
  UNITY_BEGIN();
  TEST_ASSERT(storage_init() == RC_OK);

  config.db_path = tangle_test_db_path;

  RUN_TEST(test_find_tail);
  RUN_TEST(test_find_tail_unknown_tx);
  RUN_TEST(test_find_tail_broken_link);

  RUN_TEST(test_bundle_load);
  RUN_TEST(test_bundle_load_unknown_tx);
  RUN_TEST(test_bundle_load_not_a_tail);

  TEST_ASSERT(storage_destroy() == RC_OK);
  return UNITY_END();
}
