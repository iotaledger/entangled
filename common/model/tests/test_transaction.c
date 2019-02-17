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

void test_read_write_transaction_obj(void) {
  flex_trit_t test_address[FLEX_TRIT_SIZE_243];
  int64_t test_value = 123456;
  flex_trit_t test_obsolete_tag[FLEX_TRIT_SIZE_81];
  uint64_t test_timestamp = 1477037811737;
  int64_t test_current_index = 2;
  int64_t test_last_index = 5;
  flex_trit_t test_bundle[FLEX_TRIT_SIZE_243];

  flex_trit_t test_trunk[FLEX_TRIT_SIZE_243];
  flex_trit_t test_branch[FLEX_TRIT_SIZE_243];
  uint64_t test_attachment_timestamp = 1477037821737;
  uint64_t test_attachment_timestamp_lower = 1477037831737;
  uint64_t test_attachment_timestamp_upper = 1477037841737;
  flex_trit_t test_nonce[FLEX_TRIT_SIZE_81];
  flex_trit_t test_tag[FLEX_TRIT_SIZE_81];

  flex_trit_t test_hash[FLEX_TRIT_SIZE_243];
  flex_trit_t test_signature_or_message[FLEX_TRIT_SIZE_6561];

  uint64_t test_snapshot_index = 2345;
  bool test_solid = true;
  uint64_t test_arrival_timestamp = 1477037812737;

  iota_transaction_t transaction;

  // write transaction values
  flex_trits_from_trytes(test_address, FLEX_TRIT_SIZE_243, TEST_ADDRESS_0,
                         NUM_TRYTES_ADDRESS, sizeof(TEST_ADDRESS_0));
  transaction_set_address(&transaction, test_address);
  transaction_set_value(&transaction, test_value);
  flex_trits_from_trytes(test_obsolete_tag, FLEX_TRIT_SIZE_81,
                         TEST_OBSOLETE_TAG, NUM_TRYTES_OBSOLETE_TAG,
                         sizeof(TEST_OBSOLETE_TAG));
  transaction_set_obsolete_tag(&transaction, test_obsolete_tag);
  transaction_set_timestamp(&transaction, test_timestamp);
  transaction_set_current_index(&transaction, test_current_index);
  transaction_set_last_index(&transaction, test_last_index);
  flex_trits_from_trytes(test_bundle, FLEX_TRIT_SIZE_243, TEST_BUNDLE,
                         NUM_TRYTES_BUNDLE, sizeof(TEST_BUNDLE));
  transaction_set_bundle(&transaction, test_bundle);

  flex_trits_from_trytes(test_trunk, FLEX_TRIT_SIZE_243, TEST_TRUNK,
                         NUM_TRYTES_TRUNK, sizeof(TEST_TRUNK));
  transaction_set_trunk(&transaction, test_trunk);
  flex_trits_from_trytes(test_branch, FLEX_TRIT_SIZE_243, TEST_BRANCH,
                         NUM_TRYTES_BRANCH, sizeof(TEST_BRANCH));
  transaction_set_branch(&transaction, test_branch);
  transaction_set_attachment_timestamp(&transaction, test_attachment_timestamp);
  transaction_set_attachment_timestamp_lower(&transaction,
                                             test_attachment_timestamp_lower);
  transaction_set_attachment_timestamp_upper(&transaction,
                                             test_attachment_timestamp_upper);
  flex_trits_from_trytes(test_nonce, FLEX_TRIT_SIZE_81, TEST_NONCE,
                         NUM_TRYTES_NONCE, sizeof(TEST_NONCE));
  transaction_set_nonce(&transaction, test_nonce);
  flex_trits_from_trytes(test_tag, FLEX_TRIT_SIZE_81, TEST_TAG_NULL,
                         NUM_TRYTES_TAG, sizeof(TEST_TAG_NULL));
  transaction_set_tag(&transaction, test_tag);

  flex_trits_from_trytes(test_hash, FLEX_TRIT_SIZE_243, TEST_HASH,
                         NUM_TRYTES_HASH, sizeof(TEST_HASH));
  transaction_set_hash(&transaction, test_hash);
  flex_trits_from_trytes(test_signature_or_message, FLEX_TRIT_SIZE_6561,
                         TEST_SIG_1, NUM_TRYTES_SIGNATURE, sizeof(TEST_SIG_1));
  transaction_set_signature(&transaction, test_signature_or_message);

  transaction_set_snapshot_index(&transaction, test_snapshot_index);
  transaction_set_solid(&transaction, test_solid);
  transaction_set_arrival_timestamp(&transaction, test_arrival_timestamp);

  // read transaction values
  TEST_ASSERT_EQUAL_MEMORY(transaction_address(&transaction), test_address,
                           sizeof(test_address));
  TEST_ASSERT_EQUAL_INT(transaction_value(&transaction), test_value);
  TEST_ASSERT_EQUAL_MEMORY(transaction_obsolete_tag(&transaction),
                           test_obsolete_tag, sizeof(test_obsolete_tag));
  TEST_ASSERT_EQUAL_INT(transaction_timestamp(&transaction), test_timestamp);
  TEST_ASSERT_EQUAL_INT(transaction_current_index(&transaction),
                        test_current_index);
  TEST_ASSERT_EQUAL_INT(transaction_last_index(&transaction), test_last_index);
  TEST_ASSERT_EQUAL_MEMORY(transaction_bundle(&transaction), test_bundle,
                           sizeof(test_bundle));

  TEST_ASSERT_EQUAL_MEMORY(transaction_trunk(&transaction), test_trunk,
                           sizeof(test_trunk));
  TEST_ASSERT_EQUAL_MEMORY(transaction_branch(&transaction), test_branch,
                           sizeof(test_branch));
  TEST_ASSERT_EQUAL_INT(transaction_attachment_timestamp(&transaction),
                        test_attachment_timestamp);
  TEST_ASSERT_EQUAL_INT(transaction_attachment_timestamp_lower(&transaction),
                        test_attachment_timestamp_lower);
  TEST_ASSERT_EQUAL_INT(transaction_attachment_timestamp_upper(&transaction),
                        test_attachment_timestamp_upper);
  TEST_ASSERT_EQUAL_MEMORY(transaction_nonce(&transaction), test_nonce,
                           sizeof(test_nonce));
  TEST_ASSERT_EQUAL_MEMORY(transaction_tag(&transaction), test_tag,
                           sizeof(test_tag));

  TEST_ASSERT_EQUAL_MEMORY(transaction_hash(&transaction), test_hash,
                           sizeof(test_hash));
  TEST_ASSERT_EQUAL_MEMORY(transaction_signature(&transaction),
                           test_signature_or_message,
                           sizeof(test_signature_or_message));

  TEST_ASSERT_EQUAL_INT(transaction_snapshot_index(&transaction),
                        test_snapshot_index);
  TEST_ASSERT_EQUAL_INT8(transaction_solid(&transaction), test_solid);
  TEST_ASSERT_EQUAL_INT(transaction_arrival_timestamp(&transaction),
                        test_arrival_timestamp);
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_deserialize_and_serialize);
  RUN_TEST(test_deserialize_and_serialize_allocated);
  RUN_TEST(test_reset);
  RUN_TEST(test_read_write_transaction_obj);

  return UNITY_END();
}
