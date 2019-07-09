/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <unity/unity.h>

#include "ciri/api/api.h"
#include "ciri/node/node.h"

static iota_api_t api;
static core_t core;

void test_remove_neighbors(void) {
  remove_neighbors_req_t *req = remove_neighbors_req_new();
  remove_neighbors_res_t *res = remove_neighbors_res_new();
  error_res_t *error = NULL;

  TEST_ASSERT_EQUAL_INT(router_neighbors_count(&api.core->node.router), 6);

  TEST_ASSERT(remove_neighbors_req_add(req, "tcp://8.8.8.2:15002") == RC_OK);
  TEST_ASSERT(remove_neighbors_req_add(req, "tcp://8.8.8.4:15004") == RC_OK);

  TEST_ASSERT(iota_api_remove_neighbors(&api, req, res, &error) == RC_OK);
  TEST_ASSERT(error == NULL);

  TEST_ASSERT_EQUAL_INT(router_neighbors_count(&api.core->node.router), 4);
  TEST_ASSERT_EQUAL_INT(res->removed_neighbors, 2);

  neighbor_t *neighbor = (neighbor_t *)utarray_eltptr(api.core->node.router.neighbors, 0);
  TEST_ASSERT_EQUAL_STRING(neighbor->endpoint.domain, "8.8.8.1");
  TEST_ASSERT_EQUAL_INT(neighbor->endpoint.port, 15001);

  neighbor = (neighbor_t *)utarray_eltptr(api.core->node.router.neighbors, 1);
  TEST_ASSERT_EQUAL_STRING(neighbor->endpoint.domain, "8.8.8.3");
  TEST_ASSERT_EQUAL_INT(neighbor->endpoint.port, 15003);

  neighbor = (neighbor_t *)utarray_eltptr(api.core->node.router.neighbors, 2);
  TEST_ASSERT_EQUAL_STRING(neighbor->endpoint.domain, "8.8.8.5");
  TEST_ASSERT_EQUAL_INT(neighbor->endpoint.port, 15005);

  neighbor = (neighbor_t *)utarray_eltptr(api.core->node.router.neighbors, 3);
  TEST_ASSERT_EQUAL_STRING(neighbor->endpoint.domain, "8.8.8.6");
  TEST_ASSERT_EQUAL_INT(neighbor->endpoint.port, 15006);

  remove_neighbors_req_free(&req);
  remove_neighbors_res_free(&res);
  error_res_free(&error);
}

void test_remove_neighbors_with_not_paired(void) {
  remove_neighbors_req_t *req = remove_neighbors_req_new();
  remove_neighbors_res_t *res = remove_neighbors_res_new();
  error_res_t *error = NULL;

  TEST_ASSERT_EQUAL_INT(router_neighbors_count(&api.core->node.router), 4);

  TEST_ASSERT(remove_neighbors_req_add(req, "tcp://8.8.8.1:15001") == RC_OK);
  TEST_ASSERT(remove_neighbors_req_add(req, "tcp://8.8.8.7:15007") == RC_OK);
  TEST_ASSERT(remove_neighbors_req_add(req, "tcp://8.8.8.3:15003") == RC_OK);

  TEST_ASSERT(iota_api_remove_neighbors(&api, req, res, &error) == RC_OK);
  TEST_ASSERT(error == NULL);

  TEST_ASSERT_EQUAL_INT(router_neighbors_count(&api.core->node.router), 2);
  TEST_ASSERT_EQUAL_INT(res->removed_neighbors, 2);

  neighbor_t *neighbor = (neighbor_t *)utarray_eltptr(api.core->node.router.neighbors, 0);
  TEST_ASSERT_EQUAL_STRING(neighbor->endpoint.domain, "8.8.8.5");
  TEST_ASSERT_EQUAL_INT(neighbor->endpoint.port, 15005);

  neighbor = (neighbor_t *)utarray_eltptr(api.core->node.router.neighbors, 1);
  TEST_ASSERT_EQUAL_STRING(neighbor->endpoint.domain, "8.8.8.6");
  TEST_ASSERT_EQUAL_INT(neighbor->endpoint.port, 15006);

  remove_neighbors_req_free(&req);
  remove_neighbors_res_free(&res);
  error_res_free(&error);
}

