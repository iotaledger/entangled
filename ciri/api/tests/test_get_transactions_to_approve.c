/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <unity/unity.h>

#include "ciri/api/api.h"
#include "ciri/consensus/test_utils/bundle.h"
#include "ciri/consensus/test_utils/tangle.h"
#include "ciri/node/node.h"
#include "utils/files.h"

static char *tangle_test_db_path = "ciri/api/tests/tangle-test.db";
static char *tangle_db_path = "common/storage/tangle.db";
static connection_config_t config;
static iota_api_t api;
static core_t core;
static tangle_t tangle;

void setUp(void) { TEST_ASSERT(tangle_setup(&tangle, &config, tangle_test_db_path, tangle_db_path) == RC_OK); }

void tearDown(void) { TEST_ASSERT(tangle_cleanup(&tangle, tangle_test_db_path) == RC_OK); }

void test_get_transactions_to_approve_invalid_depth(void) {
  get_transactions_to_approve_req_t *req = get_transactions_to_approve_req_new();
  get_transactions_to_approve_res_t *res = get_transactions_to_approve_res_new();
  error_res_t *error = NULL;

  get_transactions_to_approve_req_set_depth(req, 42);
  TEST_ASSERT(iota_api_get_transactions_to_approve(&api, &tangle, req, res, &error) == RC_API_INVALID_DEPTH_INPUT);
  TEST_ASSERT(error == NULL);

  get_transactions_to_approve_req_free(&req);
  get_transactions_to_approve_res_free(&res);
  error_res_free(&error);
  TEST_ASSERT(req == NULL);
  TEST_ASSERT(res == NULL);
}

void test_get_transactions_to_approve_invalid_subtangle_status(void) {
  get_transactions_to_approve_req_t *req = get_transactions_to_approve_req_new();
  get_transactions_to_approve_res_t *res = get_transactions_to_approve_res_new();
  error_res_t *error = NULL;

  get_transactions_to_approve_req_set_depth(req, 5);

  TEST_ASSERT(iota_api_get_transactions_to_approve(&api, &tangle, req, res, &error) == RC_API_UNSYNCED_NODE);
  TEST_ASSERT(error != NULL);
  TEST_ASSERT_EQUAL_STRING(error_res_get_message(error), API_ERROR_UNSYNCED_NODE);

  get_transactions_to_approve_req_free(&req);
  get_transactions_to_approve_res_free(&res);
  error_res_free(&error);
  TEST_ASSERT(req == NULL);
  TEST_ASSERT(res == NULL);
}

int main(void) {
  UNITY_BEGIN();
  TEST_ASSERT(storage_init() == RC_OK);

  config.db_path = tangle_test_db_path;
  api.core = &core;
  core.node.core = &core;

  TEST_ASSERT(iota_node_conf_init(&api.core->node.conf) == RC_OK);
  TEST_ASSERT(iota_consensus_conf_init(&api.core->consensus.conf) == RC_OK);
  TEST_ASSERT(requester_init(&api.core->node.transaction_requester, &api.core->node) == RC_OK);
  TEST_ASSERT(tips_cache_init(&api.core->node.tips, api.core->node.conf.tips_cache_size) == RC_OK);
  setUp();

  // Avoid verifying snapshot signature
  api.core->consensus.conf.snapshot_signature_skip_validation = true;

  TEST_ASSERT(iota_consensus_init(&api.core->consensus, &tangle, &api.core->node.transaction_requester,
                                  &api.core->node.tips) == RC_OK);

  tearDown();

  RUN_TEST(test_get_transactions_to_approve_invalid_subtangle_status);

  api.core->consensus.snapshots_provider.latest_snapshot.metadata.index = 42;
  api.core->consensus.milestone_tracker.latest_milestone_index = 42;

  RUN_TEST(test_get_transactions_to_approve_invalid_depth);

  TEST_ASSERT(iota_consensus_destroy(&api.core->consensus) == RC_OK);

  TEST_ASSERT(storage_destroy() == RC_OK);
  return UNITY_END();
}
