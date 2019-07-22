/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "cclient/api/tests/cclient_test_defs.h"

static iota_client_service_t g_serv;

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

  cclient_service_setup(&g_serv);

  RUN_TEST(test_get_balances_empty);
  RUN_TEST(test_get_balances);
  RUN_TEST(test_get_balances_with_tip);
  RUN_TEST(test_get_balances_invalid_tip);

  cclient_service_cleanup(&g_serv);

  return UNITY_END();
}
