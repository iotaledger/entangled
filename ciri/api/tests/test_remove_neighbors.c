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

void test_remove_neighbors(void) {
  remove_neighbors_req_t *req = remove_neighbors_req_new();
  remove_neighbors_res_t *res = remove_neighbors_res_new();

  TEST_ASSERT_EQUAL_INT(neighbors_count(node.neighbors), 6);

  remove_neighbors_req_add(req, "udp://8.8.8.2:15002");
  remove_neighbors_req_add(req, "tcp://8.8.8.4:15004");

  TEST_ASSERT(iota_api_remove_neighbors(&api, req, res) == RC_OK);

  TEST_ASSERT_EQUAL_INT(neighbors_count(node.neighbors), 4);
  TEST_ASSERT_EQUAL_INT(res->removed_neighbors, 2);

  neighbor_t *neighbor = node.neighbors;
  TEST_ASSERT_EQUAL_STRING(neighbor->endpoint.host, "8.8.8.6");
  TEST_ASSERT_EQUAL_INT(neighbor->endpoint.port, 15006);
  TEST_ASSERT_EQUAL_INT(neighbor->endpoint.protocol, PROTOCOL_TCP);
  neighbor = neighbor->next;

  TEST_ASSERT_EQUAL_STRING(neighbor->endpoint.host, "8.8.8.5");
  TEST_ASSERT_EQUAL_INT(neighbor->endpoint.port, 15005);
  TEST_ASSERT_EQUAL_INT(neighbor->endpoint.protocol, PROTOCOL_UDP);
  neighbor = neighbor->next;

  TEST_ASSERT_EQUAL_STRING(neighbor->endpoint.host, "8.8.8.3");
  TEST_ASSERT_EQUAL_INT(neighbor->endpoint.port, 15003);
  TEST_ASSERT_EQUAL_INT(neighbor->endpoint.protocol, PROTOCOL_TCP);
  neighbor = neighbor->next;

  TEST_ASSERT_EQUAL_STRING(neighbor->endpoint.host, "8.8.8.1");
  TEST_ASSERT_EQUAL_INT(neighbor->endpoint.port, 15001);
  TEST_ASSERT_EQUAL_INT(neighbor->endpoint.protocol, PROTOCOL_UDP);
  neighbor = neighbor->next;

  TEST_ASSERT_NULL(neighbor);

  remove_neighbors_req_free(&req);
  remove_neighbors_res_free(&res);
}

void test_remove_neighbors_with_not_paired(void) {
  remove_neighbors_req_t *req = remove_neighbors_req_new();
  remove_neighbors_res_t *res = remove_neighbors_res_new();

  TEST_ASSERT_EQUAL_INT(neighbors_count(node.neighbors), 4);

  remove_neighbors_req_add(req, "udp://8.8.8.1:15001");
  remove_neighbors_req_add(req, "udp://8.8.8.7:15007");
  remove_neighbors_req_add(req, "tcp://8.8.8.3:15003");

  TEST_ASSERT(iota_api_remove_neighbors(&api, req, res) == RC_OK);

  TEST_ASSERT_EQUAL_INT(neighbors_count(node.neighbors), 2);
  TEST_ASSERT_EQUAL_INT(res->removed_neighbors, 2);

  neighbor_t *neighbor = node.neighbors;
  TEST_ASSERT_EQUAL_STRING(neighbor->endpoint.host, "8.8.8.6");
  TEST_ASSERT_EQUAL_INT(neighbor->endpoint.port, 15006);
  TEST_ASSERT_EQUAL_INT(neighbor->endpoint.protocol, PROTOCOL_TCP);
  neighbor = neighbor->next;

  TEST_ASSERT_EQUAL_STRING(neighbor->endpoint.host, "8.8.8.5");
  TEST_ASSERT_EQUAL_INT(neighbor->endpoint.port, 15005);
  TEST_ASSERT_EQUAL_INT(neighbor->endpoint.protocol, PROTOCOL_UDP);
  neighbor = neighbor->next;

  TEST_ASSERT_NULL(neighbor);

  remove_neighbors_req_free(&req);
  remove_neighbors_res_free(&res);
}

void test_remove_neighbors_with_invalid(void) {
  remove_neighbors_req_t *req = remove_neighbors_req_new();
  remove_neighbors_res_t *res = remove_neighbors_res_new();

  TEST_ASSERT_EQUAL_INT(neighbors_count(node.neighbors), 4);

  remove_neighbors_req_add(req, "tcp://8.8.8.6:15006");
  remove_neighbors_req_add(req, "udp://8.8.8.7@15007");
  remove_neighbors_req_add(req, "udp://8.8.8.5:15005");

  TEST_ASSERT(iota_api_remove_neighbors(&api, req, res) == RC_OK);

  TEST_ASSERT_EQUAL_INT(neighbors_count(node.neighbors), 1);
  TEST_ASSERT_EQUAL_INT(res->removed_neighbors, 1);

  neighbor_t *neighbor = node.neighbors;
  TEST_ASSERT_EQUAL_STRING(neighbor->endpoint.host, "8.8.8.5");
  TEST_ASSERT_EQUAL_INT(neighbor->endpoint.port, 15005);
  TEST_ASSERT_EQUAL_INT(neighbor->endpoint.protocol, PROTOCOL_UDP);
  neighbor = neighbor->next;

  TEST_ASSERT_NULL(neighbor);

  remove_neighbors_req_free(&req);
  remove_neighbors_res_free(&res);
}

int main(void) {
  UNITY_BEGIN();

  api.node = &node;

  node.neighbors = NULL;
  rw_lock_handle_init(&node.neighbors_lock);

  neighbor_t neighbor;

  neighbor_init_with_uri(&neighbor, "udp://8.8.8.1:15001");
  neighbors_add(&node.neighbors, &neighbor);
  neighbor_init_with_uri(&neighbor, "udp://8.8.8.2:15002");
  neighbors_add(&node.neighbors, &neighbor);
  neighbor_init_with_uri(&neighbor, "tcp://8.8.8.3:15003");
  neighbors_add(&node.neighbors, &neighbor);
  neighbor_init_with_uri(&neighbor, "tcp://8.8.8.4:15004");
  neighbors_add(&node.neighbors, &neighbor);
  neighbor_init_with_uri(&neighbor, "udp://8.8.8.5:15005");
  neighbors_add(&node.neighbors, &neighbor);
  neighbor_init_with_uri(&neighbor, "tcp://8.8.8.6:15006");
  neighbors_add(&node.neighbors, &neighbor);

  RUN_TEST(test_remove_neighbors);
  RUN_TEST(test_remove_neighbors_with_not_paired);

  neighbors_free(&node.neighbors);
  rw_lock_handle_destroy(&node.neighbors_lock);

  return UNITY_END();
}
