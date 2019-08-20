/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <unity/unity.h>

#include "ciri/api/api.h"
#include "ciri/consensus/conf.h"
#include "ciri/consensus/test_utils/tangle.h"

static char *spent_addresses_test_db_path = "ciri/api/tests/spent-addresses-test.db";
static char *spent_addresses_db_path = "common/storage/spent-addresses.db";
static char *tangle_test_db_path = "ciri/api/tests/tangle-test.db";
static char *tangle_db_path = "common/storage/tangle.db";
static storage_connection_config_t spent_addresses_config;
static storage_connection_config_t tangle_config;
static iota_api_t api;
static core_t core;
static tangle_t tangle;
static spent_addresses_provider_t sap;

void setUp(void) {
  TEST_ASSERT(iota_utils_copy_file(spent_addresses_test_db_path, spent_addresses_db_path) == RC_OK);
  TEST_ASSERT(iota_spent_addresses_provider_init(&sap, &spent_addresses_config) == RC_OK);
  TEST_ASSERT(tangle_setup(&tangle, &tangle_config, tangle_test_db_path, tangle_db_path) == RC_OK);
}

void tearDown(void) {
  TEST_ASSERT(iota_spent_addresses_provider_destroy(&sap) == RC_OK);
  TEST_ASSERT(iota_utils_remove_file(spent_addresses_test_db_path) == RC_OK);
  TEST_ASSERT(tangle_cleanup(&tangle, tangle_test_db_path) == RC_OK);
}

static void test_were_addresses_spent_from_empty(void) {
  were_addresses_spent_from_req_t *req = were_addresses_spent_from_req_new();
  were_addresses_spent_from_res_t *res = were_addresses_spent_from_res_new();
  error_res_t *error = NULL;

  TEST_ASSERT(iota_api_were_addresses_spent_from(&api, &tangle, &sap, req, res, &error) == RC_OK);
  TEST_ASSERT(error == NULL);
  TEST_ASSERT_EQUAL_INT(were_addresses_spent_from_res_states_count(res), 0);

  were_addresses_spent_from_req_free(&req);
  were_addresses_spent_from_res_free(&res);
  error_res_free(&error);
}

static void test_were_addresses_spent_from_one_by_one(void) {
  tryte_t address_trytes[HASH_LENGTH_TRYTE];
  flex_trit_t address_trits[FLEX_TRIT_SIZE_243];

  memset(address_trytes, '9', HASH_LENGTH_TRYTE);
  address_trytes[0] = 'A';

  for (size_t i = 0; i < 26; i++) {
    were_addresses_spent_from_req_t *req = were_addresses_spent_from_req_new();
    were_addresses_spent_from_res_t *res = were_addresses_spent_from_res_new();
    error_res_t *error = NULL;

    flex_trits_from_trytes(address_trits, HASH_LENGTH_TRIT, address_trytes, HASH_LENGTH_TRYTE, HASH_LENGTH_TRYTE);
    if (i % 2) {
      TEST_ASSERT(iota_spent_addresses_provider_store(&sap, address_trits) == RC_OK);
    }
    TEST_ASSERT(were_addresses_spent_from_req_add(req, address_trits) == RC_OK);
    TEST_ASSERT(iota_api_were_addresses_spent_from(&api, &tangle, &sap, req, res, &error) == RC_OK);
    TEST_ASSERT(error == NULL);
    TEST_ASSERT_EQUAL_INT(were_addresses_spent_from_res_states_count(res), 1);
    TEST_ASSERT_EQUAL_INT(were_addresses_spent_from_res_states_at(res, 0), i % 2);
    address_trytes[0]++;
    were_addresses_spent_from_req_free(&req);
    were_addresses_spent_from_res_free(&res);
    error_res_free(&error);
  }
}

static void test_were_addresses_spent_from_batch(void) {
  were_addresses_spent_from_req_t *req = were_addresses_spent_from_req_new();
  were_addresses_spent_from_res_t *res = were_addresses_spent_from_res_new();
  error_res_t *error = NULL;
  tryte_t address_trytes[HASH_LENGTH_TRYTE];
  flex_trit_t address_trits[FLEX_TRIT_SIZE_243];

  memset(address_trytes, '9', HASH_LENGTH_TRYTE);
  address_trytes[0] = 'A';

  for (size_t i = 0; i < 26; i++) {
    flex_trits_from_trytes(address_trits, HASH_LENGTH_TRIT, address_trytes, HASH_LENGTH_TRYTE, HASH_LENGTH_TRYTE);
    if (i % 2) {
      TEST_ASSERT(iota_spent_addresses_provider_store(&sap, address_trits) == RC_OK);
    }
    TEST_ASSERT(were_addresses_spent_from_req_add(req, address_trits) == RC_OK);
    address_trytes[0]++;
  }

  TEST_ASSERT(iota_api_were_addresses_spent_from(&api, &tangle, &sap, req, res, &error) == RC_OK);
  TEST_ASSERT(error == NULL);
  TEST_ASSERT_EQUAL_INT(were_addresses_spent_from_res_states_count(res), 26);

  for (size_t i = 0; i < 26; i++) {
    TEST_ASSERT_EQUAL_INT(were_addresses_spent_from_res_states_at(res, i), i % 2);
  }

  were_addresses_spent_from_req_free(&req);
  were_addresses_spent_from_res_free(&res);
  error_res_free(&error);
}

int main(void) {
  UNITY_BEGIN();
  TEST_ASSERT(storage_init() == RC_OK);

  spent_addresses_config.db_path = spent_addresses_test_db_path;
  tangle_config.db_path = tangle_test_db_path;
  api.core = &core;

  TEST_ASSERT(iota_node_conf_init(&api.core->node.conf) == RC_OK);
  TEST_ASSERT(iota_consensus_conf_init(&api.core->consensus.conf) == RC_OK);
  TEST_ASSERT(requester_init(&api.core->node.transaction_requester, &api.core->node) == RC_OK);
  TEST_ASSERT(tips_cache_init(&api.core->node.tips, api.core->node.conf.tips_cache_size) == RC_OK);

  setUp();
  // Avoid verifying snapshot signature
  api.core->consensus.conf.snapshot_signature_skip_validation = true;
  TEST_ASSERT(iota_consensus_init(&api.core->consensus, &tangle, &api.core->node.transaction_requester,
                                  &api.core->node.tips) == RC_OK);

  state_delta_destroy(&api.core->consensus.snapshots_provider.latest_snapshot.state);

  tearDown();

  RUN_TEST(test_were_addresses_spent_from_empty);
  RUN_TEST(test_were_addresses_spent_from_one_by_one);
  RUN_TEST(test_were_addresses_spent_from_batch);

  TEST_ASSERT(storage_destroy() == RC_OK);
  return UNITY_END();
}
