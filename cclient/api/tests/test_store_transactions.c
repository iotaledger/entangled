/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "cclient/api/tests/cclient_test_defs.h"

static iota_client_service_t g_serv;

static void test_store_transactions_empty(void) {
  store_transactions_req_t *store_tx_req = store_transactions_req_new();
  TEST_ASSERT_NOT_NULL(store_tx_req);
  TEST_ASSERT_NOT_NULL(store_tx_req->trytes);

  TEST_ASSERT_EQUAL_INT16(RC_NULL_PARAM, iota_client_store_transactions(&g_serv, store_tx_req));

  store_transactions_req_free(&store_tx_req);
  TEST_ASSERT_NULL(store_tx_req);
}

int main() {
  UNITY_BEGIN();

  cclient_service_setup(&g_serv);

  RUN_TEST(test_store_transactions_empty);

  cclient_service_cleanup(&g_serv);

  return UNITY_END();
}
