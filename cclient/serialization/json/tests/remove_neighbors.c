/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "cclient/serialization/json/tests/shared.h"

void test_remove_neighbors_serialize_request(void) {
  serializer_t serializer;
  char const* json_text = "{\"command\":\"removeNeighbors\",\"uris\":[\"" TEST_NEIGHBOR1 "\",\"" TEST_NEIGHBOR2 "\"]}";

  char_buffer_t* serializer_out = char_buffer_new();
  init_json_serializer(&serializer);
  remove_neighbors_req_t* req = remove_neighbors_req_new();
  remove_neighbors_req_add(req, TEST_NEIGHBOR1);
  remove_neighbors_req_add(req, TEST_NEIGHBOR2);

  serializer.vtable.remove_neighbors_serialize_request(req, serializer_out);

  TEST_ASSERT_EQUAL_STRING(json_text, serializer_out->data);

  char_buffer_free(serializer_out);
  remove_neighbors_req_free(&req);
}

void test_remove_neighbors_deserialize_request(void) {
  serializer_t serializer;
  init_json_serializer(&serializer);
  char const* json_text = "{\"command\":\"removeNeighbors\",\"uris\":[\"" TEST_NEIGHBOR1 "\",\"" TEST_NEIGHBOR2 "\"]}";

  remove_neighbors_req_t* req = remove_neighbors_req_new();
  remove_neighbors_req_add(req, TEST_NEIGHBOR1);
  remove_neighbors_req_add(req, TEST_NEIGHBOR2);
  serializer.vtable.remove_neighbors_deserialize_request(json_text, req);

  TEST_ASSERT_EQUAL_STRING(TEST_NEIGHBOR1, remove_neighbors_req_uris_at(req, 0));
  TEST_ASSERT_EQUAL_STRING(TEST_NEIGHBOR2, remove_neighbors_req_uris_at(req, 1));

  remove_neighbors_req_free(&req);
}

void test_remove_neighbors_serialize_response(void) {
  serializer_t serializer;
  char const* json_text = "{\"removedNeighbors\":" STR(REMOVE_NEIGHBORS_RES) "}";

  char_buffer_t* serializer_out = char_buffer_new();
  init_json_serializer(&serializer);
  remove_neighbors_res_t* res = remove_neighbors_res_new();

  res->removed_neighbors = REMOVE_NEIGHBORS_RES;

  serializer.vtable.remove_neighbors_serialize_response(res, serializer_out);

  TEST_ASSERT_EQUAL_STRING(json_text, serializer_out->data);

  char_buffer_free(serializer_out);
  remove_neighbors_res_free(&res);
}

void test_remove_neighbors_deserialize_response(void) {
  serializer_t serializer;
  init_json_serializer(&serializer);
  char const* json_text = "{\"removedNeighbors\":" STR(REMOVE_NEIGHBORS_RES) ",\"duration\":2}";

  remove_neighbors_res_t* res = remove_neighbors_res_new();

  serializer.vtable.remove_neighbors_deserialize_response(json_text, res);

  TEST_ASSERT_EQUAL_INT(REMOVE_NEIGHBORS_RES, res->removed_neighbors);

  remove_neighbors_res_free(&res);
}
int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_remove_neighbors_serialize_request);
  RUN_TEST(test_remove_neighbors_deserialize_request);
  RUN_TEST(test_remove_neighbors_serialize_response);
  RUN_TEST(test_remove_neighbors_deserialize_response);

  return UNITY_END();
}
