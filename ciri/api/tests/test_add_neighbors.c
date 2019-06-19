/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <unity/unity.h>

#include "ciri/api/api.h"
#include "node/node.h"

static iota_api_t api;
static core_t core;

void test_add_neighbors(void) {
  add_neighbors_req_t *req = add_neighbors_req_new();
  add_neighbors_res_t *res = add_neighbors_res_new();
  error_res_t *error = NULL;

  TEST_ASSERT_EQUAL_INT(neighbors_count(api.core->node.neighbors), 0);

  TEST_ASSERT(add_neighbors_req_uris_add(req, "udp://8.8.8.1:15001") == RC_OK);
  TEST_ASSERT(add_neighbors_req_uris_add(req, "udp://8.8.8.2:15002") == RC_OK);
  TEST_ASSERT(add_neighbors_req_uris_add(req, "tcp://8.8.8.3:15003") == RC_OK);
  TEST_ASSERT(add_neighbors_req_uris_add(req, "tcp://8.8.8.4:15004") == RC_OK);

  TEST_ASSERT(iota_api_add_neighbors(&api, req, res, &error) == RC_OK);
  TEST_ASSERT(error == NULL);

  TEST_ASSERT_EQUAL_INT(neighbors_count(api.core->node.neighbors), 4);
  TEST_ASSERT_EQUAL_INT(res->added_neighbors, 4);

  neighbor_t *neighbor = api.core->node.neighbors;
  TEST_ASSERT_EQUAL_STRING(neighbor->endpoint.host, "8.8.8.4");
  TEST_ASSERT_EQUAL_INT(neighbor->endpoint.port, 15004);
  TEST_ASSERT_EQUAL_INT(neighbor->endpoint.protocol, PROTOCOL_TCP);
  neighbor = neighbor->next;

  TEST_ASSERT_EQUAL_STRING(neighbor->endpoint.host, "8.8.8.3");
  TEST_ASSERT_EQUAL_INT(neighbor->endpoint.port, 15003);
  TEST_ASSERT_EQUAL_INT(neighbor->endpoint.protocol, PROTOCOL_TCP);
  neighbor = neighbor->next;

  TEST_ASSERT_EQUAL_STRING(neighbor->endpoint.host, "8.8.8.2");
  TEST_ASSERT_EQUAL_INT(neighbor->endpoint.port, 15002);
  TEST_ASSERT_EQUAL_INT(neighbor->endpoint.protocol, PROTOCOL_UDP);
  neighbor = neighbor->next;

  TEST_ASSERT_EQUAL_STRING(neighbor->endpoint.host, "8.8.8.1");
  TEST_ASSERT_EQUAL_INT(neighbor->endpoint.port, 15001);
  TEST_ASSERT_EQUAL_INT(neighbor->endpoint.protocol, PROTOCOL_UDP);
  neighbor = neighbor->next;

  TEST_ASSERT_NULL(neighbor);

  add_neighbors_req_free(&req);
  add_neighbors_res_free(&res);
  error_res_free(&error);
}

void test_add_neighbors_with_already_paired(void) {
  add_neighbors_req_t *req = add_neighbors_req_new();
  add_neighbors_res_t *res = add_neighbors_res_new();
  error_res_t *error = NULL;

  TEST_ASSERT_EQUAL_INT(neighbors_count(api.core->node.neighbors), 4);

  TEST_ASSERT(add_neighbors_req_uris_add(req, "udp://8.8.8.1:15001") == RC_OK);
  TEST_ASSERT(add_neighbors_req_uris_add(req, "udp://8.8.8.5:15005") == RC_OK);
  TEST_ASSERT(add_neighbors_req_uris_add(req, "tcp://8.8.8.3:15003") == RC_OK);

  TEST_ASSERT(iota_api_add_neighbors(&api, req, res, &error) == RC_OK);
  TEST_ASSERT(error == NULL);

  TEST_ASSERT_EQUAL_INT(neighbors_count(api.core->node.neighbors), 5);
  TEST_ASSERT_EQUAL_INT(res->added_neighbors, 1);

  neighbor_t *neighbor = api.core->node.neighbors;
  TEST_ASSERT_EQUAL_STRING(neighbor->endpoint.host, "8.8.8.5");
  TEST_ASSERT_EQUAL_INT(neighbor->endpoint.port, 15005);
  TEST_ASSERT_EQUAL_INT(neighbor->endpoint.protocol, PROTOCOL_UDP);

  add_neighbors_req_free(&req);
  add_neighbors_res_free(&res);
  error_res_free(&error);
}

void test_add_neighbors_with_invalid(void) {
  add_neighbors_req_t *req = add_neighbors_req_new();
  add_neighbors_res_t *res = add_neighbors_res_new();
  error_res_t *error = NULL;

  TEST_ASSERT_EQUAL_INT(neighbors_count(api.core->node.neighbors), 5);

  TEST_ASSERT(add_neighbors_req_uris_add(req, "udp://8.8.8.6:15006") == RC_OK);
  TEST_ASSERT(add_neighbors_req_uris_add(req, "udp://8.8.8.7@15007") == RC_OK);
  TEST_ASSERT(add_neighbors_req_uris_add(req, "udp://8.8.8.8:15009") == RC_OK);

  TEST_ASSERT(iota_api_add_neighbors(&api, req, res, &error) == RC_NEIGHBOR_FAILED_URI_PARSING);
  TEST_ASSERT(error != NULL);
  TEST_ASSERT_EQUAL_STRING(error_res_get_message(error), API_ERROR_INVALID_URI_SCHEME);

  TEST_ASSERT_EQUAL_INT(neighbors_count(api.core->node.neighbors), 6);
  TEST_ASSERT_EQUAL_INT(res->added_neighbors, 1);

  neighbor_t *neighbor = api.core->node.neighbors;
  TEST_ASSERT_EQUAL_STRING(neighbor->endpoint.host, "8.8.8.6");
  TEST_ASSERT_EQUAL_INT(neighbor->endpoint.port, 15006);
  TEST_ASSERT_EQUAL_INT(neighbor->endpoint.protocol, PROTOCOL_UDP);

  add_neighbors_req_free(&req);
  add_neighbors_res_free(&res);
  error_res_free(&error);

  req = add_neighbors_req_new();
  res = add_neighbors_res_new();
  error = NULL;

  TEST_ASSERT(add_neighbors_req_uris_add(req, "adp://8.8.8.7:15008") == RC_OK);

  TEST_ASSERT(iota_api_add_neighbors(&api, req, res, &error) == RC_NEIGHBOR_INVALID_PROTOCOL);
  TEST_ASSERT(error != NULL);
  TEST_ASSERT_EQUAL_STRING(error_res_get_message(error), API_ERROR_INVALID_URI_SCHEME);

  add_neighbors_req_free(&req);
  add_neighbors_res_free(&res);
  error_res_free(&error);
}

int main(void) {
  UNITY_BEGIN();

  api.core = &core;
  api.core->node.neighbors = NULL;
  rw_lock_handle_init(&api.core->node.neighbors_lock);

  RUN_TEST(test_add_neighbors);
  RUN_TEST(test_add_neighbors_with_already_paired);
  RUN_TEST(test_add_neighbors_with_invalid);

  neighbors_free(&api.core->node.neighbors);
  rw_lock_handle_destroy(&api.core->node.neighbors_lock);

  return UNITY_END();
}
