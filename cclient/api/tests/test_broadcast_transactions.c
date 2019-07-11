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

static void test_broadcast_transactions_empty(void) {
  broadcast_transactions_req_t *bc_tx_req = broadcast_transactions_req_new();
  TEST_ASSERT_NOT_NULL(bc_tx_req);
  TEST_ASSERT_NULL(bc_tx_req->trytes);

  TEST_ASSERT_EQUAL_INT16(RC_NULL_PARAM, iota_client_broadcast_transactions(&g_serv, bc_tx_req));

  broadcast_transactions_req_free(&bc_tx_req);
  TEST_ASSERT_NULL(bc_tx_req);
}

int main() {
  UNITY_BEGIN();

  RUN_TEST(service_setup);

  RUN_TEST(test_broadcast_transactions_empty);

  RUN_TEST(service_cleanup);

  return UNITY_END();
}
