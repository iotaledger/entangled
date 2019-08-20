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
#include "utils/time.h"

static char *tangle_test_db_path = "ciri/api/tests/tangle-test.db";
static char *tangle_db_path = "common/storage/tangle.db";
static iota_api_t api;
static connection_config_t config;
static tangle_t tangle;
static core_t core;

static int LATEST_MILESTONE_INDEX = 909909;
static int LATEST_SOLID_MILESTONE_INDEX = 808808;
static int MILESTONE_START_INDEX = 404404;

static tryte_t *LATEST_MILESTONE =
    (tryte_t *)"LATESTMILESTONE999999999999999999999999999999999999999999999999999999999999999999";
static tryte_t *LATEST_SOLID_MILESTONE =
    (tryte_t *)"LATESTSOLIDMILESTONE9999999999999999999999999999999999999999999999999999999999999";
static tryte_t *COORDINATOR =
    (tryte_t *)"COORDINATOR9999999999999999999999999999999999999999999999999999999999999999999999";

void test_get_node_info(void) {
  flex_trit_t hash[FLEX_TRIT_SIZE_243];
  get_node_info_res_t *res = get_node_info_res_new();
  error_res_t *error = NULL;

  TEST_ASSERT(iota_api_get_node_info(&api, res, &error) == RC_OK);
  TEST_ASSERT(error == NULL);

  TEST_ASSERT_EQUAL_STRING(get_node_info_res_app_name(res), CIRI_NAME);
  TEST_ASSERT_EQUAL_STRING(get_node_info_res_app_version(res), CIRI_VERSION);
  flex_trits_from_trytes(hash, HASH_LENGTH_TRIT, LATEST_MILESTONE, HASH_LENGTH_TRYTE, HASH_LENGTH_TRYTE);
  TEST_ASSERT_EQUAL_MEMORY(get_node_info_res_lm(res), hash, FLEX_TRIT_SIZE_243);
  TEST_ASSERT_EQUAL_UINT32(res->latest_milestone_index, LATEST_MILESTONE_INDEX);
  flex_trits_from_trytes(hash, HASH_LENGTH_TRIT, LATEST_SOLID_MILESTONE, HASH_LENGTH_TRYTE, HASH_LENGTH_TRYTE);
  TEST_ASSERT_EQUAL_MEMORY(get_node_info_res_lssm(res), hash, FLEX_TRIT_SIZE_243);
  TEST_ASSERT_EQUAL_UINT32(res->latest_solid_subtangle_milestone_index, LATEST_SOLID_MILESTONE_INDEX);
  TEST_ASSERT_EQUAL_UINT32(res->milestone_start_index, MILESTONE_START_INDEX);
  TEST_ASSERT_EQUAL_UINT16(res->neighbors, 2);
  TEST_ASSERT_EQUAL_UINT16(res->packets_queue_size, 4);
  TEST_ASSERT_INT_WITHIN(1000, res->time, current_timestamp_ms());
  TEST_ASSERT_EQUAL_UINT32(res->tips, 5);
  TEST_ASSERT_EQUAL_UINT32(res->transactions_to_request, 3);
  flex_trits_from_trytes(hash, HASH_LENGTH_TRIT, COORDINATOR, HASH_LENGTH_TRYTE, HASH_LENGTH_TRYTE);
  TEST_ASSERT_EQUAL_MEMORY(get_node_info_res_coordinator_address(res), hash, FLEX_TRIT_SIZE_243);

  get_node_info_res_free(&res);
  error_res_free(&error);
}

