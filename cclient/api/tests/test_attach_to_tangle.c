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

static void test_attach_to_tangle_empty(void) {
  attach_to_tangle_req_t *attach_req = attach_to_tangle_req_new();
  TEST_ASSERT_NOT_NULL(attach_req);
  TEST_ASSERT_NOT_NULL(attach_req->trytes);
  TEST_ASSERT_EQUAL_UINT8(ATTACH_TO_TANGLE_MAIN_MWM, attach_req->mwm);
  TEST_ASSERT(flex_trits_are_null(attach_to_tangle_req_trunk(attach_req), NUM_FLEX_TRITS_HASH));
  TEST_ASSERT(flex_trits_are_null(attach_to_tangle_req_branch(attach_req), NUM_FLEX_TRITS_HASH));

  attach_to_tangle_res_t *attach_res = attach_to_tangle_res_new();
  TEST_ASSERT_NOT_NULL(attach_res);
  TEST_ASSERT_NOT_NULL(attach_res->trytes);

  TEST_ASSERT_EQUAL_INT16(RC_NULL_PARAM, iota_client_attach_to_tangle(&g_serv, attach_req, attach_res));
  TEST_ASSERT_NOT_NULL(attach_res);
  TEST_ASSERT_NOT_NULL(attach_res->trytes);

  attach_to_tangle_res_free(&attach_res);
  TEST_ASSERT_NULL(attach_res);
  attach_to_tangle_req_free(&attach_req);
  TEST_ASSERT_NULL(attach_req);
}

static void test_attach_to_tangle_empty_trunk_branch(void) {
  flex_trit_t raw_trits[FLEX_TRIT_SIZE_8019];
  attach_to_tangle_req_t *attach_req = attach_to_tangle_req_new();
  TEST_ASSERT_NOT_NULL(attach_req);
  TEST_ASSERT_NOT_NULL(attach_req->trytes);
  TEST_ASSERT_EQUAL_UINT8(ATTACH_TO_TANGLE_MAIN_MWM, attach_req->mwm);
  TEST_ASSERT(flex_trits_are_null(attach_to_tangle_req_trunk(attach_req), NUM_FLEX_TRITS_HASH));
  TEST_ASSERT(flex_trits_are_null(attach_to_tangle_req_branch(attach_req), NUM_FLEX_TRITS_HASH));

  attach_to_tangle_res_t *attach_res = attach_to_tangle_res_new();
  TEST_ASSERT_NOT_NULL(attach_res);
  TEST_ASSERT_NOT_NULL(attach_res->trytes);

  TEST_ASSERT(flex_trits_from_trytes(raw_trits, NUM_TRITS_SERIALIZED_TRANSACTION, TEST_TX_HASH,
                                     NUM_TRYTES_SERIALIZED_TRANSACTION, NUM_TRYTES_SERIALIZED_TRANSACTION) != 0);
  TEST_ASSERT_EQUAL_INT16(RC_OK, attach_to_tangle_req_trytes_add(attach_req, raw_trits));

  TEST_ASSERT_EQUAL_INT16(RC_NULL_PARAM, iota_client_attach_to_tangle(&g_serv, attach_req, attach_res));
  TEST_ASSERT_NOT_NULL(attach_res);
  TEST_ASSERT_NOT_NULL(attach_res->trytes);

  attach_to_tangle_res_free(&attach_res);
  TEST_ASSERT_NULL(attach_res);
  attach_to_tangle_req_free(&attach_req);
  TEST_ASSERT_NULL(attach_req);
}

