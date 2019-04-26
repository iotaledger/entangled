/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "cclient/serialization/json/tests/shared.h"

void test_get_missing_transactions_serialize_response(void) {
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
  get_missing_transactions_res_t* res = get_missing_transactions_res_new();

  flex_trit_t trits_243[FLEX_TRIT_SIZE_243];
  TEST_ASSERT(flex_trits_from_trytes(trits_243, NUM_TRITS_HASH, (tryte_t const*)TEST_81_TRYTES_3, NUM_TRYTES_HASH,
                                     NUM_TRYTES_HASH));
  TEST_ASSERT(get_missing_transactions_res_hashes_add(res, trits_243) == RC_OK);
  TEST_ASSERT(flex_trits_from_trytes(trits_243, NUM_TRITS_HASH, (tryte_t const*)TEST_81_TRYTES_2, NUM_TRYTES_HASH,
                                     NUM_TRYTES_HASH));
  TEST_ASSERT(get_missing_transactions_res_hashes_add(res, trits_243) == RC_OK);
  TEST_ASSERT(flex_trits_from_trytes(trits_243, NUM_TRITS_HASH, (tryte_t const*)TEST_81_TRYTES_1, NUM_TRYTES_HASH,
                                     NUM_TRYTES_HASH));
  TEST_ASSERT(get_missing_transactions_res_hashes_add(res, trits_243) == RC_OK);

  serializer.vtable.get_missing_transactions_serialize_response(res, out);

  TEST_ASSERT_EQUAL_STRING(json_text, out->data);

  char_buffer_free(out);
  get_missing_transactions_res_free(&res);
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_get_missing_transactions_serialize_response);

  return UNITY_END();
}
