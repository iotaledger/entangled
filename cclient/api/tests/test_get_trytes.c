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

static void test_get_trytes_empty(void) {
  get_trytes_req_t *trytes_req = get_trytes_req_new();
  TEST_ASSERT_NOT_NULL(trytes_req);
  TEST_ASSERT_NULL(trytes_req->hashes);

  get_trytes_res_t *trytes_res = get_trytes_res_new();
  TEST_ASSERT_NOT_NULL(trytes_res);

  TEST_ASSERT_EQUAL_INT16(RC_NULL_PARAM, iota_client_get_trytes(&g_serv, trytes_req, trytes_res));
  TEST_ASSERT_NULL(trytes_res->trytes);

  get_trytes_res_free(&trytes_res);
  TEST_ASSERT_NULL(trytes_res);
  get_trytes_req_free(&trytes_req);
  TEST_ASSERT_NULL(trytes_req);
}

static void test_get_trytes(void) {
  flex_trit_t flex_hash[NUM_FLEX_TRITS_HASH];
  get_trytes_req_t *trytes_req = get_trytes_req_new();
  TEST_ASSERT_NOT_NULL(trytes_req);
  TEST_ASSERT_NULL(trytes_req->hashes);

  get_trytes_res_t *trytes_res = get_trytes_res_new();
  TEST_ASSERT_NOT_NULL(trytes_res);

  TEST_ASSERT(flex_trits_from_trytes(flex_hash, NUM_TRITS_HASH, TEST_BUNDLE_TX_2, NUM_TRYTES_HASH, NUM_TRYTES_HASH) !=
              0);
  TEST_ASSERT_EQUAL_INT16(RC_OK, get_trytes_req_hash_add(trytes_req, flex_hash));

  TEST_ASSERT_EQUAL_INT16(RC_OK, iota_client_get_trytes(&g_serv, trytes_req, trytes_res));
  TEST_ASSERT_NOT_NULL(trytes_res->trytes);

  get_trytes_res_free(&trytes_res);
  TEST_ASSERT_NULL(trytes_res);
  get_trytes_req_free(&trytes_req);
  TEST_ASSERT_NULL(trytes_req);
}

int main() {
  UNITY_BEGIN();

  RUN_TEST(service_setup);

  RUN_TEST(test_get_trytes_empty);
  RUN_TEST(test_get_trytes);

  RUN_TEST(service_cleanup);

  return UNITY_END();
}
