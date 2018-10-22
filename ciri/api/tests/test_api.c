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
static tangle_t tangle;

void setUp(void) {
  TEST_ASSERT(tangle_setup(&tangle, &config, test_db_path, ciri_db_path) ==
              RC_OK);
}

void tearDown(void) {
  TEST_ASSERT(tangle_cleanup(&tangle, test_db_path) == RC_OK);
}

/*
 * get_trytes tests
 */

void test_get_trytes_empty(void) {
  get_trytes_req_t *req = get_trytes_req_new();
  get_trytes_res_t *res = get_trytes_res_new();

  api.max_get_trytes = 100;

  TEST_ASSERT(iota_api_get_trytes(&api, req, res) == RC_OK);
  TEST_ASSERT_EQUAL_INT(get_trytes_res_trytes_num(res), 0);

  get_trytes_req_free(&req);
  get_trytes_res_free(&res);
}

void test_get_trytes_not_found(void) {
  get_trytes_req_t *req = get_trytes_req_new();
  get_trytes_res_t *res = get_trytes_res_new();

  api.max_get_trytes = 100;

  get_trytes_req_add_hash(req, NULL_HASH);
  TEST_ASSERT(iota_api_get_trytes(&api, req, res) == RC_OK);
  TEST_ASSERT_EQUAL_INT(get_trytes_res_trytes_num(res), 1);
  TEST_ASSERT_EQUAL_MEMORY(get_trytes_res_trytes_at(res, 0)->trits,
                           NULL_TX_TRYTES, NUM_TRYTES_SERIALIZED_TRANSACTION);

  get_trytes_req_free(&req);
  get_trytes_res_free(&res);
}

void test_get_trytes_max(void) {
  get_trytes_req_t *req = get_trytes_req_new();
  get_trytes_res_t *res = get_trytes_res_new();

  api.max_get_trytes = 1;

  // Storing 2 transactions to get trytes from

  iota_transaction_t txs[2];
  tryte_t const *const trytes[2] = {TX_1_OF_4_VALUE_BUNDLE_TRYTES,
                                    TX_2_OF_4_VALUE_BUNDLE_TRYTES};
  transactions_deserialize(trytes, txs, 2);
  build_tangle(&tangle, txs, 2);

  // Getting trytes 1 & 2 from hashes

  get_trytes_req_add_hash(req, TX_1_OF_4_HASH);
  get_trytes_req_add_hash(req, TX_2_OF_4_HASH);
  TEST_ASSERT(iota_api_get_trytes(&api, req, res) == RC_API_MAX_GET_TRYTES);

  get_trytes_req_free(&req);
  get_trytes_res_free(&res);
  transactions_free(txs, 2);
}

void test_get_trytes(void) {
  get_trytes_req_t *req = get_trytes_req_new();
  get_trytes_res_t *res = get_trytes_res_new();

  api.max_get_trytes = 100;

  // Storing 4 transactions to get trytes from

  iota_transaction_t txs[4];
  tryte_t const *const trytes[4] = {
      TX_1_OF_4_VALUE_BUNDLE_TRYTES, TX_2_OF_4_VALUE_BUNDLE_TRYTES,
      TX_3_OF_4_VALUE_BUNDLE_TRYTES, TX_4_OF_4_VALUE_BUNDLE_TRYTES};
  transactions_deserialize(trytes, txs, 4);
  build_tangle(&tangle, txs, 4);

  // Getting trytes 1 & 3 from hashes

  get_trytes_req_add_hash(req, TX_1_OF_4_HASH);
  get_trytes_req_add_hash(req, TX_3_OF_4_HASH);
  TEST_ASSERT(iota_api_get_trytes(&api, req, res) == RC_OK);
  TEST_ASSERT_EQUAL_INT(get_trytes_res_trytes_num(res), 2);
  TEST_ASSERT_EQUAL_MEMORY(get_trytes_res_trytes_at(res, 0)->trits,
                           TX_1_OF_4_VALUE_BUNDLE_TRYTES,
                           NUM_TRYTES_SERIALIZED_TRANSACTION);
  TEST_ASSERT_EQUAL_MEMORY(get_trytes_res_trytes_at(res, 1)->trits,
                           TX_3_OF_4_VALUE_BUNDLE_TRYTES,
                           NUM_TRYTES_SERIALIZED_TRANSACTION);

  get_trytes_req_free(&req);
  req = get_trytes_req_new();
  get_trytes_res_free(&res);
  res = get_trytes_res_new();

  // Getting trytes 2 & 4 from hashes

  get_trytes_req_add_hash(req, TX_2_OF_4_HASH);
  get_trytes_req_add_hash(req, TX_4_OF_4_HASH);
  TEST_ASSERT(iota_api_get_trytes(&api, req, res) == RC_OK);
  TEST_ASSERT_EQUAL_INT(get_trytes_res_trytes_num(res), 2);
  TEST_ASSERT_EQUAL_MEMORY(get_trytes_res_trytes_at(res, 0)->trits,
                           TX_2_OF_4_VALUE_BUNDLE_TRYTES,
                           NUM_TRYTES_SERIALIZED_TRANSACTION);
  TEST_ASSERT_EQUAL_MEMORY(get_trytes_res_trytes_at(res, 1)->trits,
                           TX_4_OF_4_VALUE_BUNDLE_TRYTES,
                           NUM_TRYTES_SERIALIZED_TRANSACTION);

  get_trytes_req_free(&req);
  get_trytes_res_free(&res);
  transactions_free(txs, 4);
}

int main(void) {
  UNITY_BEGIN();

  config.db_path = test_db_path;

  api.tangle = &tangle;

  RUN_TEST(test_get_trytes_empty);
  RUN_TEST(test_get_trytes_not_found);
  RUN_TEST(test_get_trytes_max);
  RUN_TEST(test_get_trytes);

  return UNITY_END();
}
