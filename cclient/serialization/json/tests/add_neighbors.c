/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "shared.h"

void test_serialize_add_neighbors(void) {
  serializer_t serializer;
  const char* json_text =
      "{\"command\":\"addNeighbors\",\"uris\":[\"" TEST_NEIGHBOR1
      "\",\"" TEST_NEIGHBOR2 "\"]}";

  char_buffer_t* serializer_out = char_buffer_new();
  init_json_serializer(&serializer);
  add_neighbors_req_t* req = add_neighbors_req_new();
  add_neighbors_req_add(req, TEST_NEIGHBOR1);
  add_neighbors_req_add(req, TEST_NEIGHBOR2);

  serializer.vtable.add_neighbors_serialize_request(&serializer, req,
                                                    serializer_out);

  TEST_ASSERT_EQUAL_STRING(json_text, serializer_out->data);

  char_buffer_free(serializer_out);
  add_neighbors_req_free(&req);
}

void test_deserialize_add_neighbors(void) {
  serializer_t serializer;
  init_json_serializer(&serializer);
  const char* json_text =
      "{\"addedNeighbors\":" STR(ADD_NEIGHBORS_RES) ",\"duration\":2}";

  add_neighbors_res_t* res = add_neighbors_res_new();

  serializer.vtable.add_neighbors_deserialize_response(&serializer, json_text,
                                                       res);

  TEST_ASSERT_EQUAL_INT(ADD_NEIGHBORS_RES, res->added_neighbors);

  add_neighbors_res_free(&res);
}

int main(void) {
  UNITY_BEGIN();

  // add_neighbors
  RUN_TEST(test_serialize_add_neighbors);
  RUN_TEST(test_deserialize_add_neighbors);

  return UNITY_END();
}
