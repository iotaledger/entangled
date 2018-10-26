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
static requester_state_t transaction_requester;

void setUp(void) {
  TEST_ASSERT(tangle_setup(&consensus.tangle, &config, test_db_path,
                           ciri_db_path) == RC_OK);
}

void tearDown(void) {
  TEST_ASSERT(tangle_cleanup(&consensus.tangle, test_db_path) == RC_OK);
}

void test_get_transactions_to_approve_invalid_depth(void) {
  get_transactions_to_approve_req_t *req =
      get_transactions_to_approve_req_new();
  get_transactions_to_approve_res_t *res =
      get_transactions_to_approve_res_new();

  get_transactions_to_approve_req_set_depth(req, 42);
  TEST_ASSERT(iota_api_get_transactions_to_approve(&api, req, res) ==
              RC_API_INVALID_DEPTH_INPUT);

  get_transactions_to_approve_req_free(&req);
  get_transactions_to_approve_res_free(&res);
  TEST_ASSERT(req == NULL);
  TEST_ASSERT(res == NULL);
}

void test_get_transactions_to_approve_invalid_subtangle_status(void) {
  get_transactions_to_approve_req_t *req =
      get_transactions_to_approve_req_new();
  get_transactions_to_approve_res_t *res =
      get_transactions_to_approve_res_new();

  uint64_t latest_solid_subtangle_milestone_index =
      consensus.milestone_tracker.latest_solid_subtangle_milestone_index;
  uint64_t milestone_start_index =
      consensus.milestone_tracker.milestone_start_index;

  consensus.milestone_tracker.latest_solid_subtangle_milestone_index = 42;
  consensus.milestone_tracker.milestone_start_index = 42;

  TEST_ASSERT(iota_api_get_transactions_to_approve(&api, req, res) ==
              RC_API_INVALID_SUBTANGLE_STATUS);

  consensus.milestone_tracker.latest_solid_subtangle_milestone_index =
      latest_solid_subtangle_milestone_index;
  consensus.milestone_tracker.milestone_start_index = milestone_start_index;

  get_transactions_to_approve_req_free(&req);
  get_transactions_to_approve_res_free(&res);
  TEST_ASSERT(req == NULL);
  TEST_ASSERT(res == NULL);
}

int main(void) {
  UNITY_BEGIN();

  config.db_path = test_db_path;
  api.consensus = &consensus;

  setUp();
  TEST_ASSERT(iota_consensus_init(&consensus, &config, &transaction_requester,
                                  true) == RC_OK);
  tearDown();

  RUN_TEST(test_get_transactions_to_approve_invalid_depth);
  RUN_TEST(test_get_transactions_to_approve_invalid_subtangle_status);

  return UNITY_END();
}
