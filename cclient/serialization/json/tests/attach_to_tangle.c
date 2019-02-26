/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "shared.h"

void test_serialize_attach_to_tangle(void) {
  serializer_t serializer;
  init_json_serializer(&serializer);
  const char* json_text =
      "{\"command\":\"attachToTangle\",\"trunkTransaction\":\"" TEST_81_TRYTES_1
      "\",\"branchTransaction\":\"" TEST_81_TRYTES_2
      "\",\"minWeightMagnitude\":" STR(
          TEST_MWM) ",\"trytes\":[\"" TEST_2673_TRYTES_1
                    "\",\"" TEST_2673_TRYTES_2 "\"]}";

  char_buffer_t* serializer_out = char_buffer_new();
  attach_to_tangle_req_t* attach_req = attach_to_tangle_req_new();
  hash8019_array_p trytes = hash8019_array_new();
  flex_trit_t trits_8019[FLEX_TRIT_SIZE_8019];

  TEST_ASSERT_NOT_NULL(trytes);
  TEST_ASSERT_NOT_NULL(serializer_out);
  TEST_ASSERT_NOT_NULL(attach_req);

  TEST_ASSERT(flex_trits_from_trytes(attach_req->trunk, NUM_TRITS_HASH,
                                     (const tryte_t*)TEST_81_TRYTES_1,
                                     NUM_TRYTES_HASH, NUM_TRYTES_HASH));
  TEST_ASSERT(flex_trits_from_trytes(attach_req->branch, NUM_TRITS_HASH,
                                     (const tryte_t*)TEST_81_TRYTES_2,
                                     NUM_TRYTES_HASH, NUM_TRYTES_HASH));

  TEST_ASSERT(flex_trits_from_trytes(
      trits_8019, NUM_TRITS_SERIALIZED_TRANSACTION,
      (const tryte_t*)TEST_2673_TRYTES_1, NUM_TRYTES_SERIALIZED_TRANSACTION,
      NUM_TRYTES_SERIALIZED_TRANSACTION));
  hash_array_push(trytes, trits_8019);

  TEST_ASSERT(flex_trits_from_trytes(
      trits_8019, NUM_TRITS_SERIALIZED_TRANSACTION,
      (const tryte_t*)TEST_2673_TRYTES_2, NUM_TRYTES_SERIALIZED_TRANSACTION,
      NUM_TRYTES_SERIALIZED_TRANSACTION));
  hash_array_push(trytes, trits_8019);

  attach_req->mwm = TEST_MWM;
  attach_req->trytes = trytes;

  serializer.vtable.attach_to_tangle_serialize_request(&serializer, attach_req,
                                                       serializer_out);

  TEST_ASSERT_EQUAL_STRING(json_text, serializer_out->data);

  hash_array_free(trytes);
  char_buffer_free(serializer_out);
  attach_to_tangle_req_free(&attach_req);
}

void test_deserialize_attach_to_tangle(void) {
  serializer_t serializer;
  init_json_serializer(&serializer);
  const char* json_text = "{\"trytes\":[\"" TEST_2673_TRYTES_1
                          "\",\"" TEST_2673_TRYTES_2 "\"],\"duration\":4}";
  flex_trit_t* tmp_hash = NULL;
  flex_trit_t hash[FLEX_TRIT_SIZE_8019] = {};

  attach_to_tangle_res_t* attach_res = attach_to_tangle_res_new();

  serializer.vtable.attach_to_tangle_deserialize_response(
      &serializer, json_text, attach_res);

  tmp_hash = attach_to_tangle_res_trytes_at(attach_res, 0);
  flex_trits_from_trytes(hash, NUM_TRITS_SERIALIZED_TRANSACTION,
                         (const tryte_t*)TEST_2673_TRYTES_1,
                         NUM_TRYTES_SERIALIZED_TRANSACTION,
                         NUM_TRYTES_SERIALIZED_TRANSACTION);
  TEST_ASSERT_EQUAL_MEMORY(hash, tmp_hash, FLEX_TRIT_SIZE_8019);

  tmp_hash = attach_to_tangle_res_trytes_at(attach_res, 1);
  flex_trits_from_trytes(hash, NUM_TRITS_SERIALIZED_TRANSACTION,
                         (const tryte_t*)TEST_2673_TRYTES_2,
                         NUM_TRYTES_SERIALIZED_TRANSACTION,
                         NUM_TRYTES_SERIALIZED_TRANSACTION);
  TEST_ASSERT_EQUAL_MEMORY(hash, tmp_hash, FLEX_TRIT_SIZE_8019);

  tmp_hash = attach_to_tangle_res_trytes_at(attach_res, 3);
  TEST_ASSERT_NULL(tmp_hash);

  attach_to_tangle_res_free(&attach_res);
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_serialize_attach_to_tangle);
  RUN_TEST(test_deserialize_attach_to_tangle);

  return UNITY_END();
}
