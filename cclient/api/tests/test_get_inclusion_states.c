/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "cclient/api/tests/cclient_test_defs.h"

static iota_client_service_t g_serv;

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

  TEST_ASSERT_EQUAL_INT16(RC_OK, iota_client_get_inclusion_states(&g_serv, get_inclusion_req, get_inclusion_res));
  TEST_ASSERT_NOT_NULL(get_inclusion_res->states);

  get_inclusion_states_req_free(&get_inclusion_req);
  TEST_ASSERT_NULL(get_inclusion_req);
  get_inclusion_states_res_free(&get_inclusion_res);
  TEST_ASSERT_NULL(get_inclusion_res);
}

int main() {
  UNITY_BEGIN();

  cclient_service_setup(&g_serv);

  RUN_TEST(test_get_inclusion_status_empty);
  RUN_TEST(test_get_inclusion_status);
  RUN_TEST(test_get_inclusion_status_without_tip);

  cclient_service_cleanup(&g_serv);

  return UNITY_END();
}
