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

static void test_get_inclusion_status_empty(void) {
  get_inclusion_states_req_t *get_inclusion_req = get_inclusion_states_req_new();
  TEST_ASSERT_NOT_NULL(get_inclusion_req);
  TEST_ASSERT_NULL(get_inclusion_req->transactions);
  TEST_ASSERT_NULL(get_inclusion_req->tips);

  get_inclusion_states_res_t *get_inclusion_res = get_inclusion_states_res_new();
  TEST_ASSERT_NOT_NULL(get_inclusion_res);

  TEST_ASSERT_EQUAL_INT16(RC_NULL_PARAM,
                          iota_client_get_inclusion_states(&g_serv, get_inclusion_req, get_inclusion_res));
  TEST_ASSERT_NULL(get_inclusion_res->states);
  TEST_ASSERT_EQUAL_INT(0, get_inclusion_states_res_states_count(get_inclusion_res));

  get_inclusion_states_req_free(&get_inclusion_req);
  TEST_ASSERT_NULL(get_inclusion_req);
  get_inclusion_states_res_free(&get_inclusion_res);
  TEST_ASSERT_NULL(get_inclusion_res);
}

static void test_get_inclusion_status(void) {
  flex_trit_t flex_hash[NUM_FLEX_TRITS_HASH];
  get_inclusion_states_req_t *get_inclusion_req = get_inclusion_states_req_new();
  get_inclusion_states_res_t *get_inclusion_res = get_inclusion_states_res_new();
  TEST_ASSERT_NOT_NULL(get_inclusion_req);
  TEST_ASSERT_NOT_NULL(get_inclusion_res);

  TEST_ASSERT(flex_trits_from_trytes(flex_hash, NUM_TRITS_HASH, TEST_BUNDLE_TX_0, NUM_TRYTES_HASH, NUM_TRYTES_HASH) !=
              0);
  TEST_ASSERT_EQUAL_INT16(RC_OK, get_inclusion_states_req_hash_add(get_inclusion_req, flex_hash));

  TEST_ASSERT(flex_trits_from_trytes(flex_hash, NUM_TRITS_HASH, TEST_MILESTONE_0, NUM_TRYTES_HASH, NUM_TRYTES_HASH) !=
              0);
  TEST_ASSERT_EQUAL_INT16(RC_OK, get_inclusion_states_req_tip_add(get_inclusion_req, flex_hash));

  TEST_ASSERT_EQUAL_INT16(RC_OK, iota_client_get_inclusion_states(&g_serv, get_inclusion_req, get_inclusion_res));
  TEST_ASSERT_NOT_NULL(get_inclusion_res->states);
  TEST_ASSERT(get_inclusion_states_res_states_count(get_inclusion_res) > 0);

  get_inclusion_states_req_free(&get_inclusion_req);
  TEST_ASSERT_NULL(get_inclusion_req);
  get_inclusion_states_res_free(&get_inclusion_res);
  TEST_ASSERT_NULL(get_inclusion_res);
}

static void test_get_inclusion_status_without_tip(void) {
  flex_trit_t flex_hash[NUM_FLEX_TRITS_HASH];
  get_inclusion_states_req_t *get_inclusion_req = get_inclusion_states_req_new();
  get_inclusion_states_res_t *get_inclusion_res = get_inclusion_states_res_new();
  TEST_ASSERT_NOT_NULL(get_inclusion_req);
  TEST_ASSERT_NOT_NULL(get_inclusion_res);

  TEST_ASSERT(flex_trits_from_trytes(flex_hash, NUM_TRITS_HASH, TEST_BUNDLE_TX_0, NUM_TRYTES_HASH, NUM_TRYTES_HASH) !=
              0);
  TEST_ASSERT_EQUAL_INT16(RC_OK, get_inclusion_states_req_hash_add(get_inclusion_req, flex_hash));

  TEST_ASSERT_EQUAL_INT16(RC_CCLIENT_RES_ERROR,
                          iota_client_get_inclusion_states(&g_serv, get_inclusion_req, get_inclusion_res));
  TEST_ASSERT_NULL(get_inclusion_res->states);

  get_inclusion_states_req_free(&get_inclusion_req);
  TEST_ASSERT_NULL(get_inclusion_req);
  get_inclusion_states_res_free(&get_inclusion_res);
  TEST_ASSERT_NULL(get_inclusion_res);
}

int main() {
  UNITY_BEGIN();

  RUN_TEST(service_setup);

  RUN_TEST(test_get_inclusion_status_empty);
  RUN_TEST(test_get_inclusion_status);
  RUN_TEST(test_get_inclusion_status_without_tip);

  RUN_TEST(service_cleanup);

  return UNITY_END();
}