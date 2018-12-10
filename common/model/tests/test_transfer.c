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
  transfer_t* transfer =
      transfer_value_in_new(ADDRESS, TAG, 0, NULL, 0, 1509136296);
  TEST_ASSERT_NOT_NULL(transfer);

  transfer_t* transfers[1] = {transfer};
  Kerl kerl = {};
  init_kerl(&kerl);
  transfer_ctx_t* transfer_ctx = transfer_ctx_new();
  transfer_ctx_init(transfer_ctx, transfers, 1);
  transfer_ctx_hash(transfer_ctx, &kerl, transfers, 1);
  TEST_ASSERT_EQUAL_MEMORY(HASH, transfer_ctx->bundle, sizeof(HASH));
  transfer_ctx_free(transfer_ctx);
  transfer_free(&transfer);
  TEST_ASSERT_NULL(transfer);
}

void test_value_out(void) {
  iota_transaction_t tx;
  transfer_iterator_t* tf_iter = NULL;
  transfer_value_out_t OUTPUT = {SEED, 3, 5};
  struct _iota_transaction TX;
  transfer_t* transfer =
      transfer_value_out_new(&OUTPUT, TAG, ADDRESS, 0, 1509136296);

  transfer_t* transfers[1] = {transfer};
  Kerl kerl = {};
  init_kerl(&kerl);
  tf_iter = transfer_iterator_new(transfers, 1, &kerl, &TX);
  TEST_ASSERT_NOT_NULL(tf_iter);
  tf_iter->transaction = &TX;

  tx = transfer_iterator_next(tf_iter);
  TEST_ASSERT_NOT_NULL(tx);
  TEST_ASSERT_EQUAL_MEMORY(SIG1, tx->data.signature_or_message, sizeof(SIG1));

  tx = transfer_iterator_next(tf_iter);
  TEST_ASSERT_NOT_NULL(tx);
  TEST_ASSERT_EQUAL_MEMORY(SIG2, tx->data.signature_or_message, sizeof(SIG2));

  tx = transfer_iterator_next(tf_iter);
  TEST_ASSERT_NOT_NULL(tx);
  TEST_ASSERT_EQUAL_MEMORY(SIG3, tx->data.signature_or_message, sizeof(SIG3));

  tx = transfer_iterator_next(tf_iter);
  TEST_ASSERT_NULL(tx);

  transfer_iterator_free(&tf_iter);
  transfer_free(&transfer);
  TEST_ASSERT_NULL(transfer);
}

void test_transfer_data(void) {
  iota_transaction_t tx;
  transfer_iterator_t* tf_iter = NULL;
  transfer_t* transfer = NULL;

  flex_trit_t data[FLEX_TRIT_SIZE_243] = {};

  flex_trits_from_trytes(data, NUM_TRITS_ADDRESS, TEST_DATA, NUM_TRYTES_ADDRESS,
                         NUM_TRYTES_ADDRESS);

  transfer =
      transfer_data_new(ADDRESS, TAG, data, NUM_TRITS_ADDRESS, 1509136296);

  transfer_t* transfers[1] = {transfer};
  Kerl kerl = {};
  init_kerl(&kerl);
  tf_iter = transfer_iterator_new(transfers, 1, &kerl, NULL);
  TEST_ASSERT_NOT_NULL(tf_iter);

  tx = transfer_iterator_next(tf_iter);
  TEST_ASSERT_NOT_NULL(tx);
  TEST_ASSERT_EQUAL_MEMORY(data, tx->data.signature_or_message,
                           FLEX_TRIT_SIZE_243);

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