static void test_attach_to_tangle_empty_branch(void) {
  flex_trit_t raw_trits[FLEX_TRIT_SIZE_8019];
  attach_to_tangle_req_t *attach_req = attach_to_tangle_req_new();
  TEST_ASSERT_NOT_NULL(attach_req);
  TEST_ASSERT_NOT_NULL(attach_req->trytes);
  TEST_ASSERT_EQUAL_UINT8(ATTACH_TO_TANGLE_MAIN_MWM, attach_req->mwm);
  TEST_ASSERT(flex_trits_are_null(attach_to_tangle_req_trunk(attach_req), NUM_FLEX_TRITS_HASH));
  TEST_ASSERT(flex_trits_are_null(attach_to_tangle_req_branch(attach_req), NUM_FLEX_TRITS_HASH));

  attach_to_tangle_res_t *attach_res = attach_to_tangle_res_new();
  TEST_ASSERT_NOT_NULL(attach_res);
  TEST_ASSERT_NOT_NULL(attach_res->trytes);

  TEST_ASSERT(flex_trits_from_trytes(raw_trits, NUM_TRITS_SERIALIZED_TRANSACTION, TEST_TX_HASH,
                                     NUM_TRYTES_SERIALIZED_TRANSACTION, NUM_TRYTES_SERIALIZED_TRANSACTION) != 0);
  TEST_ASSERT_EQUAL_INT16(RC_OK, attach_to_tangle_req_trytes_add(attach_req, raw_trits));

  TEST_ASSERT(flex_trits_from_trytes(attach_req->trunk, NUM_TRITS_HASH, TEST_TRUNK_HASH, NUM_TRYTES_HASH,
                                     NUM_TRYTES_HASH) != 0);

  TEST_ASSERT_EQUAL_INT16(RC_NULL_PARAM, iota_client_attach_to_tangle(&g_serv, attach_req, attach_res));
  TEST_ASSERT_NOT_NULL(attach_res);
  TEST_ASSERT_NOT_NULL(attach_res->trytes);

  attach_to_tangle_res_free(&attach_res);
  TEST_ASSERT_NULL(attach_res);
  attach_to_tangle_req_free(&attach_req);
  TEST_ASSERT_NULL(attach_req);
}

static void test_attach_to_tangle_empty_trunk(void) {
  flex_trit_t raw_trits[FLEX_TRIT_SIZE_8019];
  attach_to_tangle_req_t *attach_req = attach_to_tangle_req_new();
  TEST_ASSERT_NOT_NULL(attach_req);
  TEST_ASSERT_NOT_NULL(attach_req->trytes);
  TEST_ASSERT_EQUAL_UINT8(ATTACH_TO_TANGLE_MAIN_MWM, attach_req->mwm);
  TEST_ASSERT(flex_trits_are_null(attach_to_tangle_req_trunk(attach_req), NUM_FLEX_TRITS_HASH));
  TEST_ASSERT(flex_trits_are_null(attach_to_tangle_req_branch(attach_req), NUM_FLEX_TRITS_HASH));

  attach_to_tangle_res_t *attach_res = attach_to_tangle_res_new();
  TEST_ASSERT_NOT_NULL(attach_res);
  TEST_ASSERT_NOT_NULL(attach_res->trytes);

  TEST_ASSERT(flex_trits_from_trytes(raw_trits, NUM_TRITS_SERIALIZED_TRANSACTION, TEST_TX_HASH,
                                     NUM_TRYTES_SERIALIZED_TRANSACTION, NUM_TRYTES_SERIALIZED_TRANSACTION) != 0);
  TEST_ASSERT_EQUAL_INT16(RC_OK, attach_to_tangle_req_trytes_add(attach_req, raw_trits));

  TEST_ASSERT(flex_trits_from_trytes(attach_req->branch, NUM_TRITS_HASH, TEST_TRUNK_HASH, NUM_TRYTES_HASH,
                                     NUM_TRYTES_HASH) != 0);

  TEST_ASSERT_EQUAL_INT16(RC_NULL_PARAM, iota_client_attach_to_tangle(&g_serv, attach_req, attach_res));
  TEST_ASSERT_NOT_NULL(attach_res);
  TEST_ASSERT_NOT_NULL(attach_res->trytes);

  attach_to_tangle_res_free(&attach_res);
  TEST_ASSERT_NULL(attach_res);
  attach_to_tangle_req_free(&attach_req);
  TEST_ASSERT_NULL(attach_req);
}

int main() {
  UNITY_BEGIN();

  RUN_TEST(service_setup);

  RUN_TEST(test_attach_to_tangle_empty);
  RUN_TEST(test_attach_to_tangle_empty_trunk_branch);
  RUN_TEST(test_attach_to_tangle_empty_branch);
  RUN_TEST(test_attach_to_tangle_empty_trunk);

  RUN_TEST(service_cleanup);

  return UNITY_END();
}
