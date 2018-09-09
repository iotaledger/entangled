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

struct _transfer_output OUTPUT = {SEED, 3ULL, 5ULL};

struct _iota_transaction TX;

void test_bundle_hash(void) {
  transfer_t transfer = transfer_new(VALUE_IN);
  transfer_value_in_t value_in = transfer_value_in(transfer);
  transfer_value_in_set_address(value_in, (const tryte_t *)ADDRESS);
  transfer_set_tag(transfer, (tryte_t *)TAG);
  transfer_set_timestamp(transfer, 1509136296);

  transfer_t transfers[1] = {transfer};
  Kerl kerl = {};
  init_kerl(&kerl);
  transfer_ctx_t transfer_ctx = transfer_ctx_new();
  transfer_ctx_init(transfer_ctx, transfers, 1);
  transfer_ctx_hash(transfer_ctx, &kerl, transfers, 1);
  tryte_t *bundle_hash = transfer_ctx_finalize(transfer_ctx);
  TEST_ASSERT_EQUAL_MEMORY(HASH, bundle_hash, sizeof(HASH));
  transfer_ctx_free(transfer_ctx);
  transfer_free(transfer);
}

void test_iterator(void) {
  transfer_t transfer = transfer_new(VALUE_OUT);
  transfer_value_out_t value_out = transfer_value_out(transfer);
  transfer_value_out_set_output(value_out, &OUTPUT);
  transfer_value_out_set_address(value_out, (const tryte_t *)ADDRESS);
  transfer_set_tag(transfer, (tryte_t *)TAG);
  transfer_set_timestamp(transfer, 1509136296);

  transfer_t transfers[1] = {transfer};
  Kerl kerl = {};
  init_kerl(&kerl);
  transfer_iterator_t transfer_iterator =
      transfer_iterator_new(transfers, 1, &kerl);
  transfer_iterator_set_transaction(transfer_iterator, &TX);
  iota_transaction_t transaction;
  transaction = transfer_iterator_next(transfer_iterator);
  TEST_ASSERT_NOT_NULL(transaction);
  TEST_ASSERT_EQUAL_MEMORY(SIG1, transaction_signature(transaction),
                           sizeof(SIG1));
  transaction = transfer_iterator_next(transfer_iterator);
  TEST_ASSERT_NOT_NULL(transaction);
  TEST_ASSERT_EQUAL_MEMORY(SIG2, transaction_signature(transaction),
                           sizeof(SIG2));
  transaction = transfer_iterator_next(transfer_iterator);
  TEST_ASSERT_NOT_NULL(transaction);
  TEST_ASSERT_EQUAL_MEMORY(SIG3, transaction_signature(transaction),
                           sizeof(SIG3));
  transaction = transfer_iterator_next(transfer_iterator);
  TEST_ASSERT_NULL(transaction);
  transfer_iterator_free(transfer_iterator);
  transfer_free(transfer);
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_bundle_hash);
  RUN_TEST(test_iterator);

  return UNITY_END();
}
