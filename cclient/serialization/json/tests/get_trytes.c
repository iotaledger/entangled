/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "cclient/serialization/json/tests/shared.h"

void test_serialize_get_trytes(void) {
  serializer_t serializer;
  const char* json_text =
      "{\"command\":\"getTrytes\",\"hashes\":["
      "\"" TEST_81_TRYTES_1 "\"]}";
  char_buffer_t* serializer_out = char_buffer_new();
  init_json_serializer(&serializer);
  get_trytes_req_t* req = get_trytes_req_new();
  flex_trit_t hash[FLEX_TRIT_SIZE_243] = {};

  TEST_ASSERT(
      flex_trits_from_trytes(hash, NUM_TRITS_HASH, (const tryte_t*)TEST_81_TRYTES_1, NUM_TRYTES_HASH, NUM_TRYTES_HASH));
  TEST_ASSERT(hash243_queue_push(&req->hashes, hash) == RC_OK);

  serializer.vtable.get_trytes_serialize_request(&serializer, req, serializer_out);

  TEST_ASSERT_EQUAL_STRING(json_text, serializer_out->data);

  char_buffer_free(serializer_out);
  get_trytes_req_free(&req);
}

void test_deserialize_get_trytes(void) {
  serializer_t serializer;
  init_json_serializer(&serializer);
  const char* json_text =
      "{\"trytes\":["
      "\"" TEST_2673_TRYTES_1 "\"]}";
  flex_trit_t hash[FLEX_TRIT_SIZE_8019] = {};

  flex_trit_t* tmp_trytes = NULL;
  get_trytes_res_t* res = get_trytes_res_new();

  serializer.vtable.get_trytes_deserialize_response(&serializer, json_text, res);
  tmp_trytes = hash8019_queue_at(&res->trytes, 0);
  flex_trits_from_trytes(hash, NUM_TRITS_SERIALIZED_TRANSACTION, (const tryte_t*)TEST_2673_TRYTES_1,
                         NUM_TRYTES_SERIALIZED_TRANSACTION, NUM_TRYTES_SERIALIZED_TRANSACTION);
  TEST_ASSERT_EQUAL_MEMORY(tmp_trytes, hash, FLEX_TRIT_SIZE_8019);
  tmp_trytes = hash8019_queue_at(&res->trytes, 1);
  TEST_ASSERT_NULL(tmp_trytes);

  get_trytes_res_free(&res);
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_serialize_get_trytes);
  RUN_TEST(test_deserialize_get_trytes);

  return UNITY_END();
}
