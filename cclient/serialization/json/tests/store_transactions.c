/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "cclient/serialization/json/tests/shared.h"

static void test_store_transactions_serialize_request(void) {
  retcode_t ret = RC_OK;
  serializer_t serializer;

  char const* json_text =
      "{\"command\":\"storeTransactions\",\"trytes\":["
      "\"" TEST_2673_TRYTES_1 "\"]}";

  char_buffer_t* serializer_out = char_buffer_new();
  init_json_serializer(&serializer);

  store_transactions_req_t* req = store_transactions_req_new();
  store_transactions_req_t* req_de = store_transactions_req_new();

  flex_trit_t tx_trits[FLEX_TRIT_SIZE_8019];
  size_t len = flex_trits_from_trytes(tx_trits, NUM_TRITS_SERIALIZED_TRANSACTION, (tryte_t const*)TEST_2673_TRYTES_3,
                                      NUM_TRYTES_SERIALIZED_TRANSACTION, NUM_TRYTES_SERIALIZED_TRANSACTION);
  TEST_ASSERT(len);

  TEST_ASSERT(store_transactions_req_trytes_add(req, tx_trits) == RC_OK);

  ret = serializer.vtable.store_transactions_serialize_request(req, serializer_out);
  TEST_ASSERT_EQUAL_INT(RC_OK, ret);
  TEST_ASSERT_EQUAL_STRING(json_text, serializer_out->data);

  ret = serializer.vtable.store_transactions_deserialize_request(serializer_out->data, req_de);
  TEST_ASSERT_EQUAL_INT(RC_OK, ret);

  TEST_ASSERT_EQUAL_INT(hash_array_len(req->trytes), hash_array_len(req_de->trytes));

  TEST_ASSERT_EQUAL_MEMORY(hash_array_at(req->trytes, 0), hash_array_at(req_de->trytes, 0), FLEX_TRIT_SIZE_8019);

  char_buffer_free(serializer_out);
  store_transactions_req_free(&req);
  store_transactions_req_free(&req_de);
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_store_transactions_serialize_request);
  return UNITY_END();
}
