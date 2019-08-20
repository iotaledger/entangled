/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <unity/unity.h>

#include "ciri/api/api.h"
#include "ciri/api/tests/defs.h"
#include "ciri/consensus/test_utils/bundle.h"
#include "ciri/consensus/test_utils/tangle.h"

static char *tangle_test_db_path = "ciri/api/tests/tangle-test.db";
static char *tangle_db_path = "common/storage/tangle.db";
static connection_config_t config;
static iota_api_t api;
static tangle_t tangle;

void setUp(void) { TEST_ASSERT(tangle_setup(&tangle, &config, tangle_test_db_path, tangle_db_path) == RC_OK); }

void tearDown(void) { TEST_ASSERT(tangle_cleanup(&tangle, tangle_test_db_path) == RC_OK); }

void test_get_trytes_empty(void) {
  get_trytes_req_t *req = get_trytes_req_new();
  get_trytes_res_t *res = get_trytes_res_new();
  error_res_t *error = NULL;

  api.conf.max_get_trytes = 100;

  TEST_ASSERT(iota_api_get_trytes(&api, &tangle, req, res, &error) == RC_OK);
  TEST_ASSERT(error == NULL);
  TEST_ASSERT_EQUAL_INT(hash8019_queue_count(res->trytes), 0);

  get_trytes_req_free(&req);
  get_trytes_res_free(&res);
  error_res_free(&error);
  TEST_ASSERT(req == NULL);
  TEST_ASSERT(res == NULL);
}

void test_get_trytes_not_found(void) {
  get_trytes_req_t *req = get_trytes_req_new();
  get_trytes_res_t *res = get_trytes_res_new();
  error_res_t *error = NULL;
  tryte_t tx_trytes[NUM_TRYTES_SERIALIZED_TRANSACTION];
  flex_trit_t null_hash[FLEX_TRIT_SIZE_243];
  memset(null_hash, FLEX_TRIT_NULL_VALUE, FLEX_TRIT_SIZE_243);

  api.conf.max_get_trytes = 100;

  TEST_ASSERT(hash243_queue_push(&req->hashes, null_hash) == RC_OK);

  TEST_ASSERT(iota_api_get_trytes(&api, &tangle, req, res, &error) == RC_OK);
  TEST_ASSERT(error == NULL);
  TEST_ASSERT_EQUAL_INT(hash243_queue_count(req->hashes), 1);

  flex_trits_to_trytes(tx_trytes, NUM_TRYTES_SERIALIZED_TRANSACTION, hash8019_queue_at(res->trytes, 0),
                       NUM_TRITS_SERIALIZED_TRANSACTION, NUM_TRITS_SERIALIZED_TRANSACTION);
  TEST_ASSERT_EQUAL_MEMORY(tx_trytes, NULL_TX_TRYTES, NUM_TRYTES_SERIALIZED_TRANSACTION);

  get_trytes_req_free(&req);
  get_trytes_res_free(&res);
  error_res_free(&error);
  TEST_ASSERT(req == NULL);
  TEST_ASSERT(res == NULL);
}

void test_get_trytes_max(void) {
  get_trytes_req_t *req = get_trytes_req_new();
  get_trytes_res_t *res = get_trytes_res_new();
  error_res_t *error = NULL;
  flex_trit_t hash[FLEX_TRIT_SIZE_243] = {};

  api.conf.max_get_trytes = 1;

  // Storing 2 transactions to get trytes from

  iota_transaction_t *txs[2];
  tryte_t const *const trytes[2] = {TX_1_OF_4_VALUE_BUNDLE_TRYTES, TX_2_OF_4_VALUE_BUNDLE_TRYTES};
  transactions_deserialize(trytes, txs, 2, true);
  build_tangle(&tangle, txs, 2);

  // Getting trytes 1 & 2 from hashes
  flex_trits_from_trytes(hash, NUM_TRITS_HASH, TX_1_OF_4_HASH, NUM_TRYTES_HASH, NUM_TRYTES_HASH);
  TEST_ASSERT(hash243_queue_push(&req->hashes, hash) == RC_OK);

  flex_trits_from_trytes(hash, NUM_TRITS_HASH, TX_2_OF_4_HASH, NUM_TRYTES_HASH, NUM_TRYTES_HASH);
  TEST_ASSERT(hash243_queue_push(&req->hashes, hash) == RC_OK);

  TEST_ASSERT(iota_api_get_trytes(&api, &tangle, req, res, &error) == RC_API_MAX_GET_TRYTES);
  TEST_ASSERT(error == NULL);

  get_trytes_req_free(&req);
  get_trytes_res_free(&res);
  error_res_free(&error);
  TEST_ASSERT(req == NULL);
  TEST_ASSERT(res == NULL);
  transactions_free(txs, 2);
}

