/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "cclient/api/tests/cclient_test_defs.h"

static iota_client_service_t g_serv;

static void test_get_neighbors(void) {
  retcode_t ret = RC_ERROR;
  get_neighbors_res_t *nb_res = get_neighbors_res_new();
  TEST_ASSERT_NOT_NULL(nb_res);
  TEST_ASSERT_EQUAL_UINT32(0, get_neighbors_res_num(nb_res));

  ret = iota_client_get_neighbors(&g_serv, nb_res);
  if (ret == RC_OK) {
    TEST_ASSERT(get_neighbors_res_num(nb_res) > 0);

  } else if (ret == RC_CCLIENT_RES_ERROR) {
    // getNeighbors is not available on this node
    TEST_ASSERT_EQUAL_UINT32(0, get_neighbors_res_num(nb_res));

  } else {
    // unknow
    TEST_ASSERT(1);
  }

  get_neighbors_res_free(&nb_res);
  TEST_ASSERT_NULL(nb_res);
}

int main() {
  UNITY_BEGIN();

  cclient_service_setup(&g_serv);

  RUN_TEST(test_get_neighbors);

  cclient_service_cleanup(&g_serv);

  return UNITY_END();
}
