/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "cclient/api/tests/cclient_test_defs.h"

static iota_client_service_t g_serv;

static void test_get_node_info(void) {
  get_node_info_res_t *node_res = get_node_info_res_new();
  TEST_ASSERT_NOT_NULL(node_res);

  TEST_ASSERT_EQUAL_INT16(RC_OK, iota_client_get_node_info(&g_serv, node_res));

  // node info response
  TEST_ASSERT(strlen(get_node_info_res_app_name(node_res)) > 0);
  TEST_ASSERT(strlen(get_node_info_res_app_version(node_res)) > 0);
  TEST_ASSERT(!flex_trits_are_null(get_node_info_res_lm(node_res), NUM_FLEX_TRITS_HASH));
  TEST_ASSERT(node_res->latest_milestone_index > STARTING_MILESTONE_INDEX);
  TEST_ASSERT(!flex_trits_are_null(get_node_info_res_lssm(node_res), NUM_FLEX_TRITS_HASH));
  TEST_ASSERT(node_res->latest_solid_subtangle_milestone_index > STARTING_MILESTONE_INDEX);
  TEST_ASSERT(node_res->time > OLDER_TIMESTAMP);
  TEST_ASSERT(!flex_trits_are_null(get_node_info_res_coordinator_address(node_res), NUM_FLEX_TRITS_HASH));
  TEST_ASSERT(get_node_info_req_features_len(node_res) > 0);

  get_node_info_res_free(&node_res);
  TEST_ASSERT_NULL(node_res);
}

int main() {
  UNITY_BEGIN();

  cclient_service_setup(&g_serv);

  RUN_TEST(test_get_node_info);

  cclient_service_cleanup(&g_serv);

  return UNITY_END();
}
