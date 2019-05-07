/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "cclient/serialization/json/tests/shared.h"

void test_get_tips_serialize_request(void) {
  serializer_t serializer;
  char const* json_text = "{\"command\":\"getTips\"}";

  char_buffer_t* serializer_out = char_buffer_new();
  init_json_serializer(&serializer);

  serializer.vtable.get_tips_serialize_request(serializer_out);

  TEST_ASSERT_EQUAL_STRING(json_text, serializer_out->data);

  char_buffer_free(serializer_out);
}

void test_get_tips_serialize_response(void) {
  serializer_t serializer;
  init_json_serializer(&serializer);
  char_buffer_t* out = char_buffer_new();
  char const* json_text =
      "{\"hashes\":["
      "\"" TEST_81_TRYTES_1
      "\","
      "\"" TEST_81_TRYTES_2
      "\","
      "\"" TEST_81_TRYTES_3 "\"]}";
  get_tips_res_t* res = get_tips_res_new();

  flex_trit_t trits_243[FLEX_TRIT_SIZE_243];
  TEST_ASSERT(flex_trits_from_trytes(trits_243, NUM_TRITS_HASH, (tryte_t const*)TEST_81_TRYTES_3, NUM_TRYTES_HASH,
                                     NUM_TRYTES_HASH));
  TEST_ASSERT(get_tips_res_hashes_push(res, trits_243) == RC_OK);
  TEST_ASSERT(flex_trits_from_trytes(trits_243, NUM_TRITS_HASH, (tryte_t const*)TEST_81_TRYTES_2, NUM_TRYTES_HASH,
                                     NUM_TRYTES_HASH));
  TEST_ASSERT(get_tips_res_hashes_push(res, trits_243) == RC_OK);
  TEST_ASSERT(flex_trits_from_trytes(trits_243, NUM_TRITS_HASH, (tryte_t const*)TEST_81_TRYTES_1, NUM_TRYTES_HASH,
                                     NUM_TRYTES_HASH));
  TEST_ASSERT(get_tips_res_hashes_push(res, trits_243) == RC_OK);

  serializer.vtable.get_tips_serialize_response(res, out);

  TEST_ASSERT_EQUAL_STRING(json_text, out->data);

  char_buffer_free(out);
  get_tips_res_free(&res);
}

void test_get_tips_deserialize_response(void) {
  serializer_t serializer;
  init_json_serializer(&serializer);
  char const* json_text =
      "{\"hashes\":["
      "\"" TEST_81_TRYTES_1
      "\","
      "\"" TEST_81_TRYTES_2
      "\","
      "\"" TEST_81_TRYTES_3 "\"],\"duration\":4}";

  flex_trit_t hash[FLEX_TRIT_SIZE_243] = {};

  get_tips_res_t* tips_res = get_tips_res_new();

  serializer.vtable.get_tips_deserialize_response(json_text, tips_res);

  hash243_stack_entry_t* hashes = tips_res->hashes;

  flex_trits_from_trytes(hash, NUM_TRITS_HASH, (tryte_t const*)TEST_81_TRYTES_3, NUM_TRYTES_HASH, NUM_TRYTES_HASH);
  TEST_ASSERT_EQUAL_MEMORY(hashes->hash, hash, FLEX_TRIT_SIZE_243);
  hashes = hashes->next;

  flex_trits_from_trytes(hash, NUM_TRITS_HASH, (tryte_t const*)TEST_81_TRYTES_2, NUM_TRYTES_HASH, NUM_TRYTES_HASH);
  TEST_ASSERT_EQUAL_MEMORY(hashes->hash, hash, FLEX_TRIT_SIZE_243);
  hashes = hashes->next;

  flex_trits_from_trytes(hash, NUM_TRITS_HASH, (tryte_t const*)TEST_81_TRYTES_1, NUM_TRYTES_HASH, NUM_TRYTES_HASH);
  TEST_ASSERT_EQUAL_MEMORY(hashes->hash, hash, FLEX_TRIT_SIZE_243);
  hashes = hashes->next;

  TEST_ASSERT_NULL(hashes);

  get_tips_res_free(&tips_res);
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_get_tips_serialize_request);
  RUN_TEST(test_get_tips_serialize_response);
  RUN_TEST(test_get_tips_deserialize_response);
  return UNITY_END();
}
