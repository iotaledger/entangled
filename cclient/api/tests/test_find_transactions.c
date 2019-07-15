
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

static void test_find_tx_empty(void) {
  find_transactions_req_t *find_tx_req = find_transactions_req_new();
  TEST_ASSERT_NOT_NULL(find_tx_req);
  TEST_ASSERT_NULL(find_tx_req->bundles);
  TEST_ASSERT_NULL(find_tx_req->addresses);
  TEST_ASSERT_NULL(find_tx_req->tags);
  TEST_ASSERT_NULL(find_tx_req->approvees);

  find_transactions_res_t *find_tx_res = find_transactions_res_new();
  TEST_ASSERT_NOT_NULL(find_tx_res);

  TEST_ASSERT_EQUAL_INT16(RC_NULL_PARAM, iota_client_find_transactions(&g_serv, find_tx_req, find_tx_res));
  TEST_ASSERT_NULL(find_tx_res->hashes);

  find_transactions_req_free(&find_tx_req);
  TEST_ASSERT_NULL(find_tx_req);
  find_transactions_res_free(&find_tx_res);
  TEST_ASSERT_NULL(find_tx_res);
}

static void test_find_tx_by_bundle(void) {
  flex_trit_t flex_hash[NUM_FLEX_TRITS_HASH];
  find_transactions_req_t *find_tx_req = find_transactions_req_new();
  TEST_ASSERT_NOT_NULL(find_tx_req);
  find_transactions_res_t *find_tx_res = find_transactions_res_new();
  TEST_ASSERT_NOT_NULL(find_tx_res);

  TEST_ASSERT(flex_trits_from_trytes(flex_hash, NUM_TRITS_HASH, TEST_BUNDLE_HASH_0, NUM_TRYTES_HASH, NUM_TRYTES_HASH) !=
              0);
  TEST_ASSERT_EQUAL_INT16(RC_OK, find_transactions_req_bundle_add(find_tx_req, flex_hash));

  TEST_ASSERT_EQUAL_INT16(RC_OK, iota_client_find_transactions(&g_serv, find_tx_req, find_tx_res));

  find_transactions_req_free(&find_tx_req);
  TEST_ASSERT_NULL(find_tx_req);
  find_transactions_res_free(&find_tx_res);
  TEST_ASSERT_NULL(find_tx_res);
}

static void test_find_tx_by_empty_bundle(void) {
  flex_trit_t flex_hash[NUM_FLEX_TRITS_HASH];
  find_transactions_req_t *find_tx_req = find_transactions_req_new();
  TEST_ASSERT_NOT_NULL(find_tx_req);
  find_transactions_res_t *find_tx_res = find_transactions_res_new();
  TEST_ASSERT_NOT_NULL(find_tx_res);

  memset(flex_hash, FLEX_TRIT_NULL_VALUE, NUM_FLEX_TRITS_HASH);
  TEST_ASSERT_EQUAL_INT16(RC_OK, find_transactions_req_bundle_add(find_tx_req, flex_hash));

  TEST_ASSERT_EQUAL_INT16(RC_CCLIENT_RES_ERROR, iota_client_find_transactions(&g_serv, find_tx_req, find_tx_res));
  TEST_ASSERT_NULL(find_tx_res->hashes);

  find_transactions_req_free(&find_tx_req);
  TEST_ASSERT_NULL(find_tx_req);
  find_transactions_res_free(&find_tx_res);
  TEST_ASSERT_NULL(find_tx_res);
}

static void test_find_tx_by_address(void) {
  flex_trit_t flex_addr[NUM_FLEX_TRITS_ADDRESS];
  find_transactions_req_t *find_tx_req = find_transactions_req_new();
  TEST_ASSERT_NOT_NULL(find_tx_req);
  find_transactions_res_t *find_tx_res = find_transactions_res_new();
  TEST_ASSERT_NOT_NULL(find_tx_res);

  TEST_ASSERT(flex_trits_from_trytes(flex_addr, NUM_TRITS_ADDRESS, TEST_ADDRESS_0, NUM_TRYTES_ADDRESS,
                                     NUM_TRYTES_ADDRESS) != 0);
  TEST_ASSERT_EQUAL_INT16(RC_OK, find_transactions_req_address_add(find_tx_req, flex_addr));

  TEST_ASSERT_EQUAL_INT16(RC_OK, iota_client_find_transactions(&g_serv, find_tx_req, find_tx_res));

  find_transactions_req_free(&find_tx_req);
  TEST_ASSERT_NULL(find_tx_req);
  find_transactions_res_free(&find_tx_res);
  TEST_ASSERT_NULL(find_tx_res);
}

static void test_find_tx_by_empty_address(void) {
  flex_trit_t flex_addr[NUM_FLEX_TRITS_ADDRESS];
  find_transactions_req_t *find_tx_req = find_transactions_req_new();
  TEST_ASSERT_NOT_NULL(find_tx_req);
  find_transactions_res_t *find_tx_res = find_transactions_res_new();
  TEST_ASSERT_NOT_NULL(find_tx_res);

  memset(flex_addr, FLEX_TRIT_NULL_VALUE, NUM_FLEX_TRITS_ADDRESS);
  TEST_ASSERT_EQUAL_INT16(RC_OK, find_transactions_req_address_add(find_tx_req, flex_addr));

  TEST_ASSERT_EQUAL_INT16(RC_CCLIENT_RES_ERROR, iota_client_find_transactions(&g_serv, find_tx_req, find_tx_res));
  TEST_ASSERT_NULL(find_tx_res->hashes);

  find_transactions_req_free(&find_tx_req);
  TEST_ASSERT_NULL(find_tx_req);
  find_transactions_res_free(&find_tx_res);
  TEST_ASSERT_NULL(find_tx_res);
}

