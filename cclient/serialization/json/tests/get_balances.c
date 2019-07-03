/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "cclient/serialization/json/tests/shared.h"

void test_get_balances_serialize_request(void) {
  serializer_t serializer;
  init_json_serializer(&serializer);
  const char* json_text =
      "{\"command\":\"getBalances\",\"addresses\":["
      "\"" TEST_81_TRYTES_1
      "\"]"
      ",\"threshold\":" STR(TEST_BALANCES_SERIALIZE_THRESHOLD) "}";
  flex_trit_t hash[FLEX_TRIT_SIZE_243] = {};

  char_buffer_t* serializer_out = char_buffer_new();
  get_balances_req_t* get_bal = get_balances_req_new();

  TEST_ASSERT_NOT_NULL(serializer_out);
  TEST_ASSERT_NOT_NULL(get_bal);
  flex_trits_from_trytes(hash, NUM_TRITS_HASH, (const tryte_t*)TEST_81_TRYTES_1, NUM_TRYTES_HASH, NUM_TRYTES_HASH);
  TEST_ASSERT(get_balances_req_address_add(get_bal, hash) == RC_OK);

  get_bal->threshold = TEST_BALANCES_SERIALIZE_THRESHOLD;
  serializer.vtable.get_balances_serialize_request(get_bal, serializer_out);

  TEST_ASSERT_EQUAL_STRING(json_text, serializer_out->data);

  char_buffer_free(serializer_out);
  get_balances_req_free(&get_bal);
}

void test_get_balances_deserialize_request(void) {
  serializer_t serializer;
  init_json_serializer(&serializer);
  const char* json_text =
      "{\"command\":\"getBalances\",\"addresses\":["
      "\"" TEST_81_TRYTES_1
      "\"]"
      ",\"threshold\":" STR(TEST_BALANCES_SERIALIZE_THRESHOLD) "}";
  flex_trit_t hash[FLEX_TRIT_SIZE_243] = {};
  flex_trits_from_trytes(hash, NUM_TRITS_HASH, (const tryte_t*)TEST_81_TRYTES_1, NUM_TRYTES_HASH, NUM_TRYTES_HASH);

  get_balances_req_t* deserialize_get_bal = get_balances_req_new();

  serializer.vtable.get_balances_deserialize_request(json_text, deserialize_get_bal);

  TEST_ASSERT_EQUAL_INT(TEST_BALANCES_SERIALIZE_THRESHOLD, deserialize_get_bal->threshold);

  TEST_ASSERT_EQUAL_MEMORY(hash, hash243_queue_at(deserialize_get_bal->addresses, 0), FLEX_TRIT_SIZE_243);

  get_balances_req_free(&deserialize_get_bal);
}

void test_get_balances_serialize_response(void) {
  serializer_t serializer;
  init_json_serializer(&serializer);
  const char* json_text =
      "{\"balances\":"
      "[\"" STR(TEST_BALANCES_BALANCE)
      "\"],"
      "\"references\":"
      "[\"" TEST_81_TRYTES_1
      "\"],"
      "\"milestoneIndex\":" STR(TEST_BALANCES_MILESTONEINDEX) "}";
  flex_trit_t hash[FLEX_TRIT_SIZE_243] = {};

  char_buffer_t* serializer_out = char_buffer_new();
  get_balances_res_t* get_bal = get_balances_res_new();

  TEST_ASSERT_NOT_NULL(serializer_out);
  TEST_ASSERT_NOT_NULL(get_bal);

  uint64_t TEST_BALANCES_BALANCE_LONG = TEST_BALANCES_BALANCE;
  TEST_ASSERT(get_balances_res_balances_add(get_bal, TEST_BALANCES_BALANCE_LONG) == RC_OK);
  flex_trits_from_trytes(hash, NUM_TRITS_HASH, (const tryte_t*)TEST_81_TRYTES_1, NUM_TRYTES_HASH, NUM_TRYTES_HASH);

  TEST_ASSERT(get_balances_res_reference_add(get_bal, hash) == RC_OK);
  get_bal->milestone_index = TEST_BALANCES_MILESTONEINDEX;

  serializer.vtable.get_balances_serialize_response(get_bal, serializer_out);

  TEST_ASSERT_EQUAL_STRING(json_text, serializer_out->data);

  char_buffer_free(serializer_out);
  get_balances_res_free(&get_bal);
}

void test_get_balances_deserialize_response(void) {
  serializer_t serializer;
  init_json_serializer(&serializer);
  const char* json_text =
      "{\"balances\": "
      "[\"" STR(TEST_BALANCES_BALANCE)
      "\"], "
      "\"references\": "
      "[\"" TEST_81_TRYTES_1
      "\"], "
      "\"milestoneIndex\":" STR(TEST_BALANCES_MILESTONEINDEX) "}";
  flex_trit_t hash[FLEX_TRIT_SIZE_243] = {};
  flex_trits_from_trytes(hash, NUM_TRITS_HASH, (const tryte_t*)TEST_81_TRYTES_1, NUM_TRYTES_HASH, NUM_TRYTES_HASH);

  get_balances_res_t* deserialize_get_bal = get_balances_res_new();
  serializer.vtable.get_balances_deserialize_response(json_text, deserialize_get_bal);

  TEST_ASSERT_EQUAL_UINT64(TEST_BALANCES_BALANCE, get_balances_res_balances_at(deserialize_get_bal, 0));

  TEST_ASSERT_EQUAL_MEMORY(hash, hash243_queue_at(deserialize_get_bal->references, 0), FLEX_TRIT_SIZE_243);

  TEST_ASSERT_EQUAL_INT(TEST_BALANCES_MILESTONEINDEX, deserialize_get_bal->milestone_index);

  get_balances_res_free(&deserialize_get_bal);
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_get_balances_serialize_request);
  RUN_TEST(test_get_balances_deserialize_request);
  RUN_TEST(test_get_balances_serialize_response);
  RUN_TEST(test_get_balances_deserialize_response);

  return UNITY_END();
}
