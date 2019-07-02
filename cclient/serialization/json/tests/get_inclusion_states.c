/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "cclient/serialization/json/tests/shared.h"

void test_get_inclusion_states_serialize_request(void) {
  serializer_t serializer;
  init_json_serializer(&serializer);
  char const* json_text =
      "{\"command\":\"getInclusionStates\",\"transactions\":["
      "\"" TEST_81_TRYTES_1 "\"],\"tips\":[\"" TEST_81_TRYTES_2 "\"]}";
  char const* json_text_no_tips =
      "{\"command\":\"getInclusionStates\",\"transactions\":["
      "\"" TEST_81_TRYTES_1 "\"]}";

  get_inclusion_states_req_t* get_is = get_inclusion_states_req_new();
  char_buffer_t* serializer_out = char_buffer_new();
  flex_trit_t trits_243[FLEX_TRIT_SIZE_243];

  TEST_ASSERT(flex_trits_from_trytes(trits_243, NUM_TRITS_HASH, (tryte_t const*)TEST_81_TRYTES_1, NUM_TRYTES_HASH,
                                     NUM_TRYTES_HASH));
  TEST_ASSERT(hash243_queue_push(&get_is->transactions, trits_243) == RC_OK);

  serializer.vtable.get_inclusion_states_serialize_request(get_is, serializer_out);
  // test request with no tips
  TEST_ASSERT_EQUAL_STRING(json_text_no_tips, serializer_out->data);

  TEST_ASSERT(flex_trits_from_trytes(trits_243, NUM_TRITS_HASH, (tryte_t const*)TEST_81_TRYTES_2, NUM_TRYTES_HASH,
                                     NUM_TRYTES_HASH));
  TEST_ASSERT(hash243_queue_push(&get_is->tips, trits_243) == RC_OK);

  serializer.vtable.get_inclusion_states_serialize_request(get_is, serializer_out);

  TEST_ASSERT_EQUAL_STRING(json_text, serializer_out->data);

  char_buffer_free(serializer_out);
  get_inclusion_states_req_free(&get_is);
}

void test_get_inclusion_states_deserialize_request(void) {
  serializer_t serializer;
  init_json_serializer(&serializer);
  char const* json_text = "{\"transactions\": [\"" TEST_81_TRYTES_1
                          "\"],"
                          "\"tips\": [\"" TEST_81_TRYTES_2 "\"]}";
  char const* json_text_no_tips = "{\"transactions\": [\"" TEST_81_TRYTES_1 "\"]}";
  flex_trit_t hash[FLEX_TRIT_SIZE_243];
  flex_trit_t* req_transaction = NULL;
  get_inclusion_states_req_t* deserialize_get_is = get_inclusion_states_req_new();

  // test request with no tips
  serializer.vtable.get_inclusion_states_deserialize_request(json_text_no_tips, deserialize_get_is);
  req_transaction = hash243_queue_at(deserialize_get_is->transactions, 0);
  flex_trits_from_trytes(hash, NUM_TRITS_HASH, (tryte_t*)TEST_81_TRYTES_1, NUM_TRYTES_HASH, NUM_TRYTES_HASH);
  TEST_ASSERT_EQUAL_MEMORY(hash, req_transaction, FLEX_TRIT_SIZE_243);
  TEST_ASSERT(hash243_queue_count(deserialize_get_is->tips) == 0);

  // test request with tips
  serializer.vtable.get_inclusion_states_deserialize_request(json_text, deserialize_get_is);
  req_transaction = hash243_queue_at(deserialize_get_is->transactions, 0);
  flex_trits_from_trytes(hash, NUM_TRITS_HASH, (tryte_t*)TEST_81_TRYTES_1, NUM_TRYTES_HASH, NUM_TRYTES_HASH);
  TEST_ASSERT_EQUAL_MEMORY(hash, req_transaction, FLEX_TRIT_SIZE_243);

  req_transaction = hash243_queue_at(deserialize_get_is->tips, 0);
  flex_trits_from_trytes(hash, NUM_TRITS_HASH, (tryte_t*)TEST_81_TRYTES_2, NUM_TRYTES_HASH, NUM_TRYTES_HASH);
  TEST_ASSERT_EQUAL_MEMORY(hash, req_transaction, FLEX_TRIT_SIZE_243);
  get_inclusion_states_req_free(&deserialize_get_is);
}

void test_get_inclusion_states_serialize_response(void) {
  serializer_t serializer;
  init_json_serializer(&serializer);
  char const* json_text = "{\"states\":[true,false]}";

  get_inclusion_states_res_t* get_is = get_inclusion_states_res_new();
  char_buffer_t* serializer_out = char_buffer_new();

  TEST_ASSERT(get_inclusion_states_res_states_add(get_is, true) == RC_OK);
  TEST_ASSERT(get_inclusion_states_res_states_add(get_is, false) == RC_OK);
  serializer.vtable.get_inclusion_states_serialize_response(get_is, serializer_out);

  TEST_ASSERT_EQUAL_STRING(json_text, serializer_out->data);

  char_buffer_free(serializer_out);
  get_inclusion_states_res_free(&get_is);
}

void test_get_inclusion_states_deserialize_response(void) {
  serializer_t serializer;
  init_json_serializer(&serializer);
  char const* json_text = "{\"states\": [true,false]}";
  get_inclusion_states_res_t* deserialize_get_is = get_inclusion_states_res_new();

  serializer.vtable.get_inclusion_states_deserialize_response(json_text, deserialize_get_is);
  TEST_ASSERT_TRUE(get_inclusion_states_res_states_at(deserialize_get_is, 0) == true);
  TEST_ASSERT_TRUE(get_inclusion_states_res_states_at(deserialize_get_is, 1) == false);
  TEST_ASSERT_TRUE(get_inclusion_states_res_states_at(deserialize_get_is, 2) == false);
  TEST_ASSERT_FALSE(get_inclusion_states_res_states_at(deserialize_get_is, 2) == true);
  get_inclusion_states_res_free(&deserialize_get_is);
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_get_inclusion_states_serialize_request);
  RUN_TEST(test_get_inclusion_states_deserialize_request);
  RUN_TEST(test_get_inclusion_states_serialize_response);
  RUN_TEST(test_get_inclusion_states_deserialize_response);

  return UNITY_END();
}
