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

void test_deserialize_and_serialize(void) {
  flex_trit_t trits[FLEX_TRIT_SIZE_8019];
  flex_trits_from_trytes(trits, NUM_TRITS_SERIALIZED_TRANSACTION,
                         (tryte_t *)TRYTES, NUM_TRITS_SERIALIZED_TRANSACTION,
                         NUM_TRYTES_SERIALIZED_TRANSACTION);
  iota_transaction_t *transaction = transaction_deserialize(trits, true);
  flex_trit_t *serialized_value = transaction_serialize(transaction);
  TEST_ASSERT_EQUAL_MEMORY(trits, serialized_value, sizeof(trits));
  transaction_free(transaction);
  free(serialized_value);
}

void test_deserialize_and_serialize_allocated(void) {
  flex_trit_t trits[FLEX_TRIT_SIZE_8019];
  flex_trits_from_trytes(trits, NUM_TRITS_SERIALIZED_TRANSACTION,
                         (tryte_t *)TRYTES, NUM_TRITS_SERIALIZED_TRANSACTION,
                         NUM_TRYTES_SERIALIZED_TRANSACTION);
  iota_transaction_t transaction;
  transaction_deserialize_from_trits(&transaction, trits, true);
  flex_trit_t serialized_value[FLEX_TRIT_SIZE_8019];
  transaction_serialize_on_flex_trits(&transaction, serialized_value);
  TEST_ASSERT_EQUAL_MEMORY(trits, serialized_value, FLEX_TRIT_SIZE_8019);
}

void test_reset(void) {
  iota_transaction_t *transaction = transaction_new();
  transaction_reset(transaction);
  TEST_ASSERT_EQUAL_INT8(FLEX_TRIT_NULL_VALUE, transaction->essence.address[0]);
  flex_trit_t obsolete_tag[sizeof(transaction->essence.obsolete_tag)];
  memset(obsolete_tag, FLEX_TRIT_NULL_VALUE,
         sizeof(transaction->essence.obsolete_tag));
  TEST_ASSERT_EQUAL_MEMORY(obsolete_tag, transaction->essence.obsolete_tag,
                           sizeof(transaction->essence.obsolete_tag));
  transaction_free(transaction);
}

void test_spam_transaction(void) {
  iota_transaction_t transaction = transaction_new_spam();
  TEST_ASSERT_EQUAL_MEMORY(TRITS, transaction->signature_or_message,
                           sizeof(transaction->signature_or_message));
  TEST_ASSERT_EQUAL_MEMORY(TRITS, transaction->address,
                           sizeof(transaction->address));
  TEST_ASSERT_EQUAL_MEMORY(TRITS, transaction->tag,
                           sizeof(transaction->tag));
  transaction_free(transaction);
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_deserialize_and_serialize);
  RUN_TEST(test_deserialize_and_serialize_allocated);
  RUN_TEST(test_reset);
  RUN_TEST(test_spam_transaction);

  return UNITY_END();
}
