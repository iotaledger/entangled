/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdlib.h>
#include <string.h>
#include <unity/unity.h>

#include "common/model/tests/defs.h"
#include "common/model/transfer.h"

void test_bundle_hash(void) {
  transfer_ctx_t transfer_ctx = {};
  flex_trit_t address[FLEX_TRIT_SIZE_243];
  flex_trits_from_trytes(address, NUM_TRITS_ADDRESS, TEST_ADDRESS_1, NUM_TRYTES_ADDRESS, NUM_TRYTES_ADDRESS);

  flex_trit_t tag[FLEX_TRIT_SIZE_81];
  flex_trits_from_trytes(tag, NUM_TRITS_TAG, TEST_TAG, NUM_TRYTES_TAG, NUM_TRYTES_TAG);

  flex_trit_t hash[FLEX_TRIT_SIZE_243];
  flex_trits_from_trytes(hash, NUM_TRITS_HASH, TEST_HASH, NUM_TRYTES_HASH, NUM_TRYTES_HASH);

  transfer_t* transfer = transfer_value_in_new(address, tag, 0, NULL, 0, 1509136296);
  TEST_ASSERT_NOT_NULL(transfer);

  transfer_t* transfers[1] = {transfer};
  Kerl kerl = {};
  TEST_ASSERT_TRUE(transfer_ctx_init(&transfer_ctx, transfers, 1));

  transfer_ctx_hash(&transfer_ctx, &kerl, transfers, 1);
  TEST_ASSERT_EQUAL_MEMORY(hash, transfer_ctx.bundle, FLEX_TRIT_SIZE_243);
  transfer_free(&transfer);
  TEST_ASSERT_NULL(transfer);
}

void test_value_out(void) {
  iota_transaction_t* tx;
  transfer_iterator_t* tf_iter = NULL;
  flex_trit_t sig_buff[FLEX_TRIT_SIZE_6561];
  flex_trit_t seed[FLEX_TRIT_SIZE_243];
  flex_trits_from_trytes(seed, NUM_TRITS_ADDRESS, TEST_SEED, NUM_TRYTES_ADDRESS, NUM_TRYTES_ADDRESS);
  transfer_value_out_t OUTPUT = {seed, 2, 0};
  iota_transaction_t TX = {};
  flex_trit_t address_out[FLEX_TRIT_SIZE_243];
  flex_trits_from_trytes(address_out, NUM_TRITS_ADDRESS, TEST_ADDRESS_0, NUM_TRYTES_ADDRESS, NUM_TRYTES_ADDRESS);

  flex_trit_t tag[FLEX_TRIT_SIZE_81];
  flex_trits_from_trytes(tag, NUM_TRITS_TAG, TEST_TAG_NULL, NUM_TRYTES_TAG, NUM_TRYTES_TAG);
  transfer_t* transfer_out = transfer_value_out_new(&OUTPUT, tag, address_out, -1, 1548690545190);

  flex_trit_t address_in[FLEX_TRIT_SIZE_243];
  flex_trits_from_trytes(address_in, NUM_TRITS_ADDRESS, TEST_ADDRESS_1, NUM_TRYTES_ADDRESS, NUM_TRYTES_ADDRESS);
  transfer_t* transfer_in = transfer_value_in_new(address_in, tag, 1, NULL, 0, 1548690545190);

  transfer_t* transfers[2] = {transfer_in, transfer_out};
  Kerl kerl = {};
  tf_iter = transfer_iterator_new(transfers, 2, &kerl, &TX);
  TEST_ASSERT_NOT_NULL(tf_iter);

  tx = transfer_iterator_next(tf_iter);
  TEST_ASSERT_NOT_NULL(tx);
  flex_trits_from_trytes(sig_buff, NUM_TRITS_SIGNATURE, TEST_SIG_1, NUM_TRYTES_SIGNATURE, NUM_TRYTES_SIGNATURE);
  TEST_ASSERT_EQUAL_MEMORY(sig_buff, tx->data.signature_or_message, FLEX_TRIT_SIZE_6561);
  TEST_ASSERT_EQUAL_MEMORY(tag, tx->attachment.tag, FLEX_TRIT_SIZE_81);

  flex_trit_t obsolete_tag[FLEX_TRIT_SIZE_81];
  flex_trits_from_trytes(obsolete_tag, NUM_TRITS_TAG, TEST_OBSOLETE_TAG_1, NUM_TRYTES_TAG, NUM_TRYTES_TAG);
  TEST_ASSERT_EQUAL_MEMORY(obsolete_tag, tx->essence.obsolete_tag, FLEX_TRIT_SIZE_81);

  tx = transfer_iterator_next(tf_iter);
  TEST_ASSERT_NOT_NULL(tx);
  flex_trits_from_trytes(sig_buff, NUM_TRITS_SIGNATURE, TEST_SIG_2, NUM_TRYTES_SIGNATURE, NUM_TRYTES_SIGNATURE);
  TEST_ASSERT_EQUAL_MEMORY(sig_buff, tx->data.signature_or_message, FLEX_TRIT_SIZE_6561);
  TEST_ASSERT_EQUAL_MEMORY(tag, tx->attachment.tag, FLEX_TRIT_SIZE_81);
  TEST_ASSERT_EQUAL_MEMORY(tag, tx->essence.obsolete_tag, FLEX_TRIT_SIZE_81);

  tx = transfer_iterator_next(tf_iter);
  TEST_ASSERT_NOT_NULL(tx);
  flex_trits_from_trytes(sig_buff, NUM_TRITS_SIGNATURE, TEST_SIG_3, NUM_TRYTES_SIGNATURE, NUM_TRYTES_SIGNATURE);
  TEST_ASSERT_EQUAL_MEMORY(sig_buff, tx->data.signature_or_message, FLEX_TRIT_SIZE_6561);
  TEST_ASSERT_EQUAL_MEMORY(tag, tx->attachment.tag, FLEX_TRIT_SIZE_81);
  TEST_ASSERT_EQUAL_MEMORY(tag, tx->essence.obsolete_tag, FLEX_TRIT_SIZE_81);

  tx = transfer_iterator_next(tf_iter);
  TEST_ASSERT_NULL(tx);

  transfer_iterator_free(&tf_iter);
  transfer_free(&transfer_out);
  TEST_ASSERT_NULL(transfer_out);
  transfer_free(&transfer_in);
  TEST_ASSERT_NULL(transfer_in);
}

