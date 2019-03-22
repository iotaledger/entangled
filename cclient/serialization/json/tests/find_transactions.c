/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "cclient/serialization/json/tests/shared.h"

void test_serialize_find_transactions(void) {
  serializer_t serializer;
  init_json_serializer(&serializer);
  const char* json_text =
      "{\"command\":\"findTransactions\",\"addresses\":["
      "\"" TEST_81_TRYTES_1
      "\"],\"approvees\":["
      "\"" TEST_81_TRYTES_2
      "\"],\"bundles\":["
      "\"" TEST_81_TRYTES_3
      "\"],\"tags\":["
      "\"" TEST_27_TRYTES_1 "\"]}";

  find_transactions_req_t* find_tran = find_transactions_req_new();
  char_buffer_t* serializer_out = char_buffer_new();
  flex_trit_t hash[FLEX_TRIT_SIZE_243] = {};
  flex_trit_t tag[FLEX_TRIT_SIZE_81] = {};

  flex_trits_from_trytes(hash, NUM_TRITS_HASH, (const tryte_t*)TEST_81_TRYTES_1, NUM_TRYTES_HASH, NUM_TRYTES_HASH);
  TEST_ASSERT(hash243_queue_push(&find_tran->addresses, hash) == RC_OK);

  flex_trits_from_trytes(hash, NUM_TRITS_HASH, (const tryte_t*)TEST_81_TRYTES_2, NUM_TRYTES_HASH, NUM_TRYTES_HASH);
  TEST_ASSERT(hash243_queue_push(&find_tran->approvees, hash) == RC_OK);

  flex_trits_from_trytes(hash, NUM_TRITS_HASH, (const tryte_t*)TEST_81_TRYTES_3, NUM_TRYTES_HASH, NUM_TRYTES_HASH);
  TEST_ASSERT(hash243_queue_push(&find_tran->bundles, hash) == RC_OK);

  flex_trits_from_trytes(tag, NUM_TRITS_TAG, (const tryte_t*)TEST_27_TRYTES_1, NUM_TRYTES_TAG, NUM_TRYTES_TAG);
  TEST_ASSERT(hash81_queue_push(&find_tran->tags, tag) == RC_OK);

  serializer.vtable.find_transactions_serialize_request(&serializer, find_tran, serializer_out);

  TEST_ASSERT_EQUAL_STRING(json_text, serializer_out->data);

  char_buffer_free(serializer_out);
  find_transactions_req_free(&find_tran);
}

void test_deserialize_find_transactions(void) {
  serializer_t serializer;
  init_json_serializer(&serializer);
  const char* json_text =
      "{\"hashes\": "
      "[\"" TEST_81_TRYTES_1
      "\", "
      "\"" TEST_81_TRYTES_2
      "\", "
      "\"" TEST_81_TRYTES_3 "\"]}";

  flex_trit_t hash[FLEX_TRIT_SIZE_243] = {};

  find_transactions_res_t* deserialize_find_tran = find_transactions_res_new();

  serializer.vtable.find_transactions_deserialize_response(&serializer, json_text, deserialize_find_tran);

  flex_trits_from_trytes(hash, NUM_TRITS_HASH, (const tryte_t*)TEST_81_TRYTES_1, NUM_TRYTES_HASH, NUM_TRYTES_HASH);
  TEST_ASSERT_EQUAL_MEMORY(hash, hash243_queue_at(&deserialize_find_tran->hashes, 0), FLEX_TRIT_SIZE_243);

  flex_trits_from_trytes(hash, NUM_TRITS_HASH, (const tryte_t*)TEST_81_TRYTES_2, NUM_TRYTES_HASH, NUM_TRYTES_HASH);
  TEST_ASSERT_EQUAL_MEMORY(hash, hash243_queue_at(&deserialize_find_tran->hashes, 1), FLEX_TRIT_SIZE_243);

  flex_trits_from_trytes(hash, NUM_TRITS_HASH, (const tryte_t*)TEST_81_TRYTES_3, NUM_TRYTES_HASH, NUM_TRYTES_HASH);
  TEST_ASSERT_EQUAL_MEMORY(hash, hash243_queue_at(&deserialize_find_tran->hashes, 2), FLEX_TRIT_SIZE_243);

  TEST_ASSERT_EQUAL_INT(3, hash243_queue_count(deserialize_find_tran->hashes));

  find_transactions_res_free(&deserialize_find_tran);
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_serialize_find_transactions);
  RUN_TEST(test_deserialize_find_transactions);
  return UNITY_END();
}
