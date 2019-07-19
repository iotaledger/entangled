/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "cclient/api/tests/cclient_test_defs.h"

static iota_client_service_t g_serv;

static void test_were_addresses_spent_from_empty(void) {
  were_addresses_spent_from_req_t *addr_spent_req = were_addresses_spent_from_req_new();
  TEST_ASSERT_NOT_NULL(addr_spent_req);
  TEST_ASSERT_NULL(addr_spent_req->addresses);

  were_addresses_spent_from_res_t *addr_spent_res = were_addresses_spent_from_res_new();
  TEST_ASSERT_NOT_NULL(addr_spent_res);
  TEST_ASSERT_NULL(addr_spent_res->states);

  TEST_ASSERT_EQUAL_INT16(RC_NULL_PARAM,
                          iota_client_were_addresses_spent_from(&g_serv, addr_spent_req, addr_spent_res));
  TEST_ASSERT_NULL(addr_spent_res->states);

  were_addresses_spent_from_req_free(&addr_spent_req);
  TEST_ASSERT_NULL(addr_spent_req);
  were_addresses_spent_from_res_free(&addr_spent_res);
  TEST_ASSERT_NULL(addr_spent_res);
}

static void test_were_addresses_spent_from(void) {
  flex_trit_t flex_addr[NUM_FLEX_TRITS_ADDRESS];
  were_addresses_spent_from_req_t *addr_spent_req = were_addresses_spent_from_req_new();
  TEST_ASSERT_NOT_NULL(addr_spent_req);
  TEST_ASSERT_NULL(addr_spent_req->addresses);

  were_addresses_spent_from_res_t *addr_spent_res = were_addresses_spent_from_res_new();
  TEST_ASSERT_NOT_NULL(addr_spent_res);
  TEST_ASSERT_NULL(addr_spent_res->states);

  TEST_ASSERT(flex_trits_from_trytes(flex_addr, NUM_TRITS_HASH, TEST_ADDRESS_0, NUM_TRYTES_HASH, NUM_TRYTES_HASH) != 0);
  TEST_ASSERT_EQUAL_INT16(RC_OK, were_addresses_spent_from_req_add(addr_spent_req, flex_addr));

  TEST_ASSERT_EQUAL_INT16(RC_OK, iota_client_were_addresses_spent_from(&g_serv, addr_spent_req, addr_spent_res));
  TEST_ASSERT_NOT_NULL(addr_spent_res->states);

  were_addresses_spent_from_req_free(&addr_spent_req);
  TEST_ASSERT_NULL(addr_spent_req);
  were_addresses_spent_from_res_free(&addr_spent_res);
  TEST_ASSERT_NULL(addr_spent_res);
}

int main() {
  UNITY_BEGIN();

  cclient_service_setup(&g_serv);

  RUN_TEST(test_were_addresses_spent_from_empty);
  RUN_TEST(test_were_addresses_spent_from);

  cclient_service_cleanup(&g_serv);

  return UNITY_END();
}
