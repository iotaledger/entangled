/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "cclient/api/tests/cclient_test_defs.h"

static iota_client_service_t g_serv;

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

  get_trytes_res_free(&trytes_res);
  TEST_ASSERT_NULL(trytes_res);
  get_trytes_req_free(&trytes_req);
  TEST_ASSERT_NULL(trytes_req);
}

int main() {
  UNITY_BEGIN();

  cclient_service_setup(&g_serv);

  RUN_TEST(test_get_trytes_empty);
  RUN_TEST(test_get_trytes);

  cclient_service_cleanup(&g_serv);

  return UNITY_END();
}
