/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "cclient/serialization/json/tests/shared.h"

void test_get_neighbors_serialize_request(void) {
  serializer_t serializer;
  char const* json_text = "{\"command\":\"getNeighbors\"}";

  char_buffer_t* serializer_out = char_buffer_new();
  init_json_serializer(&serializer);

  serializer.vtable.get_neighbors_serialize_request(serializer_out);

  TEST_ASSERT_EQUAL_STRING(json_text, serializer_out->data);

  char_buffer_free(serializer_out);
}

void test_get_neighbors_serialize_response(void) {
  retcode_t ret = RC_OK;
  serializer_t serializer;
  init_json_serializer(&serializer);
  char_buffer_t* out = char_buffer_new();

  char const * TEST_JSON_TEXT =
      "{\"neighbors\":[{"
      "\"address\":\"" TEST_NEIGHBORS_NEIGHBOR1 "\""
      ",\"numberOfAllTransactions\":" STR(TEST_NEIGHBORS_NUMALLTX1)
      ",\"numberOfRandomTransactionRequests\":" STR(TEST_NEIGHBORS_NUMRANDTX1)
    ",\"numberOfNewTransactions\":" STR(TEST_NEIGHBORS_NUMNEWTX1)
    ",\"numberOfInvalidTransactions\":" STR(TEST_NEIGHBORS_NUMINVALIDTX1)
    ",\"numberOfStaleTransactions\":" STR(TEST_NEIGHBORS_NUMSTALETX1)
    ",\"numberOfSentTransactions\":" STR(TEST_NEIGHBORS_NUMSENTTX1)
    ",\"connectiontype\":\"" TEST_NEIGHBORS_CONNNECTION1 "\"},{"
    "\"address\":\"" TEST_NEIGHBORS_NEIGHBOR2 "\""
      ",\"numberOfAllTransactions\":" STR(TEST_NEIGHBORS_NUMALLTX2)
      ",\"numberOfRandomTransactionRequests\":" STR(TEST_NEIGHBORS_NUMRANDTX2)
    ",\"numberOfNewTransactions\":" STR(TEST_NEIGHBORS_NUMNEWTX2)
    ",\"numberOfInvalidTransactions\":" STR(TEST_NEIGHBORS_NUMINVALIDTX2)
    ",\"numberOfStaleTransactions\":" STR(TEST_NEIGHBORS_NUMSTALETX2)
    ",\"numberOfSentTransactions\":" STR(TEST_NEIGHBORS_NUMSENTTX2)
    ",\"connectiontype\":\"" TEST_NEIGHBORS_CONNNECTION2"\"}]}";

  get_neighbors_res_t* res = get_neighbors_res_new();

  ret = get_neighbors_res_add_neighbor(
      res, TEST_NEIGHBORS_NEIGHBOR1, TEST_NEIGHBORS_NUMALLTX1, TEST_NEIGHBORS_NUMRANDTX1, TEST_NEIGHBORS_NUMNEWTX1,
      TEST_NEIGHBORS_NUMINVALIDTX1, TEST_NEIGHBORS_NUMSTALETX1, TEST_NEIGHBORS_NUMSENTTX1, TEST_NEIGHBORS_CONNNECTION1);
  TEST_ASSERT_EQUAL_INT8(RC_OK, ret);
  ret = get_neighbors_res_add_neighbor(
      res, TEST_NEIGHBORS_NEIGHBOR2, TEST_NEIGHBORS_NUMALLTX2, TEST_NEIGHBORS_NUMRANDTX2, TEST_NEIGHBORS_NUMNEWTX2,
      TEST_NEIGHBORS_NUMINVALIDTX2, TEST_NEIGHBORS_NUMSTALETX2, TEST_NEIGHBORS_NUMSENTTX2, TEST_NEIGHBORS_CONNNECTION2);
  TEST_ASSERT_EQUAL_INT8(RC_OK, ret);

  serializer.vtable.get_neighbors_serialize_response(res, out);

  TEST_ASSERT_EQUAL_STRING(TEST_JSON_TEXT, out->data);

  char_buffer_free(out);
  get_neighbors_res_free(&res);
  TEST_ASSERT_NULL(res);
}

