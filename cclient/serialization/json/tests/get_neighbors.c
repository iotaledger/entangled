/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "shared.h"

void test_serialize_get_neighbors(void) {
  serializer_t serializer;
  const char* json_text = "{\"command\":\"getNeighbors\"}";

  char_buffer_t* serializer_out = char_buffer_new();
  init_json_serializer(&serializer);

  serializer.vtable.get_neighbors_serialize_request(&serializer,
                                                    serializer_out);

  TEST_ASSERT_EQUAL_STRING(json_text, serializer_out->data);

  char_buffer_free(serializer_out);
}

void test_deserialize_get_neighbors(void) {
  serializer_t serializer;
  init_json_serializer(&serializer);
  const char* json_text =
      "{\"duration\":37,\"neighbors\":[{"
      "\"address\":\"" TEST_NEIGHBORS_NEIGHBOR1
      "\",\"numberOfAllTransactions\":" STR(TEST_NEIGHBORS_NUMALLTX1)
      ",\"numberOfInvalidTransactions\":" STR(TEST_NEIGHBORS_NUMINVALIDTX1)
      ",\"numberOfNewTransactions\":" STR(TEST_NEIGHBORS_NUMNEWTX1)
      "},{\"address\":\"" TEST_NEIGHBORS_NEIGHBOR2
      "\",\"numberOfAllTransactions\":" STR(TEST_NEIGHBORS_NUMALLTX2)
      ",\"numberOfInvalidTransactions\":" STR(TEST_NEIGHBORS_NUMINVALIDTX2)
      ",\"numberOfNewTransactions\":" STR(TEST_NEIGHBORS_NUMNEWTX2) "}]}";

  get_neighbors_res_t* nbors = get_neighbors_res_new();

  serializer.vtable.get_neighbors_deserialize_response(&serializer, json_text,
                                                       nbors);

  neighbor_info_t* nb = get_neighbors_res_neighbor_at(nbors, 0);
  TEST_ASSERT_EQUAL_STRING(TEST_NEIGHBORS_NEIGHBOR1, nb->address->data);
  TEST_ASSERT_EQUAL_INT(TEST_NEIGHBORS_NUMALLTX1, nb->all_trans_num);
  TEST_ASSERT_EQUAL_INT(TEST_NEIGHBORS_NUMINVALIDTX1, nb->invalid_trans_num);
  TEST_ASSERT_EQUAL_INT(TEST_NEIGHBORS_NUMNEWTX1, nb->new_trans_num);
  nb = get_neighbors_res_neighbor_at(nbors, 1);
  TEST_ASSERT_EQUAL_STRING(TEST_NEIGHBORS_NEIGHBOR2, nb->address->data);
  TEST_ASSERT_EQUAL_INT(TEST_NEIGHBORS_NUMALLTX2, nb->all_trans_num);
  TEST_ASSERT_EQUAL_INT(TEST_NEIGHBORS_NUMINVALIDTX2, nb->invalid_trans_num);
  TEST_ASSERT_EQUAL_INT(TEST_NEIGHBORS_NUMNEWTX2, nb->new_trans_num);
  nb = get_neighbors_res_neighbor_at(nbors, 2);
  TEST_ASSERT_NULL(nb);
  get_neighbors_res_free(nbors);
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_serialize_get_neighbors);
  RUN_TEST(test_deserialize_get_neighbors);
  return UNITY_END();
}
