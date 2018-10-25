/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <unity/unity.h>

#include "ciri/api/api.h"
#include "ciri/api/tests/defs.h"
#include "ciri/node.h"
#include "consensus/defs.h"
#include "consensus/test_utils/bundle.h"
#include "consensus/test_utils/tangle.h"

static char *test_db_path = "ciri/api/tests/test.db";
static char *ciri_db_path = "ciri/api/tests/ciri.db";
static connection_config_t config;
static iota_api_t api;
static tangle_t tangle;
static transaction_validator_t transaction_validator;
static node_t node;
static broadcaster_t broadcaster;

void setUp(void) {
  TEST_ASSERT(tangle_setup(&tangle, &config, test_db_path, ciri_db_path) ==
              RC_OK);
}

void tearDown(void) {
  TEST_ASSERT(tangle_cleanup(&tangle, test_db_path) == RC_OK);
}

void test_broadcast_transactions_empty(void) {
  broadcast_transactions_req_t *req = broadcast_transactions_req_new();

  TEST_ASSERT(iota_api_broadcast_transactions(&api, req) == RC_OK);

  TEST_ASSERT_EQUAL_INT(broadcaster_size(&broadcaster), 0);

  broadcast_transactions_req_free(&req);
  TEST_ASSERT(req == NULL);
}

void test_broadcast_transactions_invalid_tx(void) {
  broadcast_transactions_req_t *req = broadcast_transactions_req_new();
  struct _iota_transaction tx;
  flex_trit_t tx_trits[FLEX_TRIT_SIZE_8019];
  tryte_t tx_trytes[NUM_TRYTES_SERIALIZED_TRANSACTION + 1];

  // Trying to broadcast an invalid transaction (invalid supply)

  flex_trits_from_trytes(
      tx_trits, NUM_TRITS_SERIALIZED_TRANSACTION, TX_1_OF_4_VALUE_BUNDLE_TRYTES,
      NUM_TRYTES_SERIALIZED_TRANSACTION, NUM_TRYTES_SERIALIZED_TRANSACTION);
  transaction_deserialize_from_trits(&tx, tx_trits);
  tx.value = -IOTA_SUPPLY - 1;
  transaction_serialize_on_flex_trits(&tx, tx_trits);
  flex_trits_to_trytes(tx_trytes, NUM_TRYTES_SERIALIZED_TRANSACTION, tx_trits,
                       NUM_TRITS_SERIALIZED_TRANSACTION,
                       NUM_TRITS_SERIALIZED_TRANSACTION);
  tx_trytes[NUM_TRYTES_SERIALIZED_TRANSACTION] = '\0';
  broadcast_transactions_req_add_trytes(req, tx_trytes);
  TEST_ASSERT(iota_api_broadcast_transactions(&api, req) == RC_OK);

  TEST_ASSERT_EQUAL_INT(broadcaster_size(&broadcaster), 0);

  broadcast_transactions_req_free(&req);
  TEST_ASSERT(req == NULL);
}

void test_broadcast_transactions(void) {
  broadcast_transactions_req_t *req = broadcast_transactions_req_new();
  tryte_t const *const txs_trytes[4] = {
      TX_1_OF_4_VALUE_BUNDLE_TRYTES, TX_2_OF_4_VALUE_BUNDLE_TRYTES,
      TX_3_OF_4_VALUE_BUNDLE_TRYTES, TX_4_OF_4_VALUE_BUNDLE_TRYTES};

  // Broadcasting 4 transactions

  for (size_t i = 0; i < 4; i++) {
    broadcast_transactions_req_add_trytes(req, txs_trytes[i]);
  }
  TEST_ASSERT(iota_api_broadcast_transactions(&api, req) == RC_OK);

  TEST_ASSERT_EQUAL_INT(broadcaster_size(&broadcaster), 4);

  broadcast_transactions_req_free(&req);
  TEST_ASSERT(req == NULL);
}

int main(void) {
  UNITY_BEGIN();

  config.db_path = test_db_path;
  api.tangle = &tangle;
  api.transaction_validator = &transaction_validator;
  broadcaster_init(&broadcaster, &node);
  api.broadcaster = &broadcaster;

  iota_consensus_transaction_validator_init(&transaction_validator, 1536845195,
                                            10);

  RUN_TEST(test_broadcast_transactions_empty);
  RUN_TEST(test_broadcast_transactions_invalid_tx);
  RUN_TEST(test_broadcast_transactions);

  return UNITY_END();
}