void test_remove_neighbors_with_invalid(void) {
  remove_neighbors_req_t *req = remove_neighbors_req_new();
  remove_neighbors_res_t *res = remove_neighbors_res_new();
  error_res_t *error = NULL;

  TEST_ASSERT_EQUAL_INT(router_neighbors_count(&api.core->node.router), 2);

  TEST_ASSERT(remove_neighbors_req_add(req, "tcp://8.8.8.6:15006") == RC_OK);
  TEST_ASSERT(remove_neighbors_req_add(req, "tcp://8.8.8.7@15007") == RC_OK);
  TEST_ASSERT(remove_neighbors_req_add(req, "tcp://8.8.8.5:15005") == RC_OK);

  TEST_ASSERT(iota_api_remove_neighbors(&api, req, res, &error) == RC_NEIGHBOR_FAILED_URI_PARSING);
  TEST_ASSERT(error != NULL);
  TEST_ASSERT_EQUAL_STRING(error_res_get_message(error), API_ERROR_INVALID_URI_SCHEME);

  TEST_ASSERT_EQUAL_INT(router_neighbors_count(&api.core->node.router), 1);
  TEST_ASSERT_EQUAL_INT(res->removed_neighbors, 1);

  neighbor_t *neighbor = (neighbor_t *)utarray_eltptr(api.core->node.router.neighbors, 0);
  TEST_ASSERT_EQUAL_STRING(neighbor->endpoint.domain, "8.8.8.5");
  TEST_ASSERT_EQUAL_INT(neighbor->endpoint.port, 15005);

  remove_neighbors_req_free(&req);
  remove_neighbors_res_free(&res);
  error_res_free(&error);

  req = remove_neighbors_req_new();
  res = remove_neighbors_res_new();
  error = NULL;

  TEST_ASSERT(remove_neighbors_req_add(req, "adp://8.8.8.7:15008") == RC_OK);

  TEST_ASSERT(iota_api_remove_neighbors(&api, req, res, &error) == RC_NEIGHBOR_INVALID_PROTOCOL);
  TEST_ASSERT(error != NULL);
  TEST_ASSERT_EQUAL_STRING(error_res_get_message(error), API_ERROR_INVALID_URI_SCHEME);

  remove_neighbors_req_free(&req);
  remove_neighbors_res_free(&res);
  error_res_free(&error);
}

int main(void) {
  UNITY_BEGIN();

  api.core = &core;
  TEST_ASSERT(tcp_server_init(&api.core->node.tcp_server, &api.core->node) == RC_OK);
  TEST_ASSERT(router_init(&api.core->node.router, &api.core->node.conf) == RC_OK);

  neighbor_t neighbor;

  TEST_ASSERT(neighbor_init_with_uri(&neighbor, "tcp://8.8.8.1:15001") == RC_OK);
  TEST_ASSERT(router_neighbor_add(&api.core->node.router, &neighbor) == RC_OK);
  TEST_ASSERT(neighbor_init_with_uri(&neighbor, "tcp://8.8.8.2:15002") == RC_OK);
  TEST_ASSERT(router_neighbor_add(&api.core->node.router, &neighbor) == RC_OK);
  TEST_ASSERT(neighbor_init_with_uri(&neighbor, "tcp://8.8.8.3:15003") == RC_OK);
  TEST_ASSERT(router_neighbor_add(&api.core->node.router, &neighbor) == RC_OK);
  TEST_ASSERT(neighbor_init_with_uri(&neighbor, "tcp://8.8.8.4:15004") == RC_OK);
  TEST_ASSERT(router_neighbor_add(&api.core->node.router, &neighbor) == RC_OK);
  TEST_ASSERT(neighbor_init_with_uri(&neighbor, "tcp://8.8.8.5:15005") == RC_OK);
  TEST_ASSERT(router_neighbor_add(&api.core->node.router, &neighbor) == RC_OK);
  TEST_ASSERT(neighbor_init_with_uri(&neighbor, "tcp://8.8.8.6:15006") == RC_OK);
  TEST_ASSERT(router_neighbor_add(&api.core->node.router, &neighbor) == RC_OK);

  RUN_TEST(test_remove_neighbors);
  RUN_TEST(test_remove_neighbors_with_not_paired);
  RUN_TEST(test_remove_neighbors_with_invalid);

  TEST_ASSERT(router_destroy(&api.core->node.router) == RC_OK);
  TEST_ASSERT(tcp_server_destroy(&api.core->node.tcp_server) == RC_OK);

  return UNITY_END();
}
