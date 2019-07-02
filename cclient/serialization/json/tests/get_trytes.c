/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "cclient/serialization/json/tests/shared.h"

void test_get_trytes_serialize_request(void) {
  serializer_t serializer;
  char const* json_text =
      "{\"command\":\"getTrytes\",\"hashes\":["
      "\"" TEST_81_TRYTES_1 "\"]}";
  char_buffer_t* serializer_out = char_buffer_new();
  init_json_serializer(&serializer);
  get_trytes_req_t* req = get_trytes_req_new();
  flex_trit_t hash[FLEX_TRIT_SIZE_243] = {};

  TEST_ASSERT(
      flex_trits_from_trytes(hash, NUM_TRITS_HASH, (tryte_t const*)TEST_81_TRYTES_1, NUM_TRYTES_HASH, NUM_TRYTES_HASH));
  TEST_ASSERT(hash243_queue_push(&req->hashes, hash) == RC_OK);

  serializer.vtable.get_trytes_serialize_request(req, serializer_out);

  TEST_ASSERT_EQUAL_STRING(json_text, serializer_out->data);

  char_buffer_free(serializer_out);
  get_trytes_req_free(&req);
}

void test_get_trytes_deserialize_request(void) {
  serializer_t serializer;
  init_json_serializer(&serializer);
  char const* json_text =
      "{\"command\":\"getTrytes\",\"hashes\":["
      "\"" TEST_81_TRYTES_1 "\"]}";
  flex_trit_t hash[FLEX_TRIT_SIZE_243] = {};

  flex_trit_t* req_hash = NULL;
  get_trytes_req_t* req = get_trytes_req_new();

  serializer.vtable.get_trytes_deserialize_request(json_text, req);
  req_hash = hash243_queue_at(req->hashes, 0);
  flex_trits_from_trytes(hash, NUM_TRITS_HASH, (tryte_t const*)TEST_81_TRYTES_1, NUM_TRYTES_HASH, NUM_TRYTES_HASH);
  TEST_ASSERT_EQUAL_MEMORY(hash, req_hash, FLEX_TRIT_SIZE_243);
  req_hash = hash243_queue_at(req->hashes, 1);
  TEST_ASSERT_NULL(req_hash);

  get_trytes_req_free(&req);
}

void test_get_trytes_serialize_response(void) {
  serializer_t serializer;
  char const* json_text =
      "{\"trytes\":["
      "\"" TEST_2673_TRYTES_1 "\"]}";
  char_buffer_t* serializer_out = char_buffer_new();
  init_json_serializer(&serializer);
  get_trytes_res_t* res = get_trytes_res_new();
  flex_trit_t trytes[FLEX_TRIT_SIZE_8019] = {};

  TEST_ASSERT(flex_trits_from_trytes(trytes, NUM_TRITS_SERIALIZED_TRANSACTION, (tryte_t const*)TEST_2673_TRYTES_1,
                                     NUM_TRYTES_SERIALIZED_TRANSACTION, NUM_TRYTES_SERIALIZED_TRANSACTION));
  TEST_ASSERT(hash8019_queue_push(&res->trytes, trytes) == RC_OK);

  serializer.vtable.get_trytes_serialize_response(res, serializer_out);

  TEST_ASSERT_EQUAL_STRING(json_text, serializer_out->data);

  char_buffer_free(serializer_out);
  get_trytes_res_free(&res);
}

void test_get_trytes_deserialize_response(void) {
  serializer_t serializer;
  init_json_serializer(&serializer);
  char const* json_text =
      "{\"trytes\":["
      "\"" TEST_2673_TRYTES_1 "\"]}";
  flex_trit_t hash[FLEX_TRIT_SIZE_8019] = {};

  flex_trit_t* tmp_trytes = NULL;
  get_trytes_res_t* res = get_trytes_res_new();

  serializer.vtable.get_trytes_deserialize_response(json_text, res);
  tmp_trytes = hash8019_queue_at(res->trytes, 0);
  flex_trits_from_trytes(hash, NUM_TRITS_SERIALIZED_TRANSACTION, (tryte_t const*)TEST_2673_TRYTES_1,
                         NUM_TRYTES_SERIALIZED_TRANSACTION, NUM_TRYTES_SERIALIZED_TRANSACTION);
  TEST_ASSERT_EQUAL_MEMORY(tmp_trytes, hash, FLEX_TRIT_SIZE_8019);
  tmp_trytes = hash8019_queue_at(res->trytes, 1);
  TEST_ASSERT_NULL(tmp_trytes);

  get_trytes_res_free(&res);
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_get_trytes_serialize_request);
  RUN_TEST(test_get_trytes_deserialize_request);
  RUN_TEST(test_get_trytes_serialize_response);
  RUN_TEST(test_get_trytes_deserialize_response);

  return UNITY_END();
}
