/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "cclient/serialization/json/tests/shared.h"

static void test_request(void) {
  serializer_t serializer;
  const char* json_text = "{\"command\":\"addNeighbors\",\"uris\":[\"" TEST_NEIGHBOR1 "\",\"" TEST_NEIGHBOR2 "\"]}";

  char_buffer_t* serializer_out = char_buffer_new();
  init_json_serializer(&serializer);
  add_neighbors_req_t* req = add_neighbors_req_new();
  TEST_ASSERT(add_neighbors_req_uris_add(req, TEST_NEIGHBOR1) == RC_OK);
  TEST_ASSERT(add_neighbors_req_uris_add(req, TEST_NEIGHBOR2) == RC_OK);

  serializer.vtable.add_neighbors_serialize_request(req, serializer_out);
  TEST_ASSERT_EQUAL_STRING(json_text, serializer_out->data);
  add_neighbors_req_free(&req);

  req = add_neighbors_req_new();
  serializer.vtable.add_neighbors_deserialize_request(serializer_out->data, req);

  TEST_ASSERT_EQUAL_INT(2, add_neighbors_req_uris_len(req));
  TEST_ASSERT_EQUAL_STRING(TEST_NEIGHBOR1, add_neighbors_req_uris_at(req, 0));
  TEST_ASSERT_EQUAL_STRING(TEST_NEIGHBOR2, add_neighbors_req_uris_at(req, 1));

  char_buffer_free(serializer_out);
  add_neighbors_req_free(&req);
}

static void test_response(void) {
  serializer_t serializer;
  init_json_serializer(&serializer);
  const char* json_text = "{\"addedNeighbors\":" STR(ADD_NEIGHBORS_RES) "}";

  add_neighbors_res_t* res = add_neighbors_res_new();
  char_buffer_t* serializer_out = char_buffer_new();

  serializer.vtable.add_neighbors_deserialize_response(json_text, res);
  TEST_ASSERT_EQUAL_INT(ADD_NEIGHBORS_RES, res->added_neighbors);

  serializer.vtable.add_neighbors_serialize_response(res, serializer_out);

  TEST_ASSERT_EQUAL_STRING(json_text, serializer_out->data);

  char_buffer_free(serializer_out);
  add_neighbors_res_free(&res);
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_request);
  RUN_TEST(test_response);

  return UNITY_END();
}
