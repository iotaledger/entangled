/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "cclient/api/tests/cclient_conf.h"

static iota_client_service_t g_serv;

static void service_setup(void) {
  // init service
  g_serv.http.path = "/";
  g_serv.http.content_type = "application/json";
  g_serv.http.accept = "application/json";
  g_serv.http.host = NODE_HOST;
  g_serv.http.port = NODE_PORT;
  g_serv.http.api_version = 1;
  g_serv.serializer_type = SR_JSON;
  g_serv.http.ca_pem = amazon_ca1_pem;

  logger_helper_init(LOGGER_DEBUG);
  iota_client_core_init(&g_serv);
}

static void service_cleanup(void) {
  iota_client_core_destroy(&g_serv);
  logger_helper_destroy();
}

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

  RUN_TEST(service_setup);

  RUN_TEST(test_get_node_info);

  RUN_TEST(service_cleanup);

  return UNITY_END();
}
