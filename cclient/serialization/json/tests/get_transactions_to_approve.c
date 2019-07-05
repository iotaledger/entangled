/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "cclient/serialization/json/tests/shared.h"

void test_get_transactions_to_approve_serialize_request(void) {
  serializer_t serializer;
  init_json_serializer(&serializer);
  char const* json_text =
      "{\"command\":\"getTransactionsToApprove\",\"depth\":" STR(
          TEST_TRANSACTION_TO_APPROVE_DEPTH) ",\"reference\":"
                                             "\"" TEST_81_TRYTES_1 "\"}";

  char_buffer_t* serializer_out = char_buffer_new();
  get_transactions_to_approve_req_t* get_tx_approve = get_transactions_to_approve_req_new();

  flex_trit_t reference[FLEX_TRIT_SIZE_243];
  TEST_ASSERT(flex_trits_from_trytes(reference, NUM_TRITS_HASH, (tryte_t const*)TEST_81_TRYTES_1, NUM_TRYTES_HASH,
                                     NUM_TRYTES_HASH));
  TEST_ASSERT(get_transactions_to_approve_req_set_reference(get_tx_approve, reference) == RC_OK);

  get_transactions_to_approve_req_set_depth(get_tx_approve, TEST_TRANSACTION_TO_APPROVE_DEPTH);

  serializer.vtable.get_transactions_to_approve_serialize_request(get_tx_approve, serializer_out);
  TEST_ASSERT_EQUAL_STRING(json_text, serializer_out->data);

  char_buffer_free(serializer_out);
  get_transactions_to_approve_req_free(&get_tx_approve);
}

void test_get_transactions_to_approve_serialize_request_no_reference(void) {
  serializer_t serializer;
  init_json_serializer(&serializer);
  char const* json_text =
      "{\"command\":\"getTransactionsToApprove\",\"depth\":" STR(TEST_TRANSACTION_TO_APPROVE_DEPTH) "}";

  char_buffer_t* serializer_out = char_buffer_new();
  get_transactions_to_approve_req_t* get_tx_approve = get_transactions_to_approve_req_new();

  get_transactions_to_approve_req_set_depth(get_tx_approve, TEST_TRANSACTION_TO_APPROVE_DEPTH);

  serializer.vtable.get_transactions_to_approve_serialize_request(get_tx_approve, serializer_out);
  TEST_ASSERT_EQUAL_STRING(json_text, serializer_out->data);

  char_buffer_free(serializer_out);
  get_transactions_to_approve_req_free(&get_tx_approve);
}

void test_get_transactions_to_approve_deserialize_request(void) {
  serializer_t serializer;
  init_json_serializer(&serializer);
  char const* json_text =
      "{\"command\":\"getTransactionsToApprove\",\"depth\":" STR(
          TEST_TRANSACTION_TO_APPROVE_DEPTH) ",\"reference\":"
                                             "\"" TEST_81_TRYTES_1 "\"}";
  flex_trit_t hash[FLEX_TRIT_SIZE_243] = {};
  flex_trits_from_trytes(hash, NUM_TRITS_HASH, (tryte_t const*)TEST_81_TRYTES_1, NUM_TRYTES_HASH, NUM_TRYTES_HASH);

  get_transactions_to_approve_req_t* req = get_transactions_to_approve_req_new();

  serializer.vtable.get_transactions_to_approve_deserialize_request(json_text, req);
  TEST_ASSERT_EQUAL_UINT32(TEST_TRANSACTION_TO_APPROVE_DEPTH, req->depth);

  TEST_ASSERT_EQUAL_MEMORY(hash, req->reference, FLEX_TRIT_SIZE_243);

  get_transactions_to_approve_req_free(&req);
}

