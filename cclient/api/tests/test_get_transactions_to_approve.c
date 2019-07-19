/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "cclient/api/tests/cclient_test_defs.h"

static iota_client_service_t g_serv;

static void test_get_transactions_to_approve_empty(void) {
  get_transactions_to_approve_req_t *tx_approve_req = get_transactions_to_approve_req_new();
  TEST_ASSERT_NOT_NULL(tx_approve_req);
  TEST_ASSERT_EQUAL_UINT32(0, tx_approve_req->depth);

  get_transactions_to_approve_res_t *tx_approve_res = get_transactions_to_approve_res_new();
  TEST_ASSERT_NOT_NULL(tx_approve_res);
  TEST_ASSERT(flex_trits_are_null(get_transactions_to_approve_res_branch(tx_approve_res), NUM_FLEX_TRITS_HASH));
  TEST_ASSERT(flex_trits_are_null(get_transactions_to_approve_res_trunk(tx_approve_res), NUM_FLEX_TRITS_HASH));

  TEST_ASSERT_EQUAL_INT16(RC_OK, iota_client_get_transactions_to_approve(&g_serv, tx_approve_req, tx_approve_res));

  TEST_ASSERT(!flex_trits_are_null(get_transactions_to_approve_res_branch(tx_approve_res), NUM_FLEX_TRITS_HASH));
  TEST_ASSERT(!flex_trits_are_null(get_transactions_to_approve_res_trunk(tx_approve_res), NUM_FLEX_TRITS_HASH));

  get_transactions_to_approve_req_free(&tx_approve_req);
  TEST_ASSERT_NULL(tx_approve_req);
  get_transactions_to_approve_res_free(&tx_approve_res);
  TEST_ASSERT_NULL(tx_approve_res);
}

static void test_get_transactions_to_approve_invalid_depth(void) {
  get_transactions_to_approve_req_t *tx_approve_req = get_transactions_to_approve_req_new();
  TEST_ASSERT_NOT_NULL(tx_approve_req);
  TEST_ASSERT_EQUAL_UINT32(0, tx_approve_req->depth);

  get_transactions_to_approve_res_t *tx_approve_res = get_transactions_to_approve_res_new();
  TEST_ASSERT_NOT_NULL(tx_approve_res);
  TEST_ASSERT(flex_trits_are_null(get_transactions_to_approve_res_branch(tx_approve_res), NUM_FLEX_TRITS_HASH));
  TEST_ASSERT(flex_trits_are_null(get_transactions_to_approve_res_trunk(tx_approve_res), NUM_FLEX_TRITS_HASH));

  tx_approve_req->depth = UINT32_MAX;

  TEST_ASSERT_EQUAL_INT16(RC_CCLIENT_RES_ERROR,
                          iota_client_get_transactions_to_approve(&g_serv, tx_approve_req, tx_approve_res));

  TEST_ASSERT(flex_trits_are_null(get_transactions_to_approve_res_branch(tx_approve_res), NUM_FLEX_TRITS_HASH));
  TEST_ASSERT(flex_trits_are_null(get_transactions_to_approve_res_trunk(tx_approve_res), NUM_FLEX_TRITS_HASH));

  get_transactions_to_approve_req_free(&tx_approve_req);
  TEST_ASSERT_NULL(tx_approve_req);
  get_transactions_to_approve_res_free(&tx_approve_res);
  TEST_ASSERT_NULL(tx_approve_res);
}

static void test_get_transactions_to_approve(void) {
  flex_trit_t reference[FLEX_TRIT_SIZE_243];
  get_transactions_to_approve_req_t *tx_approve_req = get_transactions_to_approve_req_new();
  TEST_ASSERT_NOT_NULL(tx_approve_req);
  TEST_ASSERT_EQUAL_UINT32(0, tx_approve_req->depth);

  get_transactions_to_approve_res_t *tx_approve_res = get_transactions_to_approve_res_new();
  TEST_ASSERT_NOT_NULL(tx_approve_res);
  TEST_ASSERT(flex_trits_are_null(get_transactions_to_approve_res_branch(tx_approve_res), NUM_FLEX_TRITS_HASH));
  TEST_ASSERT(flex_trits_are_null(get_transactions_to_approve_res_trunk(tx_approve_res), NUM_FLEX_TRITS_HASH));

  TEST_ASSERT(flex_trits_from_trytes(reference, NUM_TRITS_HASH, TEST_BUNDLE_TX_0, NUM_TRYTES_HASH, NUM_TRYTES_HASH) !=
              0);

  TEST_ASSERT_EQUAL_INT16(RC_OK, get_transactions_to_approve_req_set_reference(tx_approve_req, reference));

  tx_approve_req->depth = 3;

  // reference transaction is too old
  TEST_ASSERT_EQUAL_INT16(RC_CCLIENT_RES_ERROR,
                          iota_client_get_transactions_to_approve(&g_serv, tx_approve_req, tx_approve_res));

  TEST_ASSERT(flex_trits_are_null(get_transactions_to_approve_res_branch(tx_approve_res), NUM_FLEX_TRITS_HASH));
  TEST_ASSERT(flex_trits_are_null(get_transactions_to_approve_res_trunk(tx_approve_res), NUM_FLEX_TRITS_HASH));

  get_transactions_to_approve_req_free(&tx_approve_req);
  TEST_ASSERT_NULL(tx_approve_req);
  get_transactions_to_approve_res_free(&tx_approve_res);
  TEST_ASSERT_NULL(tx_approve_res);
}

int main() {
  UNITY_BEGIN();

  cclient_service_setup(&g_serv);

  RUN_TEST(test_get_transactions_to_approve_empty);
  RUN_TEST(test_get_transactions_to_approve_invalid_depth);
  RUN_TEST(test_get_transactions_to_approve);

  cclient_service_cleanup(&g_serv);

  return UNITY_END();
}
