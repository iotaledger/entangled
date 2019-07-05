/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "cclient/serialization/json/tests/shared.h"

static void test_request(void) {
  serializer_t serializer;
  const char* json_text =
      "{\"command\":\"checkConsistency\",\"tails\":"
      "[\"" TEST_81_TRYTES_1
      "\","
      "\"" TEST_81_TRYTES_2 "\"]}";

  char_buffer_t* serializer_out = char_buffer_new();
  init_json_serializer(&serializer);
  check_consistency_req_t* req = check_consistency_req_new();
  check_consistency_req_t* req_de = check_consistency_req_new();

  flex_trit_t trits_243[FLEX_TRIT_SIZE_243];

  TEST_ASSERT(flex_trits_from_trytes(trits_243, NUM_TRITS_HASH, (const tryte_t*)TEST_81_TRYTES_1, NUM_TRYTES_HASH,
                                     NUM_TRYTES_HASH));
  TEST_ASSERT(check_consistency_req_tails_add(req, trits_243) == RC_OK);

  TEST_ASSERT(flex_trits_from_trytes(trits_243, NUM_TRITS_HASH, (const tryte_t*)TEST_81_TRYTES_2, NUM_TRYTES_HASH,
                                     NUM_TRYTES_HASH));
  TEST_ASSERT(check_consistency_req_tails_add(req, trits_243) == RC_OK);

  serializer.vtable.check_consistency_serialize_request(req, serializer_out);

  TEST_ASSERT_EQUAL_STRING(json_text, serializer_out->data);

  serializer.vtable.check_consistency_deserialize_request(serializer_out->data, req_de);

  TEST_ASSERT_EQUAL_INT(hash243_queue_count(req->tails), hash243_queue_count(req_de->tails));
  TEST_ASSERT_EQUAL_MEMORY(hash243_queue_at(req->tails, 0), hash243_queue_at(req_de->tails, 0), FLEX_TRIT_SIZE_243);
  TEST_ASSERT_EQUAL_MEMORY(hash243_queue_at(req->tails, 1), hash243_queue_at(req_de->tails, 1), FLEX_TRIT_SIZE_243);

  char_buffer_free(serializer_out);
  check_consistency_req_free(&req);
  check_consistency_req_free(&req_de);
}

static void test_response(void) {
  serializer_t serializer;
  init_json_serializer(&serializer);
  const char* json_true = "{\"state\":true,\"info\":\"\"}";
  const char* json_false = "{\"state\":false,\"info\":\"" CONSISTENCY_INFO "\"}";

  check_consistency_res_t* res = check_consistency_res_new();
  char_buffer_t* serializer_out = char_buffer_new();

  serializer.vtable.check_consistency_deserialize_response(json_true, res);
  TEST_ASSERT_TRUE(res->state == true);
  TEST_ASSERT_NOT_NULL(res->info);
  TEST_ASSERT_EQUAL_STRING("", res->info->data);

  serializer.vtable.check_consistency_serialize_response(res, serializer_out);
  TEST_ASSERT_EQUAL_STRING(json_true, serializer_out->data);

  char_buffer_free(serializer_out);
  serializer_out = char_buffer_new();

  check_consistency_res_free(&res);
  res = check_consistency_res_new();

  serializer.vtable.check_consistency_deserialize_response(json_false, res);
  TEST_ASSERT_TRUE(res->state == false);
  TEST_ASSERT_NOT_NULL(res->info);
  TEST_ASSERT_EQUAL_STRING(CONSISTENCY_INFO, res->info->data);

  serializer.vtable.check_consistency_serialize_response(res, serializer_out);
  TEST_ASSERT_EQUAL_STRING(json_false, serializer_out->data);

  check_consistency_res_free(&res);
  char_buffer_free(serializer_out);
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_request);
  RUN_TEST(test_response);
  return UNITY_END();
}