void test_get_trytes(void) {
  get_trytes_req_t *req = get_trytes_req_new();
  get_trytes_res_t *res = get_trytes_res_new();
  error_res_t *error = NULL;
  iota_transaction_t *txs[4];
  tryte_t const *const txs_trytes[4] = {TX_1_OF_4_VALUE_BUNDLE_TRYTES, TX_2_OF_4_VALUE_BUNDLE_TRYTES,
                                        TX_3_OF_4_VALUE_BUNDLE_TRYTES, TX_4_OF_4_VALUE_BUNDLE_TRYTES};
  tryte_t const *const hashes_trytes[4] = {TX_1_OF_4_HASH, TX_2_OF_4_HASH, TX_3_OF_4_HASH, TX_4_OF_4_HASH};
  tryte_t tx_trytes[NUM_TRYTES_SERIALIZED_TRANSACTION];
  hash8019_queue_entry_t *iter = NULL;
  size_t tx_index = 0;
  flex_trit_t tmp_hash[FLEX_TRIT_SIZE_243];

  api.conf.max_get_trytes = 100;

  // Storing transactions to get trytes from

  transactions_deserialize(txs_trytes, txs, 4, true);
  TEST_ASSERT(build_tangle(&tangle, txs, 4) == RC_OK);

  // Loading trytes from hashes

  for (size_t i = 0; i < 4; i++) {
    TEST_ASSERT(flex_trits_from_trytes(tmp_hash, NUM_TRITS_HASH, hashes_trytes[i], NUM_TRYTES_HASH, NUM_TRYTES_HASH));
    TEST_ASSERT(hash243_queue_push(&req->hashes, tmp_hash) == RC_OK);
  }

  TEST_ASSERT(iota_api_get_trytes(&api, &tangle, req, res, &error) == RC_OK);
  TEST_ASSERT(error == NULL);
  TEST_ASSERT_EQUAL_INT(hash243_queue_count(req->hashes), 4);

  CDL_FOREACH(res->trytes, iter) {
    flex_trits_to_trytes(tx_trytes, NUM_TRYTES_SERIALIZED_TRANSACTION, iter->hash, NUM_TRITS_SERIALIZED_TRANSACTION,
                         NUM_TRITS_SERIALIZED_TRANSACTION);
    TEST_ASSERT_EQUAL_MEMORY(tx_trytes, txs_trytes[tx_index], NUM_TRYTES_SERIALIZED_TRANSACTION);
    tx_index++;
  }

  get_trytes_req_free(&req);
  get_trytes_res_free(&res);
  error_res_free(&error);
  TEST_ASSERT(req == NULL);
  TEST_ASSERT(res == NULL);
  transactions_free(txs, 4);
}

int main(void) {
  UNITY_BEGIN();
  TEST_ASSERT(storage_init() == RC_OK);

  config.db_path = tangle_test_db_path;

  TEST_ASSERT(iota_api_conf_init(&api.conf) == RC_OK);

  RUN_TEST(test_get_trytes_empty);
  RUN_TEST(test_get_trytes_not_found);
  RUN_TEST(test_get_trytes_max);
  RUN_TEST(test_get_trytes);

  TEST_ASSERT(storage_destroy() == RC_OK);
  return UNITY_END();
}
