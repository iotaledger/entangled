/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "cclient/api/tests/cclient_test_defs.h"

static iota_client_service_t g_serv;

static void test_get_tips(void) {
  get_tips_res_t *tips_res = get_tips_res_new();
  TEST_ASSERT_NOT_NULL(tips_res);
  TEST_ASSERT_NULL(tips_res->hashes);

  TEST_ASSERT_EQUAL_INT16(RC_OK, iota_client_get_tips(&g_serv, tips_res));

  TEST_ASSERT_NOT_NULL(tips_res->hashes);

  get_tips_res_free(&tips_res);
  TEST_ASSERT_NULL(tips_res);
}

int main() {
  UNITY_BEGIN();

  cclient_service_setup(&g_serv);

  RUN_TEST(test_get_tips);

  cclient_service_cleanup(&g_serv);

  return UNITY_END();
}
