/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <unity/unity.h>

#include "ciri/api/api.h"
#include "ciri/node/node.h"

static iota_api_t api;
static core_t core;

void test_get_neighbors_empty(void) {
  get_neighbors_res_t *res = get_neighbors_res_new();
  error_res_t *error = NULL;

  TEST_ASSERT(iota_api_get_neighbors(&api, res, &error) == RC_OK);
  TEST_ASSERT(error == NULL);

  TEST_ASSERT_EQUAL_INT(get_neighbors_res_num(res), 0);

  get_neighbors_res_free(res);
  error_res_free(&error);
}

void test_get_neighbors(void) {
  get_neighbors_res_t *res = get_neighbors_res_new();
  error_res_t *error = NULL;
  char address[MAX_HOST_LENGTH + 6];

  TEST_ASSERT_EQUAL_INT(router_neighbors_count(&api.core->node.router), 4);

  TEST_ASSERT(iota_api_get_neighbors(&api, res, &error) == RC_OK);
  TEST_ASSERT(error == NULL);

  TEST_ASSERT_EQUAL_INT(router_neighbors_count(&api.core->node.router), 4);
  TEST_ASSERT_EQUAL_INT(get_neighbors_res_num(res), 4);

  for (size_t i = 0; i < get_neighbors_res_num(res); i++) {
    neighbor_info_t *info = get_neighbors_res_neighbor_at(res, i);
    neighbor_t *neighbor = (neighbor_t *)utarray_eltptr(api.core->node.router.neighbors, i);

    snprintf(address, MAX_HOST_LENGTH + MAX_PORT_LENGTH + 1, "%s:%d", neighbor->endpoint.domain,
             neighbor->endpoint.port);

    TEST_ASSERT_EQUAL_INT(neighbor->nbr_all_txs, info->all_trans_num);
    TEST_ASSERT_EQUAL_INT(neighbor->nbr_new_txs, info->new_trans_num);
    TEST_ASSERT_EQUAL_INT(neighbor->nbr_invalid_txs, info->invalid_trans_num);
    TEST_ASSERT_EQUAL_INT(neighbor->nbr_stale_txs, info->stale_trans_num);
    TEST_ASSERT_EQUAL_INT(neighbor->nbr_sent_txs, info->sent_trans_num);
    TEST_ASSERT_EQUAL_INT(neighbor->nbr_random_tx_reqs, info->random_trans_req_num);
    TEST_ASSERT_EQUAL_STRING(address, info->address->data);
  }

  get_neighbors_res_free(res);
  error_res_free(&error);
}

int main(void) {
  UNITY_BEGIN();

  api.core = &core;
  TEST_ASSERT(router_init(&api.core->node.router, &api.core->node) == RC_OK);

  RUN_TEST(test_get_neighbors_empty);

  neighbor_t neighbor;

  TEST_ASSERT(neighbor_init_with_uri(&neighbor, "tcp://127.0.0.1:15001") == RC_OK);
  neighbor.nbr_all_txs = 1;
  neighbor.nbr_new_txs = 2;
  neighbor.nbr_invalid_txs = 3;
  neighbor.nbr_stale_txs = 4;
  neighbor.nbr_sent_txs = 5;
  neighbor.nbr_random_tx_reqs = 6;
  TEST_ASSERT(router_neighbor_add(&api.core->node.router, &neighbor) == RC_OK);

  TEST_ASSERT(neighbor_init_with_uri(&neighbor, "tcp://127.0.0.1:15002") == RC_OK);
  neighbor.nbr_all_txs = 7;
  neighbor.nbr_new_txs = 8;
  neighbor.nbr_invalid_txs = 9;
  neighbor.nbr_stale_txs = 10;
  neighbor.nbr_sent_txs = 11;
  neighbor.nbr_random_tx_reqs = 12;
  TEST_ASSERT(router_neighbor_add(&api.core->node.router, &neighbor) == RC_OK);

  TEST_ASSERT(neighbor_init_with_uri(&neighbor, "tcp://127.0.0.1:15003") == RC_OK);
  neighbor.nbr_all_txs = 13;
  neighbor.nbr_new_txs = 14;
  neighbor.nbr_invalid_txs = 15;
  neighbor.nbr_stale_txs = 16;
  neighbor.nbr_sent_txs = 17;
  neighbor.nbr_random_tx_reqs = 18;
  TEST_ASSERT(router_neighbor_add(&api.core->node.router, &neighbor) == RC_OK);

  TEST_ASSERT(neighbor_init_with_uri(&neighbor, "tcp://127.0.0.1:15004") == RC_OK);
  neighbor.nbr_all_txs = 19;
  neighbor.nbr_new_txs = 20;
  neighbor.nbr_invalid_txs = 21;
  neighbor.nbr_stale_txs = 22;
  neighbor.nbr_sent_txs = 23;
  neighbor.nbr_random_tx_reqs = 24;
  TEST_ASSERT(router_neighbor_add(&api.core->node.router, &neighbor) == RC_OK);

  RUN_TEST(test_get_neighbors);

  TEST_ASSERT(router_destroy(&api.core->node.router) == RC_OK);

  return UNITY_END();
}
