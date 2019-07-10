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

static void test_get_balances_empty(void) {
  get_balances_req_t *balance_req = get_balances_req_new();
  TEST_ASSERT_NOT_NULL(balance_req);
  TEST_ASSERT_EQUAL_INT8(0, balance_req->threshold);
  TEST_ASSERT_NULL(balance_req->addresses);
  TEST_ASSERT_NULL(balance_req->tips);

  get_balances_res_t *balance_res = get_balances_res_new();
  TEST_ASSERT_NOT_NULL(balance_res);

  TEST_ASSERT_EQUAL_INT16(RC_NULL_PARAM, iota_client_get_balances(&g_serv, balance_req, balance_res));

  TEST_ASSERT_EQUAL_INT(0, get_balances_res_balances_num(balance_res));
  TEST_ASSERT_NULL(balance_res->references);
  TEST_ASSERT_EQUAL_UINT64(0, balance_res->milestone_index);

  get_balances_req_free(&balance_req);
  TEST_ASSERT_NULL(balance_req);
  get_balances_res_free(&balance_res);
  TEST_ASSERT_NULL(balance_res);
}

static void test_get_balances(void) {
  flex_trit_t flex_addr[NUM_FLEX_TRITS_ADDRESS];
  get_balances_req_t *balance_req = get_balances_req_new();
  TEST_ASSERT_NOT_NULL(balance_req);
  get_balances_res_t *balance_res = get_balances_res_new();
  TEST_ASSERT_NOT_NULL(balance_res);

  TEST_ASSERT(flex_trits_from_trytes(flex_addr, NUM_TRITS_ADDRESS, TEST_ADDRESS_0, NUM_TRYTES_ADDRESS,
                                     NUM_TRYTES_ADDRESS) != 0);
  TEST_ASSERT_EQUAL_INT16(RC_OK, get_balances_req_address_add(balance_req, flex_addr));

  balance_req->threshold = 100;

  TEST_ASSERT_EQUAL_INT16(RC_OK, iota_client_get_balances(&g_serv, balance_req, balance_res));

  TEST_ASSERT(get_balances_res_balances_num(balance_res) != 0);
  TEST_ASSERT_NOT_NULL(balance_res->references);
  TEST_ASSERT(!flex_trits_are_null(balance_res->references->hash, NUM_FLEX_TRITS_HASH));
  TEST_ASSERT(balance_res->milestone_index > STARTING_MILESTONE_INDEX);

  get_balances_req_free(&balance_req);
  TEST_ASSERT_NULL(balance_req);
  get_balances_res_free(&balance_res);
  TEST_ASSERT_NULL(balance_res);
}

static void test_get_balances_with_tip(void) {
  flex_trit_t flex_addr[NUM_FLEX_TRITS_ADDRESS];
  flex_trit_t flex_tip[NUM_FLEX_TRITS_HASH];
  get_balances_req_t *balance_req = get_balances_req_new();
  TEST_ASSERT_NOT_NULL(balance_req);
  get_balances_res_t *balance_res = get_balances_res_new();
  TEST_ASSERT_NOT_NULL(balance_res);

  TEST_ASSERT(flex_trits_from_trytes(flex_addr, NUM_TRITS_ADDRESS, TEST_ADDRESS_0, NUM_TRYTES_ADDRESS,
                                     NUM_TRYTES_ADDRESS) != 0);
  TEST_ASSERT_EQUAL_INT16(RC_OK, get_balances_req_address_add(balance_req, flex_addr));

  TEST_ASSERT(flex_trits_from_trytes(flex_tip, NUM_TRITS_HASH, TEST_BUNDLE_TX_0, NUM_TRYTES_HASH, NUM_TRYTES_HASH) !=
              0);
  TEST_ASSERT_EQUAL_INT16(RC_OK, get_balances_req_tip_add(balance_req, flex_tip));

  balance_req->threshold = 100;

  TEST_ASSERT_EQUAL_INT16(RC_OK, iota_client_get_balances(&g_serv, balance_req, balance_res));

  TEST_ASSERT(get_balances_res_balances_num(balance_res) != 0);
  TEST_ASSERT_NOT_NULL(balance_res->references);
  TEST_ASSERT(!flex_trits_are_null(balance_res->references->hash, NUM_FLEX_TRITS_HASH));
  TEST_ASSERT(balance_res->milestone_index > STARTING_MILESTONE_INDEX);

  get_balances_req_free(&balance_req);
  TEST_ASSERT_NULL(balance_req);
  get_balances_res_free(&balance_res);
  TEST_ASSERT_NULL(balance_res);
}

static void test_get_balances_invalid_tip(void) {
  flex_trit_t flex_addr[NUM_FLEX_TRITS_ADDRESS];
  flex_trit_t flex_tip[NUM_FLEX_TRITS_HASH];
  get_balances_req_t *balance_req = get_balances_req_new();
  TEST_ASSERT_NOT_NULL(balance_req);
  get_balances_res_t *balance_res = get_balances_res_new();
  TEST_ASSERT_NOT_NULL(balance_res);

  TEST_ASSERT(flex_trits_from_trytes(flex_addr, NUM_TRITS_ADDRESS, TEST_ADDRESS_0, NUM_TRYTES_ADDRESS,
                                     NUM_TRYTES_ADDRESS) != 0);
  TEST_ASSERT_EQUAL_INT16(RC_OK, get_balances_req_address_add(balance_req, flex_addr));

  TEST_ASSERT(flex_trits_from_trytes(flex_tip, NUM_TRITS_HASH, TEST_BUNDLE_HASH_0, NUM_TRYTES_HASH, NUM_TRYTES_HASH) !=
              0);
  TEST_ASSERT_EQUAL_INT16(RC_OK, get_balances_req_tip_add(balance_req, flex_tip));

  balance_req->threshold = 100;

  TEST_ASSERT_EQUAL_INT16(RC_CCLIENT_RES_ERROR, iota_client_get_balances(&g_serv, balance_req, balance_res));

  TEST_ASSERT_EQUAL_INT(0, get_balances_res_balances_num(balance_res));
  TEST_ASSERT_NULL(balance_res->references);
  TEST_ASSERT_EQUAL_UINT64(0, balance_res->milestone_index);

  get_balances_req_free(&balance_req);
  TEST_ASSERT_NULL(balance_req);
  get_balances_res_free(&balance_res);
  TEST_ASSERT_NULL(balance_res);
}

int main() {
  UNITY_BEGIN();

  RUN_TEST(service_setup);

  RUN_TEST(test_get_balances_empty);
  RUN_TEST(test_get_balances);
  RUN_TEST(test_get_balances_with_tip);
  RUN_TEST(test_get_balances_invalid_tip);

  RUN_TEST(service_cleanup);

  return UNITY_END();
}