void test_get_neighbors_deserialize_response(void) {
  serializer_t serializer;
  init_json_serializer(&serializer);
  char const * json_text =
      "{\"neighbors\":[{"
      "\"address\":\"" TEST_NEIGHBORS_NEIGHBOR1 "\""
      ",\"numberOfAllTransactions\":" STR(TEST_NEIGHBORS_NUMALLTX1)
      ",\"numberOfRandomTransactionRequests\":" STR(TEST_NEIGHBORS_NUMRANDTX1)
    ",\"numberOfNewTransactions\":" STR(TEST_NEIGHBORS_NUMNEWTX1)
    ",\"numberOfInvalidTransactions\":" STR(TEST_NEIGHBORS_NUMINVALIDTX1)
    ",\"numberOfStaleTransactions\":" STR(TEST_NEIGHBORS_NUMSTALETX1)
    ",\"numberOfSentTransactions\":" STR(TEST_NEIGHBORS_NUMSENTTX1)
    ",\"connectiontype\":\"" TEST_NEIGHBORS_CONNNECTION1 "\"},{"
    "\"address\":\"" TEST_NEIGHBORS_NEIGHBOR2 "\""
      ",\"numberOfAllTransactions\":" STR(TEST_NEIGHBORS_NUMALLTX2)
      ",\"numberOfRandomTransactionRequests\":" STR(TEST_NEIGHBORS_NUMRANDTX2)
    ",\"numberOfNewTransactions\":" STR(TEST_NEIGHBORS_NUMNEWTX2)
    ",\"numberOfInvalidTransactions\":" STR(TEST_NEIGHBORS_NUMINVALIDTX2)
    ",\"numberOfStaleTransactions\":" STR(TEST_NEIGHBORS_NUMSTALETX2)
    ",\"numberOfSentTransactions\":" STR(TEST_NEIGHBORS_NUMSENTTX2)
    ",\"connectiontype\":\"" TEST_NEIGHBORS_CONNNECTION2"\"}]}";

  get_neighbors_res_t* nbors = get_neighbors_res_new();

  serializer.vtable.get_neighbors_deserialize_response(json_text, nbors);

  neighbor_info_t* nb = get_neighbors_res_neighbor_at(nbors, 0);
  TEST_ASSERT_EQUAL_STRING(TEST_NEIGHBORS_NEIGHBOR1, nb->address->data);
  TEST_ASSERT_EQUAL_INT(TEST_NEIGHBORS_NUMALLTX1, nb->all_trans_num);
  TEST_ASSERT_EQUAL_INT(TEST_NEIGHBORS_NUMRANDTX1, nb->random_trans_req_num);
  TEST_ASSERT_EQUAL_INT(TEST_NEIGHBORS_NUMNEWTX1, nb->new_trans_num);
  TEST_ASSERT_EQUAL_INT(TEST_NEIGHBORS_NUMINVALIDTX1, nb->invalid_trans_num);
  TEST_ASSERT_EQUAL_INT(TEST_NEIGHBORS_NUMSTALETX1, nb->stale_trans_num);
  TEST_ASSERT_EQUAL_INT(TEST_NEIGHBORS_NUMSENTTX1, nb->sent_trans_num);
  TEST_ASSERT_EQUAL_STRING(TEST_NEIGHBORS_CONNNECTION1, nb->connection_type->data);

  nb = get_neighbors_res_neighbor_at(nbors, 1);
  TEST_ASSERT_EQUAL_STRING(TEST_NEIGHBORS_NEIGHBOR2, nb->address->data);
  TEST_ASSERT_EQUAL_INT(TEST_NEIGHBORS_NUMALLTX2, nb->all_trans_num);
  TEST_ASSERT_EQUAL_INT(TEST_NEIGHBORS_NUMRANDTX2, nb->random_trans_req_num);
  TEST_ASSERT_EQUAL_INT(TEST_NEIGHBORS_NUMNEWTX2, nb->new_trans_num);
  TEST_ASSERT_EQUAL_INT(TEST_NEIGHBORS_NUMINVALIDTX2, nb->invalid_trans_num);
  TEST_ASSERT_EQUAL_INT(TEST_NEIGHBORS_NUMSTALETX2, nb->stale_trans_num);
  TEST_ASSERT_EQUAL_INT(TEST_NEIGHBORS_NUMSENTTX2, nb->sent_trans_num);
  TEST_ASSERT_EQUAL_STRING(TEST_NEIGHBORS_CONNNECTION2, nb->connection_type->data);

  nb = get_neighbors_res_neighbor_at(nbors, 2);
  TEST_ASSERT_NULL(nb);
  get_neighbors_res_free(&nbors);
  TEST_ASSERT_NULL(nbors);
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_get_neighbors_serialize_request);
  RUN_TEST(test_get_neighbors_serialize_response);
  RUN_TEST(test_get_neighbors_deserialize_response);
  return UNITY_END();
}