void test_get_transactions_to_approve_deserialize_request_no_reference(void) {
  serializer_t serializer;
  init_json_serializer(&serializer);
  char const* json_text =
      "{\"command\":\"getTransactionsToApprove\",\"depth\":" STR(TEST_TRANSACTION_TO_APPROVE_DEPTH) "}";
  get_transactions_to_approve_req_t* req = get_transactions_to_approve_req_new();
  serializer.vtable.get_transactions_to_approve_deserialize_request(json_text, req);
  TEST_ASSERT_EQUAL_UINT32(TEST_TRANSACTION_TO_APPROVE_DEPTH, req->depth);

  get_transactions_to_approve_req_free(&req);
}

void test_get_transactions_to_approve_serialize_response(void) {
  serializer_t serializer;
  init_json_serializer(&serializer);
  char const* json_text =
      "{\"trunkTransaction\":"
      "\"" TEST_81_TRYTES_1
      "\",\"branchTransaction\":"
      "\"" TEST_81_TRYTES_2 "\"}";

  char_buffer_t* serializer_out = char_buffer_new();
  get_transactions_to_approve_res_t* res = get_transactions_to_approve_res_new();

  flex_trit_t tx_hash[FLEX_TRIT_SIZE_243];
  TEST_ASSERT(flex_trits_from_trytes(tx_hash, NUM_TRITS_HASH, (tryte_t const*)TEST_81_TRYTES_1, NUM_TRYTES_HASH,
                                     NUM_TRYTES_HASH));
  get_transactions_to_approve_res_set_trunk(res, tx_hash);

  TEST_ASSERT(flex_trits_from_trytes(tx_hash, NUM_TRITS_HASH, (tryte_t const*)TEST_81_TRYTES_2, NUM_TRYTES_HASH,
                                     NUM_TRYTES_HASH));
  get_transactions_to_approve_res_set_branch(res, tx_hash);

  serializer.vtable.get_transactions_to_approve_serialize_response(res, serializer_out);
  TEST_ASSERT_EQUAL_STRING(json_text, serializer_out->data);

  char_buffer_free(serializer_out);
  get_transactions_to_approve_res_free(&res);
}

void test_get_transactions_to_approve_deserialize_response(void) {
  serializer_t serializer;
  init_json_serializer(&serializer);
  char const* json_text =
      "{\"trunkTransaction\": "
      "\"" TEST_81_TRYTES_1
      "\",\"branchTransaction\":"
      "\"" TEST_81_TRYTES_2 "\"}";
  flex_trit_t hash[FLEX_TRIT_SIZE_243] = {};

  get_transactions_to_approve_res_t* deserialize_get_tx_approve = get_transactions_to_approve_res_new();
  serializer.vtable.get_transactions_to_approve_deserialize_response(json_text, deserialize_get_tx_approve);

  flex_trits_from_trytes(hash, NUM_TRITS_HASH, (tryte_t const*)TEST_81_TRYTES_1, NUM_TRYTES_HASH, NUM_TRYTES_HASH);
  TEST_ASSERT_EQUAL_MEMORY(hash, deserialize_get_tx_approve->trunk, FLEX_TRIT_SIZE_243);

  flex_trits_from_trytes(hash, NUM_TRITS_HASH, (tryte_t const*)TEST_81_TRYTES_2, NUM_TRYTES_HASH, NUM_TRYTES_HASH);
  TEST_ASSERT_EQUAL_MEMORY(hash, deserialize_get_tx_approve->branch, FLEX_TRIT_SIZE_243);

  get_transactions_to_approve_res_free(&deserialize_get_tx_approve);
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_get_transactions_to_approve_serialize_request);
  RUN_TEST(test_get_transactions_to_approve_serialize_request_no_reference);
  RUN_TEST(test_get_transactions_to_approve_deserialize_request);
  RUN_TEST(test_get_transactions_to_approve_deserialize_request_no_reference);
  RUN_TEST(test_get_transactions_to_approve_serialize_response);
  RUN_TEST(test_get_transactions_to_approve_deserialize_response);

  return UNITY_END();
}
