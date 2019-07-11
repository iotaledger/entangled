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

static void test_check_consistency_empty(void) {
  check_consistency_req_t *consistency_req = check_consistency_req_new();
  check_consistency_res_t *consistency_res = check_consistency_res_new();
  TEST_ASSERT_NOT_NULL(consistency_req);
  TEST_ASSERT_NULL(consistency_req->tails);
  TEST_ASSERT_NOT_NULL(consistency_res);

  TEST_ASSERT_EQUAL_INT16(RC_NULL_PARAM, iota_client_check_consistency(&g_serv, consistency_req, consistency_res));
  TEST_ASSERT_NULL(consistency_res->info);
  TEST_ASSERT_FALSE(consistency_res->state);

  check_consistency_req_free(&consistency_req);
  TEST_ASSERT_NULL(consistency_req);
  check_consistency_res_free(&consistency_res);
  TEST_ASSERT_NULL(consistency_res);
}

static void test_check_consistency_tail(void) {
  flex_trit_t flex_tx[NUM_FLEX_TRITS_HASH];
  check_consistency_req_t *consistency_req = check_consistency_req_new();
  check_consistency_res_t *consistency_res = check_consistency_res_new();
  TEST_ASSERT_NOT_NULL(consistency_req);
  TEST_ASSERT_NOT_NULL(consistency_res);

  TEST_ASSERT(flex_trits_from_trytes(flex_tx, NUM_TRITS_HASH, TEST_BUNDLE_TX_0, NUM_TRYTES_HASH, NUM_TRYTES_HASH) != 0);
  TEST_ASSERT_EQUAL_INT16(RC_OK, check_consistency_req_tails_add(consistency_req, flex_tx));

  TEST_ASSERT_EQUAL_INT16(RC_OK, iota_client_check_consistency(&g_serv, consistency_req, consistency_res));
  TEST_ASSERT_NOT_NULL(consistency_res->info);

  check_consistency_req_free(&consistency_req);
  TEST_ASSERT_NULL(consistency_req);
  check_consistency_res_free(&consistency_res);
  TEST_ASSERT_NULL(consistency_res);
}

static void test_check_consistency_not_tail(void) {
  flex_trit_t flex_tx[NUM_FLEX_TRITS_HASH];
  check_consistency_req_t *consistency_req = check_consistency_req_new();
  check_consistency_res_t *consistency_res = check_consistency_res_new();
  TEST_ASSERT_NOT_NULL(consistency_req);
  TEST_ASSERT_NOT_NULL(consistency_res);

  TEST_ASSERT(flex_trits_from_trytes(flex_tx, NUM_TRITS_HASH, TEST_BUNDLE_TX_1, NUM_TRYTES_HASH, NUM_TRYTES_HASH) != 0);
  TEST_ASSERT_EQUAL_INT16(RC_OK, check_consistency_req_tails_add(consistency_req, flex_tx));

  TEST_ASSERT_EQUAL_INT16(RC_CCLIENT_RES_ERROR,
                          iota_client_check_consistency(&g_serv, consistency_req, consistency_res));
  TEST_ASSERT_NULL(consistency_res->info);

  check_consistency_req_free(&consistency_req);
  TEST_ASSERT_NULL(consistency_req);
  check_consistency_res_free(&consistency_res);
  TEST_ASSERT_NULL(consistency_res);
}

static void test_check_consistency_empty_tail(void) {
  flex_trit_t flex_tx[NUM_FLEX_TRITS_HASH];
  check_consistency_req_t *consistency_req = check_consistency_req_new();
  check_consistency_res_t *consistency_res = check_consistency_res_new();
  TEST_ASSERT_NOT_NULL(consistency_req);
  TEST_ASSERT_NOT_NULL(consistency_res);

  memset(flex_tx, FLEX_TRIT_NULL_VALUE, NUM_FLEX_TRITS_HASH);
  TEST_ASSERT_EQUAL_INT16(RC_OK, check_consistency_req_tails_add(consistency_req, flex_tx));

  TEST_ASSERT_EQUAL_INT16(RC_CCLIENT_RES_ERROR,
                          iota_client_check_consistency(&g_serv, consistency_req, consistency_res));
  TEST_ASSERT_NULL(consistency_res->info);

  check_consistency_req_free(&consistency_req);
  TEST_ASSERT_NULL(consistency_req);
  check_consistency_res_free(&consistency_res);
  TEST_ASSERT_NULL(consistency_res);
}

int main() {
  UNITY_BEGIN();

  RUN_TEST(service_setup);

  RUN_TEST(test_check_consistency_empty);
  RUN_TEST(test_check_consistency_tail);
  RUN_TEST(test_check_consistency_not_tail);
  RUN_TEST(test_check_consistency_empty_tail);

  RUN_TEST(service_cleanup);

  return UNITY_END();
}
