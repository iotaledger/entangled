/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <unity/unity.h>

#include "ciri/api/api.h"
#include "consensus/test_utils/bundle.h"
#include "consensus/test_utils/tangle.h"
#include "gossip/node.h"
#include "utils/files.h"

static char *test_db_path = "ciri/api/tests/test.db";
static char *ciri_db_path = "ciri/api/tests/ciri.db";
static connection_config_t config;
static iota_api_t api;
static core_t core;
static tangle_t tangle;

void setUp(void) { TEST_ASSERT(tangle_setup(&tangle, &config, test_db_path, ciri_db_path) == RC_OK); }

void tearDown(void) { TEST_ASSERT(tangle_cleanup(&tangle, test_db_path) == RC_OK); }

void test_get_transactions_to_approve_invalid_depth(void) {
  get_transactions_to_approve_req_t *req = get_transactions_to_approve_req_new();
  get_transactions_to_approve_res_t *res = get_transactions_to_approve_res_new();

  get_transactions_to_approve_req_set_depth(req, 42);
  TEST_ASSERT(iota_api_get_transactions_to_approve(&api, &tangle, req, res) == RC_API_INVALID_DEPTH_INPUT);

  get_transactions_to_approve_req_free(&req);
  get_transactions_to_approve_res_free(&res);
  TEST_ASSERT(req == NULL);
  TEST_ASSERT(res == NULL);
}

void test_get_transactions_to_approve_invalid_subtangle_status(void) {
  get_transactions_to_approve_req_t *req = get_transactions_to_approve_req_new();
  get_transactions_to_approve_res_t *res = get_transactions_to_approve_res_new();

  uint64_t latest_solid_subtangle_milestone_index =
      api.core->consensus.milestone_tracker.latest_solid_subtangle_milestone_index;
  uint64_t milestone_start_index = api.core->consensus.milestone_tracker.milestone_start_index;

  api.core->consensus.milestone_tracker.latest_solid_subtangle_milestone_index = 42;
  api.core->consensus.milestone_tracker.milestone_start_index = 42;

  get_transactions_to_approve_req_set_depth(req, 5);

  TEST_ASSERT(iota_api_get_transactions_to_approve(&api, &tangle, req, res) == RC_API_INVALID_SUBTANGLE_STATUS);

  api.core->consensus.milestone_tracker.latest_solid_subtangle_milestone_index = latest_solid_subtangle_milestone_index;
  api.core->consensus.milestone_tracker.milestone_start_index = milestone_start_index;

  get_transactions_to_approve_req_free(&req);
  get_transactions_to_approve_res_free(&res);
  TEST_ASSERT(req == NULL);
  TEST_ASSERT(res == NULL);
}

int main(void) {
  UNITY_BEGIN();
  TEST_ASSERT(storage_init() == RC_OK);

  config.db_path = test_db_path;
  api.core = &core;

  TEST_ASSERT(iota_gossip_conf_init(&api.core->node.conf) == RC_OK);
  TEST_ASSERT(iota_consensus_conf_init(&api.core->consensus.conf) == RC_OK);
  TEST_ASSERT(requester_init(&api.core->node.transaction_requester, &api.core->node) == RC_OK);
  TEST_ASSERT(tips_cache_init(&api.core->node.tips, api.core->node.conf.tips_cache_size) == RC_OK);
  setUp();

  // Avoid verifying snapshot signature
  api.core->consensus.conf.snapshot_signature_file[0] = '\0';

  TEST_ASSERT(iota_consensus_init(&api.core->consensus, &tangle, &api.core->node.transaction_requester,
                                  &api.core->node.tips) == RC_OK);

  tearDown();

  RUN_TEST(test_get_transactions_to_approve_invalid_depth);
  RUN_TEST(test_get_transactions_to_approve_invalid_subtangle_status);

  TEST_ASSERT(iota_consensus_destroy(&api.core->consensus) == RC_OK);

  TEST_ASSERT(storage_destroy() == RC_OK);
  return UNITY_END();
}