int main(void) {
  UNITY_BEGIN();
  TEST_ASSERT(storage_init() == RC_OK);

  api.core = &core;
  TEST_ASSERT(iota_node_conf_init(&api.core->node.conf) == RC_OK);
  TEST_ASSERT(router_init(&api.core->node.router, &api.core->node) == RC_OK);
  api.core->node.conf.requester_queue_size = 100;
  TEST_ASSERT(requester_init(&api.core->node.transaction_requester, &api.core->node) == RC_OK);
  TEST_ASSERT(broadcaster_stage_init(&api.core->node.broadcaster, &api.core->node) == RC_OK);
  config.db_path = tangle_test_db_path;
  TEST_ASSERT(tangle_setup(&tangle, &config, tangle_test_db_path, tangle_db_path) == RC_OK);
  api.core->consensus.milestone_tracker.latest_milestone_index = LATEST_MILESTONE_INDEX;
  api.core->consensus.milestone_tracker.latest_solid_milestone_index = LATEST_SOLID_MILESTONE_INDEX;
  api.core->consensus.milestone_tracker.milestone_start_index = MILESTONE_START_INDEX;
  flex_trit_t latest_milestone_trits[FLEX_TRIT_SIZE_243];
  flex_trits_from_trytes(latest_milestone_trits, NUM_TRITS_HASH, LATEST_MILESTONE, HASH_LENGTH_TRYTE,
                         HASH_LENGTH_TRYTE);
  memcpy(api.core->consensus.milestone_tracker.latest_milestone, latest_milestone_trits, FLEX_TRIT_SIZE_243);
  flex_trit_t latest_solid_milestone_trits[FLEX_TRIT_SIZE_243];
  flex_trits_from_trytes(latest_solid_milestone_trits, NUM_TRITS_HASH, LATEST_SOLID_MILESTONE, HASH_LENGTH_TRYTE,
                         HASH_LENGTH_TRYTE);
  memcpy(api.core->consensus.milestone_tracker.latest_solid_milestone, latest_solid_milestone_trits,
         FLEX_TRIT_SIZE_243);

  flex_trit_t coordinator_trits[FLEX_TRIT_SIZE_243];
  flex_trits_from_trytes(coordinator_trits, NUM_TRITS_HASH, COORDINATOR, HASH_LENGTH_TRYTE, HASH_LENGTH_TRYTE);
  memcpy(api.core->consensus.conf.coordinator_address, coordinator_trits, FLEX_TRIT_SIZE_243);
  // Adding neighbors

  neighbor_t neighbor;
  TEST_ASSERT(neighbor_init_with_uri(&neighbor, "tcp://8.8.8.1:15001") == RC_OK);
  TEST_ASSERT(router_neighbor_add(&api.core->node.router, &neighbor) == RC_OK);
  TEST_ASSERT(neighbor_init_with_uri(&neighbor, "tcp://8.8.8.2:15002") == RC_OK);
  TEST_ASSERT(router_neighbor_add(&api.core->node.router, &neighbor) == RC_OK);

  // Adding tips

  TEST_ASSERT(tips_cache_init(&api.core->node.tips, 10) == RC_OK);
  flex_trit_t hashes[5][FLEX_TRIT_SIZE_243];
  tryte_t trytes[81] =
      "A99999999999999999999999999999999999999999999999999999999999999999999999"
      "999999999";

  for (size_t i = 0; i < 5; i++) {
    flex_trits_from_trytes(hashes[i], HASH_LENGTH_TRIT, trytes, HASH_LENGTH_TRYTE, HASH_LENGTH_TRYTE);
    trytes[0]++;
  }
  TEST_ASSERT(tips_cache_add(&api.core->node.tips, hashes[0]) == RC_OK);
  TEST_ASSERT(tips_cache_add(&api.core->node.tips, hashes[1]) == RC_OK);
  TEST_ASSERT(tips_cache_add(&api.core->node.tips, hashes[2]) == RC_OK);
  TEST_ASSERT(tips_cache_add(&api.core->node.tips, hashes[3]) == RC_OK);
  TEST_ASSERT(tips_cache_add(&api.core->node.tips, hashes[4]) == RC_OK);

  // Adding requests

  TEST_ASSERT(request_transaction(&api.core->node.transaction_requester, &tangle, hashes[0]) == RC_OK);
  TEST_ASSERT(request_transaction(&api.core->node.transaction_requester, &tangle, hashes[1]) == RC_OK);
  TEST_ASSERT(request_transaction(&api.core->node.transaction_requester, &tangle, hashes[2]) == RC_OK);
  // Adding broadcasts

  protocol_gossip_t packet;
  TEST_ASSERT(broadcaster_stage_add(&api.core->node.broadcaster, &packet) == RC_OK);
  TEST_ASSERT(broadcaster_stage_add(&api.core->node.broadcaster, &packet) == RC_OK);
  TEST_ASSERT(broadcaster_stage_add(&api.core->node.broadcaster, &packet) == RC_OK);
  TEST_ASSERT(broadcaster_stage_add(&api.core->node.broadcaster, &packet) == RC_OK);

  RUN_TEST(test_get_node_info);

  TEST_ASSERT(router_destroy(&api.core->node.router) == RC_OK);
  TEST_ASSERT(requester_destroy(&api.core->node.transaction_requester) == RC_OK);
  TEST_ASSERT(tangle_cleanup(&tangle, tangle_test_db_path) == RC_OK);

  TEST_ASSERT(storage_destroy() == RC_OK);
  return UNITY_END();
}