static void test_find_tx_by_tag(void) {
  flex_trit_t flex_tag[NUM_FLEX_TRITS_TAG];
  find_transactions_req_t *find_tx_req = find_transactions_req_new();
  TEST_ASSERT_NOT_NULL(find_tx_req);
  find_transactions_res_t *find_tx_res = find_transactions_res_new();
  TEST_ASSERT_NOT_NULL(find_tx_res);

  TEST_ASSERT(flex_trits_from_trytes(flex_tag, NUM_TRITS_TAG, TEST_TAG_0, NUM_TRYTES_TAG, NUM_TRYTES_TAG) != 0);
  TEST_ASSERT_EQUAL_INT16(RC_OK, find_transactions_req_tag_add(find_tx_req, flex_tag));

  TEST_ASSERT_EQUAL_INT16(RC_OK, iota_client_find_transactions(&g_serv, find_tx_req, find_tx_res));

  find_transactions_req_free(&find_tx_req);
  TEST_ASSERT_NULL(find_tx_req);
  find_transactions_res_free(&find_tx_res);
  TEST_ASSERT_NULL(find_tx_res);
}

static void test_find_tx_by_empty_tag(void) {
  flex_trit_t flex_tag[NUM_FLEX_TRITS_TAG];
  find_transactions_req_t *find_tx_req = find_transactions_req_new();
  TEST_ASSERT_NOT_NULL(find_tx_req);
  find_transactions_res_t *find_tx_res = find_transactions_res_new();
  TEST_ASSERT_NOT_NULL(find_tx_res);

  memset(flex_tag, FLEX_TRIT_NULL_VALUE, NUM_FLEX_TRITS_TAG);
  TEST_ASSERT_EQUAL_INT16(RC_OK, find_transactions_req_tag_add(find_tx_req, flex_tag));

  TEST_ASSERT_EQUAL_INT16(RC_CCLIENT_RES_ERROR, iota_client_find_transactions(&g_serv, find_tx_req, find_tx_res));
  TEST_ASSERT_NULL(find_tx_res->hashes);

  find_transactions_req_free(&find_tx_req);
  TEST_ASSERT_NULL(find_tx_req);
  find_transactions_res_free(&find_tx_res);
  TEST_ASSERT_NULL(find_tx_res);
}

static void test_find_tx_by_approvee(void) {
  flex_trit_t flex_hash[NUM_FLEX_TRITS_HASH];
  find_transactions_req_t *find_tx_req = find_transactions_req_new();
  TEST_ASSERT_NOT_NULL(find_tx_req);
  find_transactions_res_t *find_tx_res = find_transactions_res_new();
  TEST_ASSERT_NOT_NULL(find_tx_res);

  TEST_ASSERT(flex_trits_from_trytes(flex_hash, NUM_TRITS_HASH, TEST_BUNDLE_TX_1, NUM_TRYTES_HASH, NUM_TRYTES_HASH) !=
              0);
  TEST_ASSERT_EQUAL_INT16(RC_OK, find_transactions_req_approvee_add(find_tx_req, flex_hash));

  TEST_ASSERT_EQUAL_INT16(RC_OK, iota_client_find_transactions(&g_serv, find_tx_req, find_tx_res));

  find_transactions_req_free(&find_tx_req);
  TEST_ASSERT_NULL(find_tx_req);
  find_transactions_res_free(&find_tx_res);
  TEST_ASSERT_NULL(find_tx_res);
}

static void test_find_tx_by_empty_approvee(void) {
  flex_trit_t flex_hash[NUM_FLEX_TRITS_HASH];
  find_transactions_req_t *find_tx_req = find_transactions_req_new();
  TEST_ASSERT_NOT_NULL(find_tx_req);
  find_transactions_res_t *find_tx_res = find_transactions_res_new();
  TEST_ASSERT_NOT_NULL(find_tx_res);

  memset(flex_hash, FLEX_TRIT_NULL_VALUE, NUM_FLEX_TRITS_HASH);
  TEST_ASSERT_EQUAL_INT16(RC_OK, find_transactions_req_approvee_add(find_tx_req, flex_hash));

  TEST_ASSERT_EQUAL_INT16(RC_CCLIENT_RES_ERROR, iota_client_find_transactions(&g_serv, find_tx_req, find_tx_res));
  TEST_ASSERT_NULL(find_tx_res->hashes);

  find_transactions_req_free(&find_tx_req);
  TEST_ASSERT_NULL(find_tx_req);
  find_transactions_res_free(&find_tx_res);
  TEST_ASSERT_NULL(find_tx_res);
}

int main() {
  UNITY_BEGIN();

  RUN_TEST(service_setup);

  RUN_TEST(test_find_tx_empty);
  RUN_TEST(test_find_tx_by_bundle);
  RUN_TEST(test_find_tx_by_empty_bundle);
  RUN_TEST(test_find_tx_by_address);
  RUN_TEST(test_find_tx_by_empty_address);
  RUN_TEST(test_find_tx_by_tag);
  RUN_TEST(test_find_tx_by_empty_tag);
  RUN_TEST(test_find_tx_by_approvee);
  RUN_TEST(test_find_tx_by_empty_approvee);

  RUN_TEST(service_cleanup);

  return UNITY_END();
}
