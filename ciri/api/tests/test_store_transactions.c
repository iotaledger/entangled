/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <unity/unity.h>

#include "ciri/api/api.h"
#include "ciri/api/tests/defs.h"
#include "ciri/consensus/conf.h"
#include "ciri/consensus/test_utils/bundle.h"
#include "ciri/consensus/test_utils/tangle.h"
#include "ciri/node/node.h"

static char *tangle_test_db_path = "ciri/api/tests/tangle-test.db";
static char *tangle_db_path = "common/storage/tangle.db";
static storage_connection_config_t config;
static iota_api_t api;
static core_t core;
static tangle_t tangle;

void setUp(void) { TEST_ASSERT(tangle_setup(&tangle, &config, tangle_test_db_path, tangle_db_path) == RC_OK); }

void tearDown(void) { TEST_ASSERT(tangle_cleanup(&tangle, tangle_test_db_path) == RC_OK); }

void test_store_transactions_empty(void) {
  store_transactions_req_t *req = store_transactions_req_new();
  error_res_t *error = NULL;

  TEST_ASSERT(iota_api_store_transactions(&api, &tangle, req, &error) == RC_OK);
  TEST_ASSERT(error == NULL);

  store_transactions_req_free(&req);
  error_res_free(&error);
  TEST_ASSERT(req == NULL);
}

void test_store_transactions_invalid_tx(void) {
  store_transactions_req_t *req = store_transactions_req_new();
  error_res_t *error = NULL;
  DECLARE_PACK_SINGLE_TX(tx, txp, pack);
  flex_trit_t hash_trits[FLEX_TRIT_SIZE_243];
  flex_trit_t tx_trits[FLEX_TRIT_SIZE_8019];

  // Trying to store an invalid transaction (invalid supply)

  flex_trits_from_trytes(tx_trits, NUM_TRITS_SERIALIZED_TRANSACTION, TX_1_OF_4_VALUE_BUNDLE_TRYTES,
                         NUM_TRYTES_SERIALIZED_TRANSACTION, NUM_TRYTES_SERIALIZED_TRANSACTION);
  transaction_deserialize_from_trits(&tx, tx_trits, false);
  transaction_set_value(&tx, -IOTA_SUPPLY - 1);
  transaction_serialize_on_flex_trits(&tx, tx_trits);

  hash_array_push(req->trytes, tx_trits);

  TEST_ASSERT(iota_api_store_transactions(&api, &tangle, req, &error) == RC_OK);
  TEST_ASSERT(error == NULL);

  // Checking that it hasn't been stored

  flex_trits_from_trytes(hash_trits, HASH_LENGTH_TRIT, TX_1_OF_4_HASH, HASH_LENGTH_TRYTE, HASH_LENGTH_TRYTE);
  TEST_ASSERT(iota_tangle_transaction_load(&tangle, TRANSACTION_FIELD_HASH, hash_trits, &pack) == RC_OK);
  TEST_ASSERT(pack.num_loaded == 0);

  store_transactions_req_free(&req);
  error_res_free(&error);
  TEST_ASSERT(req == NULL);
}

void test_store_transactions(void) {
  store_transactions_req_t *req = store_transactions_req_new();
  error_res_t *error = NULL;
  DECLARE_PACK_SINGLE_TX(tx, txp, pack);
  flex_trit_t hash_trits[FLEX_TRIT_SIZE_243];
  tryte_t const *const txs_trytes[4] = {TX_1_OF_4_VALUE_BUNDLE_TRYTES, TX_2_OF_4_VALUE_BUNDLE_TRYTES,
                                        TX_3_OF_4_VALUE_BUNDLE_TRYTES, TX_4_OF_4_VALUE_BUNDLE_TRYTES};
  tryte_t const *const hashes_trytes[4] = {TX_1_OF_4_HASH, TX_2_OF_4_HASH, TX_3_OF_4_HASH, TX_4_OF_4_HASH};
  flex_trit_t tx_trits[FLEX_TRIT_SIZE_8019];
  tryte_t tx_trytes[NUM_TRYTES_SERIALIZED_TRANSACTION];

  // Storing 4 transactions

  for (size_t i = 0; i < 4; i++) {
    flex_trits_from_trytes(tx_trits, NUM_TRITS_SERIALIZED_TRANSACTION, txs_trytes[i], NUM_TRYTES_SERIALIZED_TRANSACTION,
                           NUM_TRYTES_SERIALIZED_TRANSACTION);
    hash_array_push(req->trytes, tx_trits);
  }
  TEST_ASSERT(iota_api_store_transactions(&api, &tangle, req, &error) == RC_OK);
  TEST_ASSERT(error == NULL);
  // Checking that they have been stored

  for (size_t i = 0; i < 4; i++) {
    hash_pack_reset(&pack);
    flex_trits_from_trytes(hash_trits, HASH_LENGTH_TRIT, hashes_trytes[i], HASH_LENGTH_TRYTE, HASH_LENGTH_TRYTE);
    TEST_ASSERT(iota_tangle_transaction_load(&tangle, TRANSACTION_FIELD_HASH, hash_trits, &pack) == RC_OK);
    TEST_ASSERT(pack.num_loaded == 1);
    transaction_serialize_on_flex_trits(txp, tx_trits);
    flex_trits_to_trytes(tx_trytes, NUM_TRYTES_SERIALIZED_TRANSACTION, tx_trits, NUM_TRITS_SERIALIZED_TRANSACTION,
                         NUM_TRITS_SERIALIZED_TRANSACTION);
    TEST_ASSERT_EQUAL_MEMORY(tx_trytes, txs_trytes[i], NUM_TRYTES_SERIALIZED_TRANSACTION);
  }

  store_transactions_req_free(&req);
  error_res_free(&error);
  TEST_ASSERT(req == NULL);
}

int main(void) {
  UNITY_BEGIN();
  TEST_ASSERT(storage_init() == RC_OK);

  config.db_path = tangle_test_db_path;
  api.core = &core;
  TEST_ASSERT(iota_node_conf_init(&api.core->node.conf) == RC_OK);
  TEST_ASSERT(requester_init(&api.core->node.transaction_requester, &api.core->node) == RC_OK);
  TEST_ASSERT(iota_consensus_conf_init(&api.core->consensus.conf) == RC_OK);
  api.core->consensus.conf.snapshot_timestamp_sec = 1536845195;
  api.core->consensus.conf.mwm = 1;
  iota_consensus_transaction_validator_init(&api.core->consensus.transaction_validator,
                                            &api.core->consensus.snapshots_provider,
                                            &api.core->node.transaction_requester, &api.core->consensus.conf);
  tips_cache_init(&api.core->node.tips, 5000);
  iota_consensus_transaction_solidifier_init(&api.core->consensus.transaction_solidifier, &api.core->consensus.conf,
                                             &api.core->node.transaction_requester,
                                             &api.core->consensus.snapshots_provider, &api.core->node.tips);
  iota_milestone_tracker_init(&core.consensus.milestone_tracker, &core.consensus.conf,
                              &core.consensus.snapshots_provider, &core.consensus.ledger_validator,
                              &core.consensus.transaction_solidifier);

  RUN_TEST(test_store_transactions_empty);
  RUN_TEST(test_store_transactions_invalid_tx);
  RUN_TEST(test_store_transactions);

  TEST_ASSERT(storage_destroy() == RC_OK);
  return UNITY_END();
}
