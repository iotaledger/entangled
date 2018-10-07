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
#include "common/model/transaction.h"
#include "common/trinary/tryte.h"

void test_serialize(void) {
  flex_trit_t *serialized_value =
      transaction_serialize((const iota_transaction_t)&TRANSACTION);
  TEST_ASSERT_EQUAL_MEMORY(TRITS, serialized_value, sizeof(TRITS));
  free(serialized_value);
}

void test_deserialize(void) {
  iota_transaction_t transaction =
      transaction_deserialize((const flex_trit_t *)&TRITS);
  TEST_ASSERT_EQUAL_MEMORY(&TRANSACTION, transaction, sizeof(TRANSACTION));
  transaction_free(transaction);
}

void test_serialize_allocated(void) {
  flex_trit_t serialized_value[sizeof(TRITS)];
  transaction_serialize_on_flex_trits((const iota_transaction_t)&TRANSACTION,
                                      serialized_value);
  TEST_ASSERT_EQUAL_MEMORY(TRITS, serialized_value, sizeof(TRITS));
}

void test_deserialize_allocated(void) {
  struct _iota_transaction transaction = {};
  transaction_deserialize_from_trits(&transaction, (const flex_trit_t *)&TRITS);
  TEST_ASSERT_EQUAL_MEMORY(&TRANSACTION, &transaction, sizeof(TRANSACTION));
}

void test_reset(void) {
  iota_transaction_t transaction = transaction_new();
  transaction_reset(transaction);
  TEST_ASSERT_EQUAL_INT8(FLEX_TRIT_NULL_VALUE, transaction->address[0]);
  flex_trit_t obsolete_tag[sizeof(transaction->obsolete_tag)];
  memset(obsolete_tag, FLEX_TRIT_NULL_VALUE, sizeof(transaction->obsolete_tag));
  TEST_ASSERT_EQUAL_MEMORY(obsolete_tag, transaction->obsolete_tag,
                           sizeof(transaction->obsolete_tag));
  transaction_free(transaction);
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_serialize);
  RUN_TEST(test_deserialize);
  RUN_TEST(test_serialize_allocated);
  RUN_TEST(test_deserialize_allocated);
  RUN_TEST(test_reset);

  return UNITY_END();
}
