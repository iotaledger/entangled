/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "cclient/serialization/json/tests/shared.h"

static void test_request(void) {
  serializer_t serializer;
  init_json_serializer(&serializer);
  const char* json_text =
      "{\"command\":\"attachToTangle\",\"trunkTransaction\":\"" TEST_81_TRYTES_1
      "\",\"branchTransaction\":\"" TEST_81_TRYTES_2
      "\",\"minWeightMagnitude\":" STR(TEST_MWM) ",\"trytes\":[\"" TEST_2673_TRYTES_1 "\",\"" TEST_2673_TRYTES_2 "\"]}";

  char_buffer_t* serializer_out = char_buffer_new();
  attach_to_tangle_req_t* attach_req = attach_to_tangle_req_new();
  attach_to_tangle_req_t* attach_req_de = attach_to_tangle_req_new();
  flex_trit_t trits_8019[FLEX_TRIT_SIZE_8019];

  TEST_ASSERT_NOT_NULL(serializer_out);
  TEST_ASSERT_NOT_NULL(attach_req);

  TEST_ASSERT(flex_trits_from_trytes(attach_req->trunk, NUM_TRITS_HASH, (const tryte_t*)TEST_81_TRYTES_1,
                                     NUM_TRYTES_HASH, NUM_TRYTES_HASH));
  TEST_ASSERT(flex_trits_from_trytes(attach_req->branch, NUM_TRITS_HASH, (const tryte_t*)TEST_81_TRYTES_2,
                                     NUM_TRYTES_HASH, NUM_TRYTES_HASH));

  TEST_ASSERT(flex_trits_from_trytes(trits_8019, NUM_TRITS_SERIALIZED_TRANSACTION, (const tryte_t*)TEST_2673_TRYTES_1,
                                     NUM_TRYTES_SERIALIZED_TRANSACTION, NUM_TRYTES_SERIALIZED_TRANSACTION));
  TEST_ASSERT(attach_to_tangle_req_trytes_add(attach_req, trits_8019) == RC_OK);

  TEST_ASSERT(flex_trits_from_trytes(trits_8019, NUM_TRITS_SERIALIZED_TRANSACTION, (const tryte_t*)TEST_2673_TRYTES_2,
                                     NUM_TRYTES_SERIALIZED_TRANSACTION, NUM_TRYTES_SERIALIZED_TRANSACTION));
  TEST_ASSERT(attach_to_tangle_req_trytes_add(attach_req, trits_8019) == RC_OK);

  TEST_ASSERT_NOT_NULL(attach_req->trytes);

  attach_req->mwm = TEST_MWM;

  serializer.vtable.attach_to_tangle_serialize_request(attach_req, serializer_out);

  TEST_ASSERT_EQUAL_STRING(json_text, serializer_out->data);

  serializer.vtable.attach_to_tangle_deserialize_request(serializer_out->data, attach_req_de);

  TEST_ASSERT_EQUAL_INT(TEST_MWM, attach_req_de->mwm);
  TEST_ASSERT_EQUAL_MEMORY(attach_req->trunk, attach_req_de->trunk, FLEX_TRIT_SIZE_243);
  TEST_ASSERT_EQUAL_MEMORY(attach_req->branch, attach_req_de->branch, FLEX_TRIT_SIZE_243);

  TEST_ASSERT_EQUAL_INT(hash_array_len(attach_req->trytes), hash_array_len(attach_req_de->trytes));

  TEST_ASSERT_EQUAL_MEMORY(hash_array_at(attach_req->trytes, 0), hash_array_at(attach_req_de->trytes, 0),
                           FLEX_TRIT_SIZE_8019);
  TEST_ASSERT_EQUAL_MEMORY(hash_array_at(attach_req->trytes, 1), hash_array_at(attach_req_de->trytes, 1),
                           FLEX_TRIT_SIZE_8019);

  char_buffer_free(serializer_out);
  attach_to_tangle_req_free(&attach_req);
  attach_to_tangle_req_free(&attach_req_de);
}

static void test_response(void) {
  serializer_t serializer;
  init_json_serializer(&serializer);
  const char* json_text = "{\"trytes\":[\"" TEST_2673_TRYTES_1 "\",\"" TEST_2673_TRYTES_2 "\"]}";
  flex_trit_t* tmp_hash = NULL;
  flex_trit_t hash[FLEX_TRIT_SIZE_8019] = {};
  retcode_t ret;

  attach_to_tangle_res_t* attach_res = attach_to_tangle_res_new();
  char_buffer_t* serializer_out = char_buffer_new();

  ret = serializer.vtable.attach_to_tangle_deserialize_response(json_text, attach_res);
  TEST_ASSERT_EQUAL_INT(RC_OK, ret);

  tmp_hash = attach_to_tangle_res_trytes_at(attach_res, 0);
  flex_trits_from_trytes(hash, NUM_TRITS_SERIALIZED_TRANSACTION, (const tryte_t*)TEST_2673_TRYTES_1,
                         NUM_TRYTES_SERIALIZED_TRANSACTION, NUM_TRYTES_SERIALIZED_TRANSACTION);
  TEST_ASSERT_EQUAL_MEMORY(hash, tmp_hash, FLEX_TRIT_SIZE_8019);

  tmp_hash = attach_to_tangle_res_trytes_at(attach_res, 1);
  flex_trits_from_trytes(hash, NUM_TRITS_SERIALIZED_TRANSACTION, (const tryte_t*)TEST_2673_TRYTES_2,
                         NUM_TRYTES_SERIALIZED_TRANSACTION, NUM_TRYTES_SERIALIZED_TRANSACTION);
  TEST_ASSERT_EQUAL_MEMORY(hash, tmp_hash, FLEX_TRIT_SIZE_8019);

  tmp_hash = attach_to_tangle_res_trytes_at(attach_res, 3);
  TEST_ASSERT_NULL(tmp_hash);

  ret = serializer.vtable.attach_to_tangle_serialize_response(attach_res, serializer_out);
  TEST_ASSERT_EQUAL_INT(RC_OK, ret);

  TEST_ASSERT_EQUAL_STRING(json_text, serializer_out->data);

  attach_to_tangle_res_free(&attach_res);
  char_buffer_free(serializer_out);
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_request);
  RUN_TEST(test_response);

  return UNITY_END();
}
