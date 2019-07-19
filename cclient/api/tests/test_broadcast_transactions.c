/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "cclient/api/tests/cclient_test_defs.h"

static iota_client_service_t g_serv;

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

  cclient_service_setup(&g_serv);

  RUN_TEST(test_broadcast_transactions_empty);

  cclient_service_cleanup(&g_serv);

  return UNITY_END();
}
