/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "cclient/serialization/json/tests/shared.h"

void test_serialize_broadcast_transactions(void) {
  serializer_t serializer;
  const char* json_text =
      "{\"command\":\"broadcastTransactions\",\"trytes\":["
      "\"" TEST_2673_TRYTES_3 "\"]}";

  char_buffer_t* serializer_out = char_buffer_new();
  init_json_serializer(&serializer);
  broadcast_transactions_req_t* req = broadcast_transactions_req_new();
  flex_trit_t tx_trits[FLEX_TRIT_SIZE_8019];
  size_t len = flex_trits_from_trytes(
      tx_trits, NUM_TRITS_SERIALIZED_TRANSACTION,
      (const tryte_t*)TEST_2673_TRYTES_3, NUM_TRYTES_SERIALIZED_TRANSACTION,
      NUM_TRYTES_SERIALIZED_TRANSACTION);
  TEST_ASSERT(len);

  hash_array_push(req->trytes, tx_trits);

  serializer.vtable.broadcast_transactions_serialize_request(&serializer, req,
                                                             serializer_out);
  TEST_ASSERT_EQUAL_STRING(json_text, serializer_out->data);

  char_buffer_free(serializer_out);
  broadcast_transactions_req_free(&req);
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_serialize_broadcast_transactions);
  return UNITY_END();
}
