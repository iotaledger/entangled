/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "cclient/serialization/json/tests/shared.h"

void test_serialize_get_inclusion_states(void) {
  serializer_t serializer;
  init_json_serializer(&serializer);
  const char* json_text =
      "{\"command\":\"getInclusionStates\",\"transactions\":["
      "\"" TEST_81_TRYTES_1 "\"],\"tips\":[\"" TEST_81_TRYTES_2 "\"]}";

  get_inclusion_states_req_t* get_is = get_inclusion_states_req_new();
  char_buffer_t* serializer_out = char_buffer_new();
  flex_trit_t trits_243[FLEX_TRIT_SIZE_243];

  TEST_ASSERT(flex_trits_from_trytes(trits_243, NUM_TRITS_HASH, (const tryte_t*)TEST_81_TRYTES_1, NUM_TRYTES_HASH,
                                     NUM_TRYTES_HASH));
  TEST_ASSERT(hash243_queue_push(&get_is->hashes, trits_243) == RC_OK);

  TEST_ASSERT(flex_trits_from_trytes(trits_243, NUM_TRITS_HASH, (const tryte_t*)TEST_81_TRYTES_2, NUM_TRYTES_HASH,
                                     NUM_TRYTES_HASH));
  TEST_ASSERT(hash243_queue_push(&get_is->tips, trits_243) == RC_OK);

  serializer.vtable.get_inclusion_states_serialize_request(&serializer, get_is, serializer_out);

  TEST_ASSERT_EQUAL_STRING(json_text, serializer_out->data);

  char_buffer_free(serializer_out);
  get_inclusion_states_req_free(&get_is);
}

void test_deserialize_get_inclusion_states(void) {
  serializer_t serializer;
  init_json_serializer(&serializer);
  const char* json_text = "{\"states\": [true,false]}";
  get_inclusion_states_res_t* deserialize_get_is = get_inclusion_states_res_new();

  serializer.vtable.get_inclusion_states_deserialize_response(&serializer, json_text, deserialize_get_is);
  TEST_ASSERT_TRUE(get_inclusion_states_res_states_at(deserialize_get_is, 0) == true);
  TEST_ASSERT_TRUE(get_inclusion_states_res_states_at(deserialize_get_is, 1) == false);
  TEST_ASSERT_TRUE(get_inclusion_states_res_states_at(deserialize_get_is, 2) == false);
  TEST_ASSERT_FALSE(get_inclusion_states_res_states_at(deserialize_get_is, 2) == true);
  get_inclusion_states_res_free(&deserialize_get_is);
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_serialize_get_inclusion_states);
  RUN_TEST(test_deserialize_get_inclusion_states);
  return UNITY_END();
}
