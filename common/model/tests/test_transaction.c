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

void test_read_transaction_obj(void) {
  int64_t test_value = 123456;
  uint64_t test_timestamp = 1477037811737;
  uint64_t test_attachment_timestamp = 1477037821737;
  int64_t test_current_index = 2;
  int64_t test_last_index = 5;
  uint64_t test_attachment_timestamp_lower = 1477037831737;
  uint64_t test_attachment_timestamp_upper = 1477037841737;
  uint64_t test_snapshot_index = 2345;
  bool test_solid = true;
  uint64_t test_arrival_timestamp = 1477037812737;
  iota_transaction_t *transaction = transaction_new();

  // write transaction values
  transaction_set_address(transaction, TEST_ADDRESS_0);
  transaction_set_value(transaction, test_value);
  transaction_set_obsolete_tag(transaction, TEST_OBSOLETE_TAG);
  transaction_set_timestamp(transaction, test_timestamp);
  transaction_set_current_index(transaction, test_current_index);
  transaction_set_last_index(transaction, test_last_index);
  transaction_set_bundle(transaction, TEST_BUNDLE);

  transaction_set_trunk(transaction, TEST_TRUNK);
  transaction_set_branch(transaction, TEST_BRANCH);
  transaction_set_attachment_timestamp(transaction, test_attachment_timestamp);
  transaction_set_attachment_timestamp_lower(transaction,
                                             test_attachment_timestamp_lower);
  transaction_set_attachment_timestamp_upper(transaction,
                                             test_attachment_timestamp_upper);
  transaction_set_nonce(transaction, TEST_NONCE);
  transaction_set_tag(transaction, TEST_TAG_NULL);

  transaction_set_hash(transaction, TEST_HASH);
  transaction_set_signature(transaction, TEST_SIG_1);

  transaction_set_snapshot_index(transaction, test_snapshot_index);
  transaction_set_solid(transaction, test_solid);
  transaction_set_arrival_timestamp(transaction, test_arrival_timestamp);

  // read transaction values
  TEST_ASSERT_EQUAL_MEMORY(transaction_address(transaction), TEST_ADDRESS_0,
                           NUM_TRYTES_ADDRESS);
  TEST_ASSERT_EQUAL_INT(transaction_value(transaction), test_value);
  TEST_ASSERT_EQUAL_MEMORY(transaction_obsolete_tag(transaction),
                           TEST_OBSOLETE_TAG, NUM_TRYTES_OBSOLETE_TAG);
  TEST_ASSERT_EQUAL_INT(transaction_timestamp(transaction), test_timestamp);
  TEST_ASSERT_EQUAL_INT(transaction_current_index(transaction),
                        test_current_index);
  TEST_ASSERT_EQUAL_INT(transaction_last_index(transaction), test_last_index);
  TEST_ASSERT_EQUAL_MEMORY(transaction_bundle(transaction), TEST_BUNDLE,
                           NUM_TRYTES_BUNDLE);

  TEST_ASSERT_EQUAL_MEMORY(transaction_trunk(transaction), TEST_TRUNK,
                           NUM_TRYTES_TRUNK);
  TEST_ASSERT_EQUAL_MEMORY(transaction_branch(transaction), TEST_BRANCH,
                           NUM_TRYTES_BRANCH);
  TEST_ASSERT_EQUAL_INT(transaction_attachment_timestamp(transaction),
                        test_attachment_timestamp);
  TEST_ASSERT_EQUAL_INT(transaction_attachment_timestamp_lower(transaction),
                        test_attachment_timestamp_lower);
  TEST_ASSERT_EQUAL_INT(transaction_attachment_timestamp_upper(transaction),
                        test_attachment_timestamp_upper);
  TEST_ASSERT_EQUAL_MEMORY(transaction_nonce(transaction), TEST_NONCE,
                           NUM_TRYTES_NONCE);
  TEST_ASSERT_EQUAL_MEMORY(transaction_tag(transaction), TEST_TAG_NULL,
                           NUM_TRYTES_TAG);

  TEST_ASSERT_EQUAL_MEMORY(transaction_hash(transaction), TEST_HASH,
                           NUM_TRYTES_HASH);
  TEST_ASSERT_EQUAL_MEMORY(transaction_signature(transaction), TEST_SIG_1,
                           NUM_TRYTES_SIGNATURE);

  TEST_ASSERT_EQUAL_INT(transaction_snapshot_index(transaction),
                        test_snapshot_index);
  TEST_ASSERT_EQUAL_INT8(transaction_solid(transaction), test_solid);
  TEST_ASSERT_EQUAL_INT(transaction_arrival_timestamp(transaction),
                        test_arrival_timestamp);

  transaction_free(transaction);
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_deserialize_and_serialize);
  RUN_TEST(test_deserialize_and_serialize_allocated);
  RUN_TEST(test_reset);
  RUN_TEST(test_read_transaction_obj);

  return UNITY_END();
}
