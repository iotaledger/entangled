/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <unity/unity.h>

#include "ciri/api/api.h"

static iota_api_t api;

void test_check_consistency(void) {
  check_consistency_req_t *req = check_consistency_req_new();
  check_consistency_res_t *res = check_consistency_res_new();

  TEST_ASSERT(iota_api_check_consistency(&api, req, res) == RC_OK);

  check_consistency_req_free(&req);
  check_consistency_res_free(res);
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_check_consistency);

  return UNITY_END();
}
