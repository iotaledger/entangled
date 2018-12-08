/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <unity/unity.h>

#include "ciri/api/api.h"
#include "gossip/node.h"

static iota_api_t api;
static node_t node;

void test_add_neighbors(void) {
  add_neighbors_req_t *req = add_neighbors_req_new();
  add_neighbors_res_t *res = add_neighbors_res_new();

  TEST_ASSERT_EQUAL_INT(neighbors_count(node.neighbors), 0);

  add_neighbors_req_add(req, "udp://8.8.8.1:15001");

  TEST_ASSERT(iota_api_add_neighbors(&api, req, res) == RC_OK);

  TEST_ASSERT_EQUAL_INT(neighbors_count(node.neighbors), 0);
  TEST_ASSERT_EQUAL_INT(res->added_neighbors, 0);

  add_neighbors_req_free(req);
  add_neighbors_res_free(&res);
}

int main(void) {
  UNITY_BEGIN();

  api.node = &node;

  node.neighbors = NULL;
  rw_lock_handle_init(&node.neighbors_lock);

  RUN_TEST(test_add_neighbors);

  neighbors_free(&node.neighbors);
  rw_lock_handle_destroy(&node.neighbors_lock);

  return UNITY_END();
}
