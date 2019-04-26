/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "cclient/serialization/json/tests/shared.h"

static void test_request(void) {
  retcode_t ret = RC_OK;
  serializer_t serializer;

  const char* json_text =
      "{\"command\":\"broadcastTransactions\",\"trytes\":["
      "\"" TEST_2673_TRYTES_3 "\"]}";

  char_buffer_t* serializer_out = char_buffer_new();
  init_json_serializer(&serializer);

  broadcast_transactions_req_t* req = broadcast_transactions_req_new();
  broadcast_transactions_req_t* req_de = broadcast_transactions_req_new();

  flex_trit_t tx_trits[FLEX_TRIT_SIZE_8019];
  size_t len = flex_trits_from_trytes(tx_trits, NUM_TRITS_SERIALIZED_TRANSACTION, (const tryte_t*)TEST_2673_TRYTES_3,
                                      NUM_TRYTES_SERIALIZED_TRANSACTION, NUM_TRYTES_SERIALIZED_TRANSACTION);
  TEST_ASSERT(len);

  broadcast_transactions_req_trytes_add(req, tx_trits);

  ret = serializer.vtable.broadcast_transactions_serialize_request(req, serializer_out);
  TEST_ASSERT_EQUAL_INT(RC_OK, ret);
  TEST_ASSERT_EQUAL_STRING(json_text, serializer_out->data);

  ret = serializer.vtable.broadcast_transactions_deserialize_request(serializer_out->data, req_de);
  TEST_ASSERT_EQUAL_INT(RC_OK, ret);

  TEST_ASSERT_EQUAL_INT(hash_array_len(req->trytes), hash_array_len(req_de->trytes));

  TEST_ASSERT_EQUAL_MEMORY(hash_array_at(req->trytes, 0), hash_array_at(req_de->trytes, 0), FLEX_TRIT_SIZE_8019);

  char_buffer_free(serializer_out);
  broadcast_transactions_req_free(&req);
  broadcast_transactions_req_free(&req_de);
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_request);
  return UNITY_END();
}
