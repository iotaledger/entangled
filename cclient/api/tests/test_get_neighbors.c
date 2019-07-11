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

  logger_helper_init();
  iota_client_core_init(&g_serv);
}

static void service_cleanup(void) {
  iota_client_core_destroy(&g_serv);
  logger_helper_destroy();
}

static void test_get_neighbors(void) {
  retcode_t ret = RC_ERROR;
  get_neighbors_res_t *nb_res = get_neighbors_res_new();
  TEST_ASSERT_NOT_NULL(nb_res);
  TEST_ASSERT_EQUAL_UINT32(0, get_neighbors_res_num(nb_res));

  ret = iota_client_get_neighbors(&g_serv, nb_res);
  if (ret == RC_OK) {
    TEST_ASSERT(get_neighbors_res_num(nb_res) > 0);

  } else if (ret == RC_CCLIENT_RES_ERROR) {
    // getNeighbors is not available on this node
    TEST_ASSERT_EQUAL_UINT32(0, get_neighbors_res_num(nb_res));

  } else {
    // unknow
    TEST_ASSERT(1);
  }

  get_neighbors_res_free(&nb_res);
  TEST_ASSERT_NULL(nb_res);
}

int main() {
  UNITY_BEGIN();

  RUN_TEST(service_setup);

  RUN_TEST(test_get_neighbors);

  RUN_TEST(service_cleanup);

  return UNITY_END();
}
