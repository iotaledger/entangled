/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <unity/unity.h>

#include "ciri/api/api.h"

static iota_api_t api;

void test_find_transactions_no_input(void) {
  find_transactions_req_t *req = find_transactions_req_new();
  find_transactions_res_t *res = find_transactions_res_new();

  TEST_ASSERT(iota_api_find_transactions(&api, req, res) ==
              RC_API_FIND_TRANSACTIONS_NO_INPUT);

  find_transactions_req_free(&req);
  find_transactions_res_free(&res);
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_find_transactions_no_input);

  return UNITY_END();
}
