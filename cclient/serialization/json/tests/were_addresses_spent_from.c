/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "cclient/serialization/json/tests/shared.h"

void test_were_addresses_spent_from_serialize_request(void) {
  serializer_t serializer;
  init_json_serializer(&serializer);
  char const* json_text =
      "{\"command\":\"wereAddressesSpentFrom\",\"addresses\":["
      "\"" TEST_81_TRYTES_1 "\"]}";

  were_addresses_spent_from_req_t* addr_spent = were_addresses_spent_from_req_new();
  char_buffer_t* serializer_out = char_buffer_new();
  flex_trit_t trits_243[FLEX_TRIT_SIZE_243];

  TEST_ASSERT(flex_trits_from_trytes(trits_243, NUM_TRITS_HASH, (tryte_t const*)TEST_81_TRYTES_1, NUM_TRYTES_HASH,
                                     NUM_TRYTES_HASH));
  TEST_ASSERT(were_addresses_spent_from_req_add(addr_spent, trits_243) == RC_OK);

  serializer.vtable.were_addresses_spent_from_serialize_request(addr_spent, serializer_out);

  TEST_ASSERT_EQUAL_STRING(json_text, serializer_out->data);

  char_buffer_free(serializer_out);
  were_addresses_spent_from_req_free(&addr_spent);
}

void test_were_addresses_spent_from_deserialize_request(void) {
  serializer_t serializer;
  init_json_serializer(&serializer);
  char const* json_text = "{\"addresses\": [\"" TEST_81_TRYTES_1 "\"]}";
  flex_trit_t hash[FLEX_TRIT_SIZE_243];
  flex_trit_t* req_transaction = NULL;
  were_addresses_spent_from_req_t* deserialize_addr_spent = were_addresses_spent_from_req_new();

  serializer.vtable.were_addresses_spent_from_deserialize_request(json_text, deserialize_addr_spent);
  req_transaction = hash243_queue_at(deserialize_addr_spent->addresses, 0);
  flex_trits_from_trytes(hash, NUM_TRITS_HASH, (tryte_t*)TEST_81_TRYTES_1, NUM_TRYTES_HASH, NUM_TRYTES_HASH);
  TEST_ASSERT_EQUAL_MEMORY(hash, req_transaction, FLEX_TRIT_SIZE_243);

  were_addresses_spent_from_req_free(&deserialize_addr_spent);
}

void test_were_addresses_spent_from_serialize_response(void) {
  serializer_t serializer;
  init_json_serializer(&serializer);
  char const* json_text = "{\"states\":[true,false]}";

  were_addresses_spent_from_res_t* addr_spent = were_addresses_spent_from_res_new();
  char_buffer_t* serializer_out = char_buffer_new();

  TEST_ASSERT(were_addresses_spent_from_res_states_add(addr_spent, true) == RC_OK);
  TEST_ASSERT(were_addresses_spent_from_res_states_add(addr_spent, false) == RC_OK);
  serializer.vtable.were_addresses_spent_from_serialize_response(addr_spent, serializer_out);

  TEST_ASSERT_EQUAL_STRING(json_text, serializer_out->data);

  char_buffer_free(serializer_out);
  were_addresses_spent_from_res_free(&addr_spent);
}

void test_were_addresses_spent_from_deserialize_response(void) {
  serializer_t serializer;
  init_json_serializer(&serializer);
  char const* json_text = "{\"states\": [true,false]}";
  were_addresses_spent_from_res_t* deserialize_addr_spent = were_addresses_spent_from_res_new();

  serializer.vtable.were_addresses_spent_from_deserialize_response(json_text, deserialize_addr_spent);
  TEST_ASSERT_TRUE(were_addresses_spent_from_res_states_at(deserialize_addr_spent, 0) == true);
  TEST_ASSERT_TRUE(were_addresses_spent_from_res_states_at(deserialize_addr_spent, 1) == false);
  TEST_ASSERT_TRUE(were_addresses_spent_from_res_states_at(deserialize_addr_spent, 2) == false);
  TEST_ASSERT_FALSE(were_addresses_spent_from_res_states_at(deserialize_addr_spent, 2) == true);
  were_addresses_spent_from_res_free(&deserialize_addr_spent);
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_were_addresses_spent_from_serialize_request);
  RUN_TEST(test_were_addresses_spent_from_deserialize_request);
  RUN_TEST(test_were_addresses_spent_from_serialize_response);
  RUN_TEST(test_were_addresses_spent_from_deserialize_response);

  return UNITY_END();
}
