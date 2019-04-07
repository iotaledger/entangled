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
  flex_trits_from_trytes(trits, NUM_TRITS_SERIALIZED_TRANSACTION, (tryte_t *)TRYTES, NUM_TRITS_SERIALIZED_TRANSACTION,
                         NUM_TRYTES_SERIALIZED_TRANSACTION);
  iota_transaction_t *transaction = transaction_deserialize(trits, true);
  flex_trit_t *serialized_value = transaction_serialize(transaction);
  TEST_ASSERT_EQUAL_MEMORY(trits, serialized_value, sizeof(trits));
  transaction_free(transaction);
  free(serialized_value);
}

void test_deserialize_and_serialize_allocated(void) {
  flex_trit_t trits[FLEX_TRIT_SIZE_8019];
  flex_trits_from_trytes(trits, NUM_TRITS_SERIALIZED_TRANSACTION, (tryte_t *)TRYTES, NUM_TRITS_SERIALIZED_TRANSACTION,
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
  memset(obsolete_tag, FLEX_TRIT_NULL_VALUE, sizeof(transaction->essence.obsolete_tag));
  TEST_ASSERT_EQUAL_MEMORY(obsolete_tag, transaction->essence.obsolete_tag, sizeof(transaction->essence.obsolete_tag));
  transaction_free(transaction);
}

void test_read_write_transaction_obj(void) {
  flex_trit_t TEST_FLEX_TRIT_ADDRESS[FLEX_TRIT_SIZE_243];
  flex_trit_t TEST_FLEX_TRIT_OBSOLETE_TAG[FLEX_TRIT_SIZE_81];
  flex_trit_t TEST_FLEX_TRIT_BUNDLE[FLEX_TRIT_SIZE_243];
  flex_trit_t TEST_FLEX_TRIT_TRUNK[FLEX_TRIT_SIZE_243];
  flex_trit_t TEST_FLEX_TRIT_BRANCH[FLEX_TRIT_SIZE_243];
  flex_trit_t TEST_FLEX_TRIT_NONCE[FLEX_TRIT_SIZE_81];
  flex_trit_t TEST_FLEX_TRIT_TAG[FLEX_TRIT_SIZE_81];
  flex_trit_t TEST_FLEX_TRIT_HASH[FLEX_TRIT_SIZE_243];
  flex_trit_t TEST_FLEX_TRIT_SIGNATURE[FLEX_TRIT_SIZE_6561];
  flex_trit_t TEST_FLEX_TRIT_MESSAGE[FLEX_TRIT_SIZE_6561];

  iota_transaction_t transaction;

  transaction_reset(&transaction);

  // write transaction values

  // Essence
  flex_trits_from_trytes(TEST_FLEX_TRIT_ADDRESS, NUM_TRITS_ADDRESS, TEST_ADDRESS_0, NUM_TRYTES_ADDRESS,
                         NUM_TRYTES_ADDRESS);
  transaction_set_address(&transaction, TEST_FLEX_TRIT_ADDRESS);
  TEST_ASSERT_TRUE(transaction.loaded_columns_mask.essence & MASK_ESSENCE_ADDRESS);
  transaction_set_value(&transaction, TEST_VALUE);
  TEST_ASSERT_TRUE(transaction.loaded_columns_mask.essence & MASK_ESSENCE_VALUE);
  flex_trits_from_trytes(TEST_FLEX_TRIT_OBSOLETE_TAG, NUM_TRITS_OBSOLETE_TAG, TEST_OBSOLETE_TAG,
                         NUM_TRYTES_OBSOLETE_TAG, NUM_TRYTES_OBSOLETE_TAG);
  transaction_set_obsolete_tag(&transaction, TEST_FLEX_TRIT_OBSOLETE_TAG);
  TEST_ASSERT_TRUE(transaction.loaded_columns_mask.essence & MASK_ESSENCE_OBSOLETE_TAG);
  transaction_set_timestamp(&transaction, TEST_TIMESTAMP);
  TEST_ASSERT_TRUE(transaction.loaded_columns_mask.essence & MASK_ESSENCE_TIMESTAMP);
  transaction_set_current_index(&transaction, TEST_CURRENT_INDEX);
  TEST_ASSERT_TRUE(transaction.loaded_columns_mask.essence & MASK_ESSENCE_CURRENT_INDEX);
  transaction_set_last_index(&transaction, TEST_LAST_INDEX);
  TEST_ASSERT_TRUE(transaction.loaded_columns_mask.essence & MASK_ESSENCE_LAST_INDEX);
  flex_trits_from_trytes(TEST_FLEX_TRIT_BUNDLE, NUM_TRITS_BUNDLE, TEST_BUNDLE, NUM_TRYTES_BUNDLE, NUM_TRYTES_BUNDLE);
  transaction_set_bundle(&transaction, TEST_FLEX_TRIT_BUNDLE);
  TEST_ASSERT_TRUE(transaction.loaded_columns_mask.essence & MASK_ESSENCE_BUNDLE);

  // Attachment
  flex_trits_from_trytes(TEST_FLEX_TRIT_TRUNK, NUM_TRITS_TRUNK, TEST_TRUNK, NUM_TRYTES_TRUNK, NUM_TRYTES_TRUNK);
  transaction_set_trunk(&transaction, TEST_FLEX_TRIT_TRUNK);
  TEST_ASSERT_TRUE(transaction.loaded_columns_mask.attachment & MASK_ATTACHMENT_TRUNK);
  flex_trits_from_trytes(TEST_FLEX_TRIT_BRANCH, NUM_TRITS_BRANCH, TEST_BRANCH, NUM_TRYTES_BRANCH, NUM_TRYTES_BRANCH);
  transaction_set_branch(&transaction, TEST_FLEX_TRIT_BRANCH);
  TEST_ASSERT_TRUE(transaction.loaded_columns_mask.attachment & MASK_ATTACHMENT_BRANCH);
  transaction_set_attachment_timestamp(&transaction, TEST_ATTACHMENT_TIMESTAMP);
  TEST_ASSERT_TRUE(transaction.loaded_columns_mask.attachment & MASK_ATTACHMENT_TIMESTAMP);
  transaction_set_attachment_timestamp_lower(&transaction, TEST_ATTACHMENT_TIMESTAMP_LOWER);
  TEST_ASSERT_TRUE(transaction.loaded_columns_mask.attachment & MASK_ATTACHMENT_TIMESTAMP_LOWER);
  transaction_set_attachment_timestamp_upper(&transaction, TEST_ATTACHMENT_TIMESTAMP_UPPER);
  TEST_ASSERT_TRUE(transaction.loaded_columns_mask.attachment & MASK_ATTACHMENT_TIMESTAMP_UPPER);
  flex_trits_from_trytes(TEST_FLEX_TRIT_NONCE, NUM_TRITS_NONCE, TEST_NONCE, NUM_TRYTES_NONCE, NUM_TRYTES_NONCE);
  transaction_set_nonce(&transaction, TEST_FLEX_TRIT_NONCE);
  TEST_ASSERT_TRUE(transaction.loaded_columns_mask.attachment & MASK_ATTACHMENT_NONCE);
  flex_trits_from_trytes(TEST_FLEX_TRIT_TAG, NUM_TRITS_TAG, TEST_TAG_NULL, NUM_TRYTES_TAG, NUM_TRYTES_TAG);
  transaction_set_tag(&transaction, TEST_FLEX_TRIT_TAG);
  TEST_ASSERT_TRUE(transaction.loaded_columns_mask.attachment & MASK_ATTACHMENT_TAG);

  // Consensus
  flex_trits_from_trytes(TEST_FLEX_TRIT_HASH, NUM_TRITS_HASH, TEST_HASH, NUM_TRYTES_HASH, NUM_TRYTES_HASH);
  transaction_set_hash(&transaction, TEST_FLEX_TRIT_HASH);
  TEST_ASSERT_TRUE(transaction.loaded_columns_mask.consensus & MASK_CONSENSUS_HASH);

  // Data
  flex_trits_from_trytes(TEST_FLEX_TRIT_SIGNATURE, NUM_TRITS_SIGNATURE, TEST_SIG_1, NUM_TRYTES_SIGNATURE,
                         NUM_TRYTES_SIGNATURE);
  transaction_set_signature(&transaction, TEST_FLEX_TRIT_SIGNATURE);
  TEST_ASSERT_TRUE(transaction.loaded_columns_mask.data & MASK_DATA_SIG_OR_MSG);

  // Metadata
  transaction_set_snapshot_index(&transaction, TEST_SNAPSHOT_INDEX);
  TEST_ASSERT_TRUE(transaction.loaded_columns_mask.metadata & MASK_METADATA_SNAPSHOT_INDEX);
  transaction_set_solid(&transaction, TEST_SOLID);
  TEST_ASSERT_TRUE(transaction.loaded_columns_mask.metadata & MASK_METADATA_SOLID);
  transaction_set_arrival_timestamp(&transaction, TEST_ARRIVAL_TIMESTAMP);
  TEST_ASSERT_TRUE(transaction.loaded_columns_mask.metadata & MASK_METADATA_ARRIVAL_TIMESTAMP);

  // read transaction values

  // Essence
  TEST_ASSERT_EQUAL_MEMORY(transaction_address(&transaction), TEST_FLEX_TRIT_ADDRESS, sizeof(TEST_FLEX_TRIT_ADDRESS));
  TEST_ASSERT_EQUAL_INT64(transaction_value(&transaction), TEST_VALUE);
  TEST_ASSERT_EQUAL_MEMORY(transaction_obsolete_tag(&transaction), TEST_FLEX_TRIT_OBSOLETE_TAG,
                           sizeof(TEST_FLEX_TRIT_OBSOLETE_TAG));
  TEST_ASSERT_EQUAL_INT64(transaction_timestamp(&transaction), TEST_TIMESTAMP);
  TEST_ASSERT_EQUAL_INT64(transaction_current_index(&transaction), TEST_CURRENT_INDEX);
  TEST_ASSERT_EQUAL_INT64(transaction_last_index(&transaction), TEST_LAST_INDEX);
  TEST_ASSERT_EQUAL_MEMORY(transaction_bundle(&transaction), TEST_FLEX_TRIT_BUNDLE, sizeof(TEST_FLEX_TRIT_BUNDLE));

  // Attachment
  TEST_ASSERT_EQUAL_MEMORY(transaction_trunk(&transaction), TEST_FLEX_TRIT_TRUNK, sizeof(TEST_FLEX_TRIT_TRUNK));
  TEST_ASSERT_EQUAL_MEMORY(transaction_branch(&transaction), TEST_FLEX_TRIT_BRANCH, sizeof(TEST_FLEX_TRIT_BRANCH));
  TEST_ASSERT_EQUAL_INT64(transaction_attachment_timestamp(&transaction), TEST_ATTACHMENT_TIMESTAMP);
  TEST_ASSERT_EQUAL_INT64(transaction_attachment_timestamp_lower(&transaction), TEST_ATTACHMENT_TIMESTAMP_LOWER);
  TEST_ASSERT_EQUAL_INT64(transaction_attachment_timestamp_upper(&transaction), TEST_ATTACHMENT_TIMESTAMP_UPPER);
  TEST_ASSERT_EQUAL_MEMORY(transaction_nonce(&transaction), TEST_FLEX_TRIT_NONCE, sizeof(TEST_FLEX_TRIT_NONCE));
  TEST_ASSERT_EQUAL_MEMORY(transaction_tag(&transaction), TEST_FLEX_TRIT_TAG, sizeof(TEST_FLEX_TRIT_TAG));

  // Consensus
  TEST_ASSERT_EQUAL_MEMORY(transaction_hash(&transaction), TEST_FLEX_TRIT_HASH, sizeof(TEST_FLEX_TRIT_HASH));
  TEST_ASSERT_EQUAL_MEMORY(transaction_signature(&transaction), TEST_FLEX_TRIT_SIGNATURE,
                           sizeof(TEST_FLEX_TRIT_SIGNATURE));

  // Data
  // test transaction_set_message() individually
  transaction.loaded_columns_mask.data ^= MASK_DATA_SIG_OR_MSG;
  flex_trits_from_trytes(TEST_FLEX_TRIT_MESSAGE, NUM_TRITS_MESSAGE, TEST_MSG, NUM_TRYTES_MESSAGE, NUM_TRYTES_MESSAGE);
  transaction_set_message(&transaction, TEST_FLEX_TRIT_MESSAGE);
  TEST_ASSERT_TRUE(transaction.loaded_columns_mask.data & MASK_DATA_SIG_OR_MSG);
  TEST_ASSERT_EQUAL_MEMORY(transaction_signature(&transaction), TEST_FLEX_TRIT_MESSAGE, sizeof(TEST_FLEX_TRIT_MESSAGE));

  // Metadata
  TEST_ASSERT_EQUAL_INT64(transaction_snapshot_index(&transaction), TEST_SNAPSHOT_INDEX);
  TEST_ASSERT_EQUAL_INT8(transaction_solid(&transaction), TEST_SOLID);
  TEST_ASSERT_EQUAL_INT64(transaction_arrival_timestamp(&transaction), TEST_ARRIVAL_TIMESTAMP);
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_deserialize_and_serialize);
  RUN_TEST(test_deserialize_and_serialize_allocated);
  RUN_TEST(test_reset);
  RUN_TEST(test_read_write_transaction_obj);

  return UNITY_END();
}
