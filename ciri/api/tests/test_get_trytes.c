/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <unity/unity.h>

#include "ciri/api/api.h"
#include "ciri/api/tests/defs.h"
#include "consensus/test_utils/bundle.h"
#include "consensus/test_utils/tangle.h"

static char *test_db_path = "ciri/api/tests/test.db";
static char *ciri_db_path = "ciri/api/tests/ciri.db";
static connection_config_t config;
static iota_api_t api;
static iota_consensus_t consensus;

void setUp(void) {
  TEST_ASSERT(tangle_setup(&api.consensus->tangle, &config, test_db_path,
                           ciri_db_path) == RC_OK);
}

void tearDown(void) {
  TEST_ASSERT(tangle_cleanup(&api.consensus->tangle, test_db_path) == RC_OK);
}

void test_get_trytes_empty(void) {
  get_trytes_req_t *req = get_trytes_req_new();
  get_trytes_res_t *res = get_trytes_res_new();

  api.conf.max_get_trytes = 100;

  TEST_ASSERT(iota_api_get_trytes(&api, req, res) == RC_OK);
  TEST_ASSERT_EQUAL_INT(get_trytes_res_trytes_num(res), 0);

  get_trytes_req_free(&req);
  get_trytes_res_free(&res);
  TEST_ASSERT(req == NULL);
  TEST_ASSERT(res == NULL);
}

void test_get_trytes_not_found(void) {
  get_trytes_req_t *req = get_trytes_req_new();
  get_trytes_res_t *res = get_trytes_res_new();
  tryte_t tx_trytes[NUM_TRYTES_SERIALIZED_TRANSACTION];
  const flex_trit_t null_hash[FLEX_TRIT_SIZE_243] = {
      [0 ... FLEX_TRIT_SIZE_243 - 1] = FLEX_TRIT_NULL_VALUE};

  api.conf.max_get_trytes = 100;

  TEST_ASSERT(get_trytes_req_add_hash(req, null_hash) == RC_OK);

  TEST_ASSERT(iota_api_get_trytes(&api, req, res) == RC_OK);
  TEST_ASSERT_EQUAL_INT(get_trytes_res_trytes_num(res), 1);

  flex_trits_to_trytes(tx_trytes, NUM_TRYTES_SERIALIZED_TRANSACTION,
                       get_trytes_res_trytes_at(res, 0),
                       NUM_TRITS_SERIALIZED_TRANSACTION,
                       NUM_TRITS_SERIALIZED_TRANSACTION);
  TEST_ASSERT_EQUAL_MEMORY(tx_trytes, NULL_TX_TRYTES,
                           NUM_TRYTES_SERIALIZED_TRANSACTION);

  get_trytes_req_free(&req);
  get_trytes_res_free(&res);
  TEST_ASSERT(req == NULL);
  TEST_ASSERT(res == NULL);
}

void test_get_trytes_max(void) {
  get_trytes_req_t *req = get_trytes_req_new();
  get_trytes_res_t *res = get_trytes_res_new();
  flex_trit_t hash[FLEX_TRIT_SIZE_243] = {};

  api.conf.max_get_trytes = 1;

  // Storing 2 transactions to get trytes from

  iota_transaction_t txs[2];
  tryte_t const *const trytes[2] = {TX_1_OF_4_VALUE_BUNDLE_TRYTES,
                                    TX_2_OF_4_VALUE_BUNDLE_TRYTES};
  transactions_deserialize(trytes, txs, 2);
  build_tangle(&api.consensus->tangle, txs, 2);

  // Getting trytes 1 & 2 from hashes
  flex_trits_from_trytes(hash, NUM_TRITS_HASH, TX_1_OF_4_HASH, NUM_TRYTES_HASH,
                         NUM_TRYTES_HASH);
  TEST_ASSERT(get_trytes_req_add_hash(req, hash) == RC_OK);

  flex_trits_from_trytes(hash, NUM_TRITS_HASH, TX_2_OF_4_HASH, NUM_TRYTES_HASH,
                         NUM_TRYTES_HASH);
  TEST_ASSERT(get_trytes_req_add_hash(req, hash) == RC_OK);

  TEST_ASSERT(iota_api_get_trytes(&api, req, res) == RC_API_MAX_GET_TRYTES);

  get_trytes_req_free(&req);
  get_trytes_res_free(&res);
  TEST_ASSERT(req == NULL);
  TEST_ASSERT(res == NULL);
  transactions_free(txs, 2);
}

void test_get_trytes(void) {
  get_trytes_req_t *req = get_trytes_req_new();
  get_trytes_res_t *res = get_trytes_res_new();
  iota_transaction_t txs[4];
  tryte_t const *const txs_trytes[4] = {
      TX_1_OF_4_VALUE_BUNDLE_TRYTES, TX_2_OF_4_VALUE_BUNDLE_TRYTES,
      TX_3_OF_4_VALUE_BUNDLE_TRYTES, TX_4_OF_4_VALUE_BUNDLE_TRYTES};
  tryte_t const *const hashes_trytes[4] = {TX_1_OF_4_HASH, TX_2_OF_4_HASH,
                                           TX_3_OF_4_HASH, TX_4_OF_4_HASH};
  tryte_t tx_trytes[NUM_TRYTES_SERIALIZED_TRANSACTION];
  hash8019_queue_entry_t *iter = NULL;
  size_t tx_index = 0;

  api.conf.max_get_trytes = 100;

  // Storing transactions to get trytes from

  transactions_deserialize(txs_trytes, txs, 4);
  TEST_ASSERT(build_tangle(&api.consensus->tangle, txs, 4) == RC_OK);

  // Loading trytes from hashes

  for (size_t i = 0; i < 4; i++) {
    trit_array_p tmp_hash = trit_array_new_from_trytes(hashes_trytes[i]);
    TEST_ASSERT(get_trytes_req_add_hash(req, tmp_hash->trits) == RC_OK);
    trit_array_free(tmp_hash);
  }

  TEST_ASSERT(iota_api_get_trytes(&api, req, res) == RC_OK);
  TEST_ASSERT_EQUAL_INT(get_trytes_res_trytes_num(res), 4);

  CDL_FOREACH(res->trytes, iter) {
    flex_trits_to_trytes(tx_trytes, NUM_TRYTES_SERIALIZED_TRANSACTION,
                         iter->hash, NUM_TRITS_SERIALIZED_TRANSACTION,
                         NUM_TRITS_SERIALIZED_TRANSACTION);
    TEST_ASSERT_EQUAL_MEMORY(tx_trytes, txs_trytes[tx_index],
                             NUM_TRYTES_SERIALIZED_TRANSACTION);
    tx_index++;
  }

  get_trytes_req_free(&req);
  get_trytes_res_free(&res);
  TEST_ASSERT(req == NULL);
  TEST_ASSERT(res == NULL);
  transactions_free(txs, 4);
}

int main(void) {
  UNITY_BEGIN();

  config.db_path = test_db_path;
  api.consensus = &consensus;

  TEST_ASSERT(iota_api_conf_init(&api.conf) == RC_OK);

  RUN_TEST(test_get_trytes_empty);
  RUN_TEST(test_get_trytes_not_found);
  RUN_TEST(test_get_trytes_max);
  RUN_TEST(test_get_trytes);

  return UNITY_END();
}
