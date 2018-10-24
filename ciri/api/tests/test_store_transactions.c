/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <unity/unity.h>

#include "ciri/api/api.h"
#include "ciri/api/tests/defs.h"
#include "consensus/defs.h"
#include "consensus/test_utils/bundle.h"
#include "consensus/test_utils/tangle.h"

static char *test_db_path = "ciri/api/tests/test.db";
static char *ciri_db_path = "ciri/api/tests/ciri.db";
static connection_config_t config;
static iota_api_t api;
static tangle_t tangle;
static transaction_validator_t transaction_validator;

void setUp(void) {
  TEST_ASSERT(tangle_setup(&tangle, &config, test_db_path, ciri_db_path) ==
              RC_OK);
}

void tearDown(void) {
  TEST_ASSERT(tangle_cleanup(&tangle, test_db_path) == RC_OK);
}

void test_store_transactions_empty(void) {
  store_transactions_req_t *req = store_transactions_req_new();

  TEST_ASSERT(iota_api_store_transactions(&api, req) == RC_OK);

  store_transactions_req_free(&req);
  TEST_ASSERT(req == NULL);
}

void test_store_transactions_invalid_tx(void) {
  store_transactions_req_t *req = store_transactions_req_new();
  DECLARE_PACK_SINGLE_TX(tx, txp, pack);
  flex_trit_t hash_trits[FLEX_TRIT_SIZE_243];
  struct _trit_array hash = {.trits = hash_trits,
                             .num_trits = HASH_LENGTH_TRIT,
                             .num_bytes = FLEX_TRIT_SIZE_243,
                             .dynamic = 0};
  flex_trit_t tx_trits[FLEX_TRIT_SIZE_8019];
  tryte_t tx_trytes[NUM_TRYTES_SERIALIZED_TRANSACTION];

  // Trying to store an invalid transaction (invalid supply)

  flex_trits_from_trytes(
      tx_trits, NUM_TRITS_SERIALIZED_TRANSACTION, TX_1_OF_4_VALUE_BUNDLE_TRYTES,
      NUM_TRYTES_SERIALIZED_TRANSACTION, NUM_TRYTES_SERIALIZED_TRANSACTION);
  transaction_deserialize_from_trits(&tx, tx_trits);
  tx.value = -IOTA_SUPPLY - 1;
  transaction_serialize_on_flex_trits(&tx, tx_trits);
  flex_trits_to_trytes(tx_trytes, NUM_TRYTES_SERIALIZED_TRANSACTION, tx_trits,
                       NUM_TRITS_SERIALIZED_TRANSACTION,
                       NUM_TRITS_SERIALIZED_TRANSACTION);

  store_transactions_req_add_trytes(req, tx_trytes);

  TEST_ASSERT(iota_api_store_transactions(&api, req) == RC_OK);

  // Checking that it hasn't been stored

  flex_trits_from_trytes(hash_trits, HASH_LENGTH_TRIT, TX_1_OF_4_HASH,
                         HASH_LENGTH_TRYTE, HASH_LENGTH_TRYTE);
  TEST_ASSERT(iota_tangle_transaction_load(api.tangle, TRANSACTION_FIELD_HASH,
                                           &hash, &pack) == RC_OK);
  TEST_ASSERT(pack.num_loaded == 0);

  store_transactions_req_free(&req);
  TEST_ASSERT(req == NULL);
}

void test_store_transactions(void) {
  store_transactions_req_t *req = store_transactions_req_new();
  DECLARE_PACK_SINGLE_TX(tx, txp, pack);
  flex_trit_t hash_trits[FLEX_TRIT_SIZE_243];
  struct _trit_array hash = {.trits = hash_trits,
                             .num_trits = HASH_LENGTH_TRIT,
                             .num_bytes = FLEX_TRIT_SIZE_243,
                             .dynamic = 0};
  tryte_t const *const txs_trytes[4] = {
      TX_1_OF_4_VALUE_BUNDLE_TRYTES, TX_2_OF_4_VALUE_BUNDLE_TRYTES,
      TX_3_OF_4_VALUE_BUNDLE_TRYTES, TX_4_OF_4_VALUE_BUNDLE_TRYTES};
  tryte_t const *const hashes_trytes[4] = {TX_1_OF_4_HASH, TX_2_OF_4_HASH,
                                           TX_3_OF_4_HASH, TX_4_OF_4_HASH};
  flex_trit_t tx_trits[FLEX_TRIT_SIZE_8019];
  tryte_t tx_trytes[NUM_TRYTES_SERIALIZED_TRANSACTION];

  // Storing 4 transactions

  for (size_t i = 0; i < 4; i++) {
    store_transactions_req_add_trytes(req, txs_trytes[i]);
  }
  TEST_ASSERT(iota_api_store_transactions(&api, req) == RC_OK);

  // Checking that they have been stored

  for (size_t i = 0; i < 4; i++) {
    hash_pack_reset(&pack);
    flex_trits_from_trytes(hash_trits, HASH_LENGTH_TRIT, hashes_trytes[i],
                           HASH_LENGTH_TRYTE, HASH_LENGTH_TRYTE);
    TEST_ASSERT(iota_tangle_transaction_load(api.tangle, TRANSACTION_FIELD_HASH,
                                             &hash, &pack) == RC_OK);
    TEST_ASSERT(pack.num_loaded == 1);
    transaction_serialize_on_flex_trits(txp, tx_trits);
    flex_trits_to_trytes(tx_trytes, NUM_TRYTES_SERIALIZED_TRANSACTION, tx_trits,
                         NUM_TRITS_SERIALIZED_TRANSACTION,
                         NUM_TRITS_SERIALIZED_TRANSACTION);
    TEST_ASSERT_EQUAL_MEMORY(tx_trytes, txs_trytes[i],
                             NUM_TRYTES_SERIALIZED_TRANSACTION);
  }

  store_transactions_req_free(&req);
  TEST_ASSERT(req == NULL);
}

int main(void) {
  UNITY_BEGIN();

  config.db_path = test_db_path;
  api.tangle = &tangle;
  api.transaction_validator = &transaction_validator;

  iota_consensus_transaction_validator_init(&transaction_validator, 1536845195,
                                            10);

  RUN_TEST(test_store_transactions_empty);
  RUN_TEST(test_store_transactions_invalid_tx);
  RUN_TEST(test_store_transactions);

  return UNITY_END();
}