void test_transfer_data(void) {
  iota_transaction_t* tx;
  transfer_iterator_t* tf_iter = NULL;
  transfer_t* transfer = NULL;

  flex_trit_t address[FLEX_TRIT_SIZE_243];
  flex_trits_from_trytes(address, NUM_TRITS_ADDRESS, TEST_DATA_ADDRESS, NUM_TRYTES_ADDRESS, NUM_TRYTES_ADDRESS);

  flex_trit_t data[FLEX_TRIT_SIZE_243] = {};
  flex_trits_from_trytes(data, NUM_TRITS_ADDRESS, TEST_DATA, NUM_TRYTES_ADDRESS, NUM_TRYTES_ADDRESS);

  flex_trit_t tag[FLEX_TRIT_SIZE_81];
  flex_trits_from_trytes(tag, NUM_TRITS_TAG, TEST_TAG, NUM_TRYTES_TAG, NUM_TRYTES_TAG);
  transfer = transfer_data_new(address, tag, data, NUM_TRITS_ADDRESS, 1509136296);

  transfer_t* transfers[1] = {transfer};
  Kerl kerl = {};
  tf_iter = transfer_iterator_new(transfers, 1, &kerl, NULL);
  TEST_ASSERT_NOT_NULL(tf_iter);

  tx = transfer_iterator_next(tf_iter);
  TEST_ASSERT_NOT_NULL(tx);
  TEST_ASSERT_EQUAL_MEMORY(data, tx->data.signature_or_message, FLEX_TRIT_SIZE_243);

  tx = transfer_iterator_next(tf_iter);
  TEST_ASSERT_NULL(tx);

  transfer_iterator_free(&tf_iter);
  TEST_ASSERT_NULL(tf_iter);
  transfer_free(&transfer);
  TEST_ASSERT_NULL(transfer);
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_bundle_hash);
  RUN_TEST(test_value_out);
  RUN_TEST(test_transfer_data);

  return UNITY_END();
}
