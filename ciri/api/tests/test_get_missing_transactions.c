/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <unity/unity.h>

#include "ciri/api/api.h"
#include "ciri/consensus/test_utils/tangle.h"
#include "ciri/node/node.h"

static char *tangle_test_db_path = "ciri/api/tests/tangle-test.db";
static char *tangle_db_path = "common/storage/tangle.db";
static iota_api_t api;
static storage_connection_config_t config;
static tangle_t tangle;
static core_t core;

void test_get_missing_transactions_empty(void) {
  get_missing_transactions_res_t *res = get_missing_transactions_res_new();
  error_res_t *error = NULL;

  TEST_ASSERT_EQUAL_INT(get_missing_transactions_res_hash_num(res), 0);
  TEST_ASSERT(iota_api_get_missing_transactions(&api, res, &error) == RC_OK);
  TEST_ASSERT(error == NULL);
  TEST_ASSERT_EQUAL_INT(get_missing_transactions_res_hash_num(res), 0);

  get_missing_transactions_res_free(&res);
  error_res_free(&error);
}

void test_get_missing_transactions(void) {
  get_missing_transactions_res_t *res = get_missing_transactions_res_new();
  error_res_t *error = NULL;
  flex_trit_t hashes[10][FLEX_TRIT_SIZE_243];
  tryte_t trytes[81] =
      "A99999999999999999999999999999999999999999999999999999999999999999999999"
      "999999999";
  hash243_stack_entry_t *iter = NULL;

  for (size_t i = 0; i < 10; i++) {
    flex_trits_from_trytes(hashes[i], HASH_LENGTH_TRIT, trytes, HASH_LENGTH_TRYTE, HASH_LENGTH_TRYTE);
    TEST_ASSERT(request_transaction(&api.core->node.transaction_requester, &tangle, hashes[i]) == RC_OK);
    trytes[0]++;
  }

  TEST_ASSERT(iota_api_get_missing_transactions(&api, res, &error) == RC_OK);
  TEST_ASSERT(error == NULL);
  TEST_ASSERT_EQUAL_INT(get_missing_transactions_res_hash_num(res), 10);

  iter = res->hashes;
  for (int i = 9; i >= 0; i--) {
    TEST_ASSERT_EQUAL_MEMORY(iter->hash, hashes[i], FLEX_TRIT_SIZE_243);
    iter = iter->next;
  }

  get_missing_transactions_res_free(&res);
  error_res_free(&error);
}

int main(void) {
  UNITY_BEGIN();
  TEST_ASSERT(storage_init() == RC_OK);

  api.core = &core;
  config.db_path = tangle_test_db_path;
  TEST_ASSERT(tangle_setup(&tangle, &config, tangle_test_db_path, tangle_db_path) == RC_OK);
  TEST_ASSERT(iota_node_conf_init(&api.core->node.conf) == RC_OK);
  TEST_ASSERT(requester_init(&api.core->node.transaction_requester, &api.core->node) == RC_OK);

  RUN_TEST(test_get_missing_transactions_empty);
  RUN_TEST(test_get_missing_transactions);

  TEST_ASSERT(requester_destroy(&api.core->node.transaction_requester) == RC_OK);
  TEST_ASSERT(tangle_cleanup(&tangle, tangle_test_db_path) == RC_OK);

  TEST_ASSERT(storage_destroy() == RC_OK);
  return UNITY_END();
}
