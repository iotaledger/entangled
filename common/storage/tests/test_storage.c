/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdlib.h>
#include <string.h>

#include <unity/unity.h>

#include "common/helpers/digest.h"
#include "common/model/milestone.h"
#include "common/model/transaction.h"
#include "common/storage/storage.h"
#include "common/storage/test_utils.h"
#include "common/storage/tests/defs.h"
#include "common/trinary/add.h"
#include "utils/containers/hash/hash243_set.h"
#include "utils/time.h"

static char* tangle_test_db_path = "common/storage/tests/test.db";

static storage_connection_config_t config;
static storage_connection_t connection;
static storage_connection_type_t connection_type;

void setUp(void) {
  TEST_ASSERT(storage_test_setup(&connection, &config, tangle_test_db_path, connection_type) == RC_OK);
}

void tearDown(void) { TEST_ASSERT(storage_test_teardown(&connection, tangle_test_db_path, connection_type) == RC_OK); }

static void store_test_transaction(iota_transaction_t* const transaction) {
  flex_trit_t transaction_trits[FLEX_TRIT_SIZE_8019];

  flex_trits_from_trytes(transaction_trits, NUM_TRITS_SERIALIZED_TRANSACTION, TEST_TX_TRYTES,
                         NUM_TRITS_SERIALIZED_TRANSACTION, NUM_TRYTES_SERIALIZED_TRANSACTION);
  transaction_deserialize_from_trits(transaction, transaction_trits, true);

  TEST_ASSERT(storage_transaction_store(&connection, transaction) == RC_OK);
}

static void store_test_milestone(iota_milestone_t* const milestone) {
  milestone->index = TEST_MS_INDEX;
  flex_trits_from_trytes(milestone->hash, NUM_TRITS_HASH, TEST_TX_HASH, NUM_TRITS_HASH, NUM_TRYTES_HASH);

  TEST_ASSERT(storage_milestone_store(&connection, milestone) == RC_OK);
}

static void test_connection_init_destroy(void) {
  // Empty because connection init/destroy are handled by setUp/tearDown
}

static void test_transaction_count(void) {
  uint64_t count = 0;
  trit_t hash[HASH_LENGTH_TRIT];
  flex_trit_t transaction_trits[FLEX_TRIT_SIZE_8019];
  iota_transaction_t transaction;

  flex_trits_from_trytes(transaction_trits, NUM_TRITS_SERIALIZED_TRANSACTION, TEST_TX_TRYTES,
                         NUM_TRITS_SERIALIZED_TRANSACTION, NUM_TRYTES_SERIALIZED_TRANSACTION);
  transaction_deserialize_from_trits(&transaction, transaction_trits, true);

  TEST_ASSERT(storage_transaction_count(&connection, &count) == RC_OK);
  TEST_ASSERT_EQUAL_INT(count, 0);

  flex_trits_to_trits(hash, HASH_LENGTH_TRIT, transaction_hash(&transaction), HASH_LENGTH_TRIT, HASH_LENGTH_TRIT);

  for (size_t i = 0; i < 10; i++) {
    flex_trits_from_trits(transaction_hash(&transaction), HASH_LENGTH_TRIT, hash, HASH_LENGTH_TRIT, HASH_LENGTH_TRIT);
    TEST_ASSERT(storage_transaction_store(&connection, &transaction) == RC_OK);
    TEST_ASSERT(storage_transaction_count(&connection, &count) == RC_OK);
    TEST_ASSERT_EQUAL_INT(count, i + 1);
    add_assign(hash, HASH_LENGTH_TRIT, 1);
  }
}

static void test_transaction_store(void) {
  iota_transaction_t transaction;

  store_test_transaction(&transaction);
}

static void test_transaction_store_duplicate(void) {
  iota_transaction_t transaction;

  store_test_transaction(&transaction);
  TEST_ASSERT(storage_transaction_store(&connection, &transaction) != RC_OK);
}

static void test_transaction_load_not_found(void) {
  DECLARE_PACK_SINGLE_TX(loaded_transaction, ptr, pack);

  TEST_ASSERT(storage_transaction_load(&connection, TRANSACTION_FIELD_HASH, TEST_TX_HASH, &pack) == RC_OK);
  TEST_ASSERT_EQUAL_INT(pack.num_loaded, 0);
  TEST_ASSERT_FALSE(pack.insufficient_capacity);
}

static void test_transaction_load_found(void) {
  iota_transaction_t transaction;
  DECLARE_PACK_SINGLE_TX(loaded_transaction, ptr, pack);

  store_test_transaction(&transaction);

  TEST_ASSERT(storage_transaction_load(&connection, TRANSACTION_FIELD_HASH, TEST_TX_HASH, &pack) == RC_OK);
  TEST_ASSERT_EQUAL_INT(pack.num_loaded, 1);
  TEST_ASSERT_FALSE(pack.insufficient_capacity);

  TEST_ASSERT_EQUAL_MEMORY(transaction_address(ptr), transaction_address(&transaction), NUM_FLEX_TRITS_ADDRESS);
  TEST_ASSERT_EQUAL_INT(transaction_value(ptr), transaction_value(&transaction));
  TEST_ASSERT_EQUAL_MEMORY(transaction_obsolete_tag(ptr), transaction_obsolete_tag(&transaction),
                           NUM_FLEX_TRITS_OBSOLETE_TAG);
  TEST_ASSERT_EQUAL_INT(transaction_timestamp(ptr), transaction_timestamp(&transaction));
  TEST_ASSERT_EQUAL_INT(transaction_current_index(ptr), transaction_current_index(&transaction));
  TEST_ASSERT_EQUAL_INT(transaction_last_index(ptr), transaction_last_index(&transaction));
  TEST_ASSERT_EQUAL_MEMORY(transaction_bundle(ptr), transaction_bundle(&transaction), NUM_FLEX_TRITS_BUNDLE);
  TEST_ASSERT_EQUAL_MEMORY(transaction_trunk(ptr), transaction_trunk(&transaction), NUM_FLEX_TRITS_TRUNK);
  TEST_ASSERT_EQUAL_MEMORY(transaction_branch(ptr), transaction_branch(&transaction), NUM_FLEX_TRITS_BRANCH);
  TEST_ASSERT_EQUAL_INT(transaction_attachment_timestamp(ptr), transaction_attachment_timestamp(&transaction));
  TEST_ASSERT_EQUAL_INT(transaction_attachment_timestamp_lower(ptr),
                        transaction_attachment_timestamp_lower(&transaction));
  TEST_ASSERT_EQUAL_INT(transaction_attachment_timestamp_upper(ptr),
                        transaction_attachment_timestamp_upper(&transaction));
  TEST_ASSERT_EQUAL_MEMORY(transaction_nonce(ptr), transaction_nonce(&transaction), NUM_FLEX_TRITS_NONCE);
  TEST_ASSERT_EQUAL_MEMORY(transaction_tag(ptr), transaction_tag(&transaction), NUM_FLEX_TRITS_TAG);
  TEST_ASSERT_EQUAL_MEMORY(transaction_hash(ptr), transaction_hash(&transaction), NUM_FLEX_TRITS_HASH);
  TEST_ASSERT_EQUAL_MEMORY(transaction_signature(ptr), transaction_signature(&transaction), NUM_FLEX_TRITS_SIGNATURE);

  TEST_ASSERT_EQUAL_INT(ptr->loaded_columns_mask.metadata & MASK_METADATA_ALL, 0);
}

static void test_transaction_load_essence_metadata(void) {
  iota_transaction_t transaction;
  DECLARE_PACK_SINGLE_TX(loaded_transaction, ptr, pack);

  store_test_transaction(&transaction);

  TEST_ASSERT(storage_transaction_update_snapshot_index(&connection, TEST_TX_HASH, 42) == RC_OK);
  TEST_ASSERT(storage_transaction_update_solidity(&connection, TEST_TX_HASH, 1) == RC_OK);
  TEST_ASSERT(storage_transaction_update_validity(&connection, TEST_TX_HASH, 5) == RC_OK);

  TEST_ASSERT(storage_transaction_load_essence_metadata(&connection, TEST_TX_HASH, &pack) == RC_OK);
  TEST_ASSERT_EQUAL_INT(pack.num_loaded, 1);
  TEST_ASSERT_FALSE(pack.insufficient_capacity);

  TEST_ASSERT_EQUAL_MEMORY(transaction_address(ptr), transaction_address(&transaction), NUM_FLEX_TRITS_ADDRESS);
  TEST_ASSERT_EQUAL_INT(transaction_value(ptr), transaction_value(&transaction));
  TEST_ASSERT_EQUAL_MEMORY(transaction_obsolete_tag(ptr), transaction_obsolete_tag(&transaction),
                           NUM_FLEX_TRITS_OBSOLETE_TAG);
  TEST_ASSERT_EQUAL_INT(transaction_timestamp(ptr), transaction_timestamp(&transaction));
  TEST_ASSERT_EQUAL_INT(transaction_current_index(ptr), transaction_current_index(&transaction));
  TEST_ASSERT_EQUAL_INT(transaction_last_index(ptr), transaction_last_index(&transaction));
  TEST_ASSERT_EQUAL_MEMORY(transaction_bundle(ptr), transaction_bundle(&transaction), NUM_FLEX_TRITS_BUNDLE);
  TEST_ASSERT_EQUAL_INT(transaction_snapshot_index(ptr), 42);
  TEST_ASSERT_EQUAL_INT(transaction_solid(ptr), 1);
  TEST_ASSERT_EQUAL_INT(transaction_validity(ptr), 5);
  TEST_ASSERT_TRUE(transaction_arrival_timestamp(ptr) <= current_timestamp_ms());

  TEST_ASSERT_EQUAL_INT(ptr->loaded_columns_mask.attachment & MASK_ATTACHMENT_ALL, 0);
  TEST_ASSERT_EQUAL_INT(ptr->loaded_columns_mask.consensus & MASK_CONSENSUS_ALL, 0);
  TEST_ASSERT_EQUAL_INT(ptr->loaded_columns_mask.data & MASK_DATA_ALL, 0);
}

static void test_transaction_load_essence_attachment_metadata(void) {
  iota_transaction_t transaction;
  DECLARE_PACK_SINGLE_TX(loaded_transaction, ptr, pack);

  store_test_transaction(&transaction);

  TEST_ASSERT(storage_transaction_update_snapshot_index(&connection, TEST_TX_HASH, 42) == RC_OK);
  TEST_ASSERT(storage_transaction_update_solidity(&connection, TEST_TX_HASH, 1) == RC_OK);
  TEST_ASSERT(storage_transaction_update_validity(&connection, TEST_TX_HASH, 5) == RC_OK);

  TEST_ASSERT(storage_transaction_load_essence_attachment_metadata(&connection, TEST_TX_HASH, &pack) == RC_OK);
  TEST_ASSERT_EQUAL_INT(pack.num_loaded, 1);
  TEST_ASSERT_FALSE(pack.insufficient_capacity);

  TEST_ASSERT_EQUAL_MEMORY(transaction_address(ptr), transaction_address(&transaction), NUM_FLEX_TRITS_ADDRESS);
  TEST_ASSERT_EQUAL_INT(transaction_value(ptr), transaction_value(&transaction));
  TEST_ASSERT_EQUAL_MEMORY(transaction_obsolete_tag(ptr), transaction_obsolete_tag(&transaction),
                           NUM_FLEX_TRITS_OBSOLETE_TAG);
  TEST_ASSERT_EQUAL_INT(transaction_timestamp(ptr), transaction_timestamp(&transaction));
  TEST_ASSERT_EQUAL_INT(transaction_current_index(ptr), transaction_current_index(&transaction));
  TEST_ASSERT_EQUAL_INT(transaction_last_index(ptr), transaction_last_index(&transaction));
  TEST_ASSERT_EQUAL_MEMORY(transaction_bundle(ptr), transaction_bundle(&transaction), NUM_FLEX_TRITS_BUNDLE);
  TEST_ASSERT_EQUAL_MEMORY(transaction_trunk(ptr), transaction_trunk(&transaction), NUM_FLEX_TRITS_TRUNK);
  TEST_ASSERT_EQUAL_MEMORY(transaction_branch(ptr), transaction_branch(&transaction), NUM_FLEX_TRITS_BRANCH);
  TEST_ASSERT_EQUAL_INT(transaction_attachment_timestamp(ptr), transaction_attachment_timestamp(&transaction));
  TEST_ASSERT_EQUAL_INT(transaction_attachment_timestamp_lower(ptr),
                        transaction_attachment_timestamp_lower(&transaction));
  TEST_ASSERT_EQUAL_INT(transaction_attachment_timestamp_upper(ptr),
                        transaction_attachment_timestamp_upper(&transaction));
  TEST_ASSERT_EQUAL_MEMORY(transaction_nonce(ptr), transaction_nonce(&transaction), NUM_FLEX_TRITS_NONCE);
  TEST_ASSERT_EQUAL_MEMORY(transaction_tag(ptr), transaction_tag(&transaction), NUM_FLEX_TRITS_TAG);
  TEST_ASSERT_EQUAL_INT(transaction_snapshot_index(ptr), 42);
  TEST_ASSERT_EQUAL_INT(transaction_solid(ptr), 1);
  TEST_ASSERT_EQUAL_INT(transaction_validity(ptr), 5);
  TEST_ASSERT_TRUE(transaction_arrival_timestamp(ptr) <= current_timestamp_ms());

  TEST_ASSERT_EQUAL_INT(ptr->loaded_columns_mask.consensus & MASK_CONSENSUS_ALL, 0);
  TEST_ASSERT_EQUAL_INT(ptr->loaded_columns_mask.data & MASK_DATA_ALL, 0);
}

static void test_transaction_load_essence_consensus(void) {
  iota_transaction_t transaction;
  DECLARE_PACK_SINGLE_TX(loaded_transaction, ptr, pack);

  store_test_transaction(&transaction);

  TEST_ASSERT(storage_transaction_load_essence_consensus(&connection, TEST_TX_HASH, &pack) == RC_OK);
  TEST_ASSERT_EQUAL_INT(pack.num_loaded, 1);
  TEST_ASSERT_FALSE(pack.insufficient_capacity);

  TEST_ASSERT_EQUAL_MEMORY(transaction_address(ptr), transaction_address(&transaction), NUM_FLEX_TRITS_ADDRESS);
  TEST_ASSERT_EQUAL_INT(transaction_value(ptr), transaction_value(&transaction));
  TEST_ASSERT_EQUAL_MEMORY(transaction_obsolete_tag(ptr), transaction_obsolete_tag(&transaction),
                           NUM_FLEX_TRITS_OBSOLETE_TAG);
  TEST_ASSERT_EQUAL_INT(transaction_timestamp(ptr), transaction_timestamp(&transaction));
  TEST_ASSERT_EQUAL_INT(transaction_current_index(ptr), transaction_current_index(&transaction));
  TEST_ASSERT_EQUAL_INT(transaction_last_index(ptr), transaction_last_index(&transaction));
  TEST_ASSERT_EQUAL_MEMORY(transaction_bundle(ptr), transaction_bundle(&transaction), NUM_FLEX_TRITS_BUNDLE);
  TEST_ASSERT_EQUAL_MEMORY(transaction_hash(ptr), transaction_hash(&transaction), NUM_FLEX_TRITS_HASH);

  TEST_ASSERT_EQUAL_INT(ptr->loaded_columns_mask.attachment & MASK_ATTACHMENT_ALL, 0);
  TEST_ASSERT_EQUAL_INT(ptr->loaded_columns_mask.data & MASK_DATA_ALL, 0);
  TEST_ASSERT_EQUAL_INT(ptr->loaded_columns_mask.metadata & MASK_METADATA_ALL, 0);
}

static void test_transaction_load_metadata(void) {
  DECLARE_PACK_SINGLE_TX(loaded_transaction, ptr, pack);
  iota_transaction_t transaction;

  store_test_transaction(&transaction);

  TEST_ASSERT(storage_transaction_update_snapshot_index(&connection, TEST_TX_HASH, 42) == RC_OK);
  TEST_ASSERT(storage_transaction_update_solidity(&connection, TEST_TX_HASH, 1) == RC_OK);
  TEST_ASSERT(storage_transaction_update_validity(&connection, TEST_TX_HASH, 5) == RC_OK);

  TEST_ASSERT(storage_transaction_load_metadata(&connection, TEST_TX_HASH, &pack) == RC_OK);
  TEST_ASSERT_EQUAL_INT(pack.num_loaded, 1);
  TEST_ASSERT_FALSE(pack.insufficient_capacity);

  TEST_ASSERT_EQUAL_INT(transaction_snapshot_index(ptr), 42);
  TEST_ASSERT_EQUAL_INT(transaction_solid(ptr), 1);
  TEST_ASSERT_EQUAL_INT(transaction_validity(ptr), 5);
  TEST_ASSERT_TRUE(transaction_arrival_timestamp(ptr) <= current_timestamp_ms());

  TEST_ASSERT_EQUAL_INT(ptr->loaded_columns_mask.essence & MASK_ESSENCE_ALL, 0);
  TEST_ASSERT_EQUAL_INT(ptr->loaded_columns_mask.attachment & MASK_ATTACHMENT_ALL, 0);
  TEST_ASSERT_EQUAL_INT(ptr->loaded_columns_mask.consensus & MASK_CONSENSUS_ALL, 0);
  TEST_ASSERT_EQUAL_INT(ptr->loaded_columns_mask.data & MASK_DATA_ALL, 0);
}

static void test_transaction_exist_false(void) {
  bool exist;

  exist = true;
  TEST_ASSERT(storage_transaction_exist(&connection, TRANSACTION_FIELD_NONE, NULL, &exist) == RC_OK);
  TEST_ASSERT_FALSE(exist);

  exist = true;
  TEST_ASSERT(storage_transaction_exist(&connection, TRANSACTION_FIELD_HASH, TEST_TX_HASH, &exist) == RC_OK);
  TEST_ASSERT_FALSE(exist);
}

static void test_transaction_exist_true(void) {
  iota_transaction_t transaction;
  bool exist;

  store_test_transaction(&transaction);

  exist = false;
  TEST_ASSERT(storage_transaction_exist(&connection, TRANSACTION_FIELD_NONE, NULL, &exist) == RC_OK);
  TEST_ASSERT_TRUE(exist);

  exist = false;
  TEST_ASSERT(storage_transaction_exist(&connection, TRANSACTION_FIELD_HASH, TEST_TX_HASH, &exist) == RC_OK);
  TEST_ASSERT_TRUE(exist);
}

static void test_transaction_update_snapshot_index(void) {
  DECLARE_PACK_SINGLE_TX(loaded_transaction, ptr, pack);
  iota_transaction_t transaction;

  store_test_transaction(&transaction);

  TEST_ASSERT(storage_transaction_update_snapshot_index(&connection, TEST_TX_HASH, 42) == RC_OK);
  TEST_ASSERT(storage_transaction_load_metadata(&connection, TEST_TX_HASH, &pack) == RC_OK);

  TEST_ASSERT_EQUAL_INT(transaction_snapshot_index(ptr), 42);
  TEST_ASSERT_EQUAL_INT(transaction_solid(ptr), 0);
  TEST_ASSERT_EQUAL_INT(transaction_validity(ptr), 0);
  TEST_ASSERT_TRUE(transaction_arrival_timestamp(ptr) <= current_timestamp_ms());
}

static void test_transaction_update_solidity(void) {
  DECLARE_PACK_SINGLE_TX(loaded_transaction, ptr, pack);
  iota_transaction_t transaction;

  store_test_transaction(&transaction);

  TEST_ASSERT(storage_transaction_update_solidity(&connection, TEST_TX_HASH, 1) == RC_OK);
  TEST_ASSERT(storage_transaction_load_metadata(&connection, TEST_TX_HASH, &pack) == RC_OK);

  TEST_ASSERT_EQUAL_INT(transaction_snapshot_index(ptr), 0);
  TEST_ASSERT_EQUAL_INT(transaction_solid(ptr), 1);
  TEST_ASSERT_EQUAL_INT(transaction_validity(ptr), 0);
  TEST_ASSERT_TRUE(transaction_arrival_timestamp(ptr) <= current_timestamp_ms());
}

static void test_transaction_update_validity(void) {
  DECLARE_PACK_SINGLE_TX(loaded_transaction, ptr, pack);
  iota_transaction_t transaction;

  store_test_transaction(&transaction);

  TEST_ASSERT(storage_transaction_update_validity(&connection, TEST_TX_HASH, 5) == RC_OK);
  TEST_ASSERT(storage_transaction_load_metadata(&connection, TEST_TX_HASH, &pack) == RC_OK);

  TEST_ASSERT_EQUAL_INT(transaction_snapshot_index(ptr), 0);
  TEST_ASSERT_EQUAL_INT(transaction_solid(ptr), 0);
  TEST_ASSERT_EQUAL_INT(transaction_validity(ptr), 5);
  TEST_ASSERT_TRUE(transaction_arrival_timestamp(ptr) <= current_timestamp_ms());
}

static void test_transaction_load_hashes(void) {}

static void test_transaction_load_hashes_of_approvers(void) {}

static void test_transaction_load_hashes_of_milestone_candidates(void) {}

static void test_transaction_approvers_count(void) {
  uint64_t count = 0;
  trit_t hash[HASH_LENGTH_TRIT];
  flex_trit_t transaction_trits[FLEX_TRIT_SIZE_8019];
  iota_transaction_t transaction;

  flex_trits_from_trytes(transaction_trits, NUM_TRITS_SERIALIZED_TRANSACTION, TEST_TX_TRYTES,
                         NUM_TRITS_SERIALIZED_TRANSACTION, NUM_TRYTES_SERIALIZED_TRANSACTION);
  transaction_deserialize_from_trits(&transaction, transaction_trits, true);

  TEST_ASSERT(storage_transaction_approvers_count(&connection, TEST_TX_HASH, &count) == RC_OK);
  TEST_ASSERT_EQUAL_INT(count, 0);

  TEST_ASSERT(storage_transaction_store(&connection, &transaction) == RC_OK);

  flex_trits_to_trits(hash, HASH_LENGTH_TRIT, transaction_hash(&transaction), HASH_LENGTH_TRIT, HASH_LENGTH_TRIT);
  add_assign(hash, HASH_LENGTH_TRIT, 1);

  for (size_t i = 0; i < 10; i++) {
    if (i % 2) {
      memcpy(transaction_trunk(&transaction), TEST_TX_HASH, FLEX_TRIT_SIZE_243);
    } else {
      memcpy(transaction_branch(&transaction), TEST_TX_HASH, FLEX_TRIT_SIZE_243);
    }
    flex_trits_from_trits(transaction_hash(&transaction), HASH_LENGTH_TRIT, hash, HASH_LENGTH_TRIT, HASH_LENGTH_TRIT);
    TEST_ASSERT(storage_transaction_store(&connection, &transaction) == RC_OK);
    transaction_deserialize_from_trits(&transaction, transaction_trits, true);
    add_assign(hash, HASH_LENGTH_TRIT, 1);
  }

  TEST_ASSERT(storage_transaction_approvers_count(&connection, TEST_TX_HASH, &count) == RC_OK);
  TEST_ASSERT_EQUAL_INT(count, 10);
}

static void test_transaction_find(void) {}

static void test_transaction_delete(void) {
  iota_transaction_t transaction;
  bool exist;

  store_test_transaction(&transaction);

  exist = false;
  TEST_ASSERT(storage_transaction_exist(&connection, TRANSACTION_FIELD_HASH, TEST_TX_HASH, &exist) == RC_OK);
  TEST_ASSERT_TRUE(exist);

  TEST_ASSERT(storage_transaction_delete(&connection, TEST_TX_HASH) == RC_OK);

  TEST_ASSERT(storage_transaction_exist(&connection, TRANSACTION_FIELD_HASH, TEST_TX_HASH, &exist) == RC_OK);
  TEST_ASSERT_FALSE(exist);
}

static void test_transactions_metadata_clear(void) {
  DECLARE_PACK_SINGLE_TX(loaded_transaction, ptr, pack);
  iota_transaction_t transaction;

  store_test_transaction(&transaction);

  TEST_ASSERT(storage_transaction_update_snapshot_index(&connection, TEST_TX_HASH, 42) == RC_OK);
  TEST_ASSERT(storage_transaction_update_solidity(&connection, TEST_TX_HASH, 1) == RC_OK);
  TEST_ASSERT(storage_transaction_update_validity(&connection, TEST_TX_HASH, 5) == RC_OK);

  TEST_ASSERT(storage_transactions_metadata_clear(&connection) == RC_OK);
  TEST_ASSERT(storage_transaction_load_metadata(&connection, TEST_TX_HASH, &pack) == RC_OK);

  TEST_ASSERT_EQUAL_INT(transaction_snapshot_index(ptr), 0);
  TEST_ASSERT_EQUAL_INT(transaction_solid(ptr), 0);
  TEST_ASSERT_EQUAL_INT(transaction_validity(ptr), 0);
  TEST_ASSERT_TRUE(transaction_arrival_timestamp(ptr) <= current_timestamp_ms());
}

static void test_transactions_update_snapshot_index(void) {
  DECLARE_PACK_SINGLE_TX(loaded_transaction, ptr, pack);
  trit_t hash[HASH_LENGTH_TRIT];
  flex_trit_t transaction_trits[FLEX_TRIT_SIZE_8019];
  iota_transaction_t transaction;
  hash243_set_t hashes = NULL;

  flex_trits_from_trytes(transaction_trits, NUM_TRITS_SERIALIZED_TRANSACTION, TEST_TX_TRYTES,
                         NUM_TRITS_SERIALIZED_TRANSACTION, NUM_TRYTES_SERIALIZED_TRANSACTION);
  transaction_deserialize_from_trits(&transaction, transaction_trits, true);

  flex_trits_to_trits(hash, HASH_LENGTH_TRIT, transaction_hash(&transaction), HASH_LENGTH_TRIT, HASH_LENGTH_TRIT);

  for (size_t i = 0; i < 10; i++) {
    flex_trits_from_trits(transaction_hash(&transaction), HASH_LENGTH_TRIT, hash, HASH_LENGTH_TRIT, HASH_LENGTH_TRIT);
    TEST_ASSERT(storage_transaction_store(&connection, &transaction) == RC_OK);
    if (i % 2) {
      TEST_ASSERT(hash243_set_add(&hashes, transaction_hash(&transaction)) == RC_OK);
    }
    add_assign(hash, HASH_LENGTH_TRIT, 1);
  }

  TEST_ASSERT(storage_transactions_update_snapshot_index(&connection, hashes, 42) == RC_OK);

  flex_trits_to_trits(hash, HASH_LENGTH_TRIT, TEST_TX_HASH, HASH_LENGTH_TRIT, HASH_LENGTH_TRIT);

  for (size_t i = 0; i < 10; i++) {
    flex_trits_from_trits(transaction_hash(&transaction), HASH_LENGTH_TRIT, hash, HASH_LENGTH_TRIT, HASH_LENGTH_TRIT);
    TEST_ASSERT(storage_transaction_load_metadata(&connection, transaction_hash(&transaction), &pack) == RC_OK);
    if (i % 2) {
      TEST_ASSERT_EQUAL_INT(transaction_snapshot_index(ptr), 42);
    } else {
      TEST_ASSERT_EQUAL_INT(transaction_snapshot_index(ptr), 0);
    }
    add_assign(hash, HASH_LENGTH_TRIT, 1);
  }

  hash243_set_free(&hashes);
}

static void test_transactions_update_solidity(void) {}

static void test_transactions_delete(void) {
  uint64_t count = 0;
  trit_t hash[HASH_LENGTH_TRIT];
  flex_trit_t transaction_trits[FLEX_TRIT_SIZE_8019];
  iota_transaction_t transaction;
  hash243_set_t hashes = NULL;

  flex_trits_from_trytes(transaction_trits, NUM_TRITS_SERIALIZED_TRANSACTION, TEST_TX_TRYTES,
                         NUM_TRITS_SERIALIZED_TRANSACTION, NUM_TRYTES_SERIALIZED_TRANSACTION);
  transaction_deserialize_from_trits(&transaction, transaction_trits, true);

  TEST_ASSERT(storage_transaction_count(&connection, &count) == RC_OK);
  TEST_ASSERT_EQUAL_INT(count, 0);

  flex_trits_to_trits(hash, HASH_LENGTH_TRIT, transaction_hash(&transaction), HASH_LENGTH_TRIT, HASH_LENGTH_TRIT);

  for (size_t i = 0; i < 10; i++) {
    flex_trits_from_trits(transaction_hash(&transaction), HASH_LENGTH_TRIT, hash, HASH_LENGTH_TRIT, HASH_LENGTH_TRIT);
    TEST_ASSERT(storage_transaction_store(&connection, &transaction) == RC_OK);
    if (i % 2) {
      TEST_ASSERT(hash243_set_add(&hashes, transaction_hash(&transaction)) == RC_OK);
    }
    add_assign(hash, HASH_LENGTH_TRIT, 1);
  }

  TEST_ASSERT(storage_transaction_count(&connection, &count) == RC_OK);
  TEST_ASSERT_EQUAL_INT(count, 10);

  TEST_ASSERT(storage_transactions_delete(&connection, hashes) == RC_OK);

  TEST_ASSERT(storage_transaction_count(&connection, &count) == RC_OK);
  TEST_ASSERT_EQUAL_INT(count, 5);

  hash243_set_free(&hashes);
}

static void test_bundle_update_validity(void) {}

static void test_milestone_clear(void) {
  iota_milestone_t milestone;
  bool exist = false;

  store_test_milestone(&milestone);

  TEST_ASSERT(storage_milestone_exist(&connection, TEST_TX_HASH, &exist) == RC_OK);
  TEST_ASSERT_TRUE(exist);

  TEST_ASSERT(storage_milestone_clear(&connection) == RC_OK);

  TEST_ASSERT(storage_milestone_exist(&connection, TEST_TX_HASH, &exist) == RC_OK);
  TEST_ASSERT_FALSE(exist);
}

static void test_milestone_store(void) {
  iota_milestone_t milestone;

  store_test_milestone(&milestone);
}

static void test_milestone_load_not_found(void) {
  DECLARE_PACK_SINGLE_MILESTONE(loaded_milestone, ptr, pack);

  TEST_ASSERT(storage_milestone_load(&connection, TEST_TX_HASH, &pack) == RC_OK);
  TEST_ASSERT_EQUAL_INT(pack.num_loaded, 0);
  TEST_ASSERT_FALSE(pack.insufficient_capacity);
}

static void test_milestone_load_found(void) {
  iota_milestone_t milestone;
  DECLARE_PACK_SINGLE_MILESTONE(loaded_milestone, ptr, pack);

  store_test_milestone(&milestone);

  TEST_ASSERT(storage_milestone_load(&connection, TEST_TX_HASH, &pack) == RC_OK);
  TEST_ASSERT_EQUAL_INT(pack.num_loaded, 1);
  TEST_ASSERT_FALSE(pack.insufficient_capacity);

  TEST_ASSERT_EQUAL_INT(ptr->index, milestone.index);
  TEST_ASSERT_EQUAL_MEMORY(ptr->hash, milestone.hash, NUM_FLEX_TRITS_HASH);
}

static void test_milestone_load_last(void) {
  iota_milestone_t milestone;
  trit_t hash[HASH_LENGTH_TRIT];
  DECLARE_PACK_SINGLE_MILESTONE(loaded_milestone, ptr, pack);

  flex_trits_to_trits(hash, HASH_LENGTH_TRIT, TEST_TX_HASH, HASH_LENGTH_TRIT, HASH_LENGTH_TRIT);
  for (size_t i = 0; i < 10; i++) {
    milestone.index = TEST_MS_INDEX + i;
    flex_trits_from_trits(milestone.hash, HASH_LENGTH_TRIT, hash, HASH_LENGTH_TRIT, HASH_LENGTH_TRIT);
    TEST_ASSERT(storage_milestone_store(&connection, &milestone) == RC_OK);
    add_assign(hash, HASH_LENGTH_TRIT, 1);
  }

  TEST_ASSERT(storage_milestone_load_last(&connection, &pack) == RC_OK);
  TEST_ASSERT_EQUAL_INT(pack.num_loaded, 1);
  TEST_ASSERT_FALSE(pack.insufficient_capacity);

  TEST_ASSERT_EQUAL_INT(ptr->index, TEST_MS_INDEX + 10 - 1);
  flex_trits_to_trits(hash, HASH_LENGTH_TRIT, TEST_TX_HASH, HASH_LENGTH_TRIT, HASH_LENGTH_TRIT);
  add_assign(hash, HASH_LENGTH_TRIT, 10 - 1);
  flex_trits_from_trits(milestone.hash, HASH_LENGTH_TRIT, hash, HASH_LENGTH_TRIT, HASH_LENGTH_TRIT);
  TEST_ASSERT_EQUAL_MEMORY(ptr->hash, milestone.hash, NUM_FLEX_TRITS_HASH);
}

static void test_milestone_load_first(void) {
  iota_milestone_t milestone;
  trit_t hash[HASH_LENGTH_TRIT];
  DECLARE_PACK_SINGLE_MILESTONE(loaded_milestone, ptr, pack);

  flex_trits_to_trits(hash, HASH_LENGTH_TRIT, TEST_TX_HASH, HASH_LENGTH_TRIT, HASH_LENGTH_TRIT);
  for (size_t i = 0; i < 10; i++) {
    milestone.index = TEST_MS_INDEX + i;
    flex_trits_from_trits(milestone.hash, HASH_LENGTH_TRIT, hash, HASH_LENGTH_TRIT, HASH_LENGTH_TRIT);
    TEST_ASSERT(storage_milestone_store(&connection, &milestone) == RC_OK);
    add_assign(hash, HASH_LENGTH_TRIT, 1);
  }

  TEST_ASSERT(storage_milestone_load_first(&connection, &pack) == RC_OK);
  TEST_ASSERT_EQUAL_INT(pack.num_loaded, 1);
  TEST_ASSERT_FALSE(pack.insufficient_capacity);

  TEST_ASSERT_EQUAL_INT(ptr->index, TEST_MS_INDEX);
  TEST_ASSERT_EQUAL_MEMORY(ptr->hash, TEST_TX_HASH, NUM_FLEX_TRITS_HASH);
}

static void test_milestone_load_by_index(void) {
  iota_milestone_t milestone;
  trit_t hash[HASH_LENGTH_TRIT];
  DECLARE_PACK_SINGLE_MILESTONE(loaded_milestone, ptr, pack);

  flex_trits_to_trits(hash, HASH_LENGTH_TRIT, TEST_TX_HASH, HASH_LENGTH_TRIT, HASH_LENGTH_TRIT);
  for (size_t i = 0; i < 10; i++) {
    milestone.index = TEST_MS_INDEX + i;
    flex_trits_from_trits(milestone.hash, HASH_LENGTH_TRIT, hash, HASH_LENGTH_TRIT, HASH_LENGTH_TRIT);
    TEST_ASSERT(storage_milestone_store(&connection, &milestone) == RC_OK);
    add_assign(hash, HASH_LENGTH_TRIT, 1);
  }

  TEST_ASSERT(storage_milestone_load_by_index(&connection, TEST_MS_INDEX + 5, &pack) == RC_OK);
  TEST_ASSERT_EQUAL_INT(pack.num_loaded, 1);
  TEST_ASSERT_FALSE(pack.insufficient_capacity);

  TEST_ASSERT_EQUAL_INT(ptr->index, TEST_MS_INDEX + 5);
  flex_trits_to_trits(hash, HASH_LENGTH_TRIT, TEST_TX_HASH, HASH_LENGTH_TRIT, HASH_LENGTH_TRIT);
  add_assign(hash, HASH_LENGTH_TRIT, 5);
  flex_trits_from_trits(milestone.hash, HASH_LENGTH_TRIT, hash, HASH_LENGTH_TRIT, HASH_LENGTH_TRIT);
  TEST_ASSERT_EQUAL_MEMORY(ptr->hash, milestone.hash, NUM_FLEX_TRITS_HASH);
}

static void test_milestone_load_next(void) {
  iota_milestone_t milestone;
  trit_t hash[HASH_LENGTH_TRIT];
  DECLARE_PACK_SINGLE_MILESTONE(loaded_milestone, ptr, pack);

  flex_trits_to_trits(hash, HASH_LENGTH_TRIT, TEST_TX_HASH, HASH_LENGTH_TRIT, HASH_LENGTH_TRIT);
  for (size_t i = 0; i < 10; i += 2) {
    milestone.index = TEST_MS_INDEX + i;
    flex_trits_from_trits(milestone.hash, HASH_LENGTH_TRIT, hash, HASH_LENGTH_TRIT, HASH_LENGTH_TRIT);
    TEST_ASSERT(storage_milestone_store(&connection, &milestone) == RC_OK);
    add_assign(hash, HASH_LENGTH_TRIT, 2);
  }

  TEST_ASSERT(storage_milestone_load_next(&connection, TEST_MS_INDEX + 2, &pack) == RC_OK);
  TEST_ASSERT_EQUAL_INT(pack.num_loaded, 1);
  TEST_ASSERT_FALSE(pack.insufficient_capacity);

  TEST_ASSERT_EQUAL_INT(ptr->index, TEST_MS_INDEX + 4);
  flex_trits_to_trits(hash, HASH_LENGTH_TRIT, TEST_TX_HASH, HASH_LENGTH_TRIT, HASH_LENGTH_TRIT);
  add_assign(hash, HASH_LENGTH_TRIT, 4);
  flex_trits_from_trits(milestone.hash, HASH_LENGTH_TRIT, hash, HASH_LENGTH_TRIT, HASH_LENGTH_TRIT);
  TEST_ASSERT_EQUAL_MEMORY(ptr->hash, milestone.hash, NUM_FLEX_TRITS_HASH);
}

static void test_milestone_exist_false(void) {
  bool exist = true;

  TEST_ASSERT(storage_milestone_exist(&connection, TEST_TX_HASH, &exist) == RC_OK);
  TEST_ASSERT_FALSE(exist);
}

static void test_milestone_exist_true(void) {
  iota_milestone_t milestone;
  bool exist = false;

  store_test_milestone(&milestone);

  TEST_ASSERT(storage_milestone_exist(&connection, TEST_TX_HASH, &exist) == RC_OK);
  TEST_ASSERT_TRUE(exist);
}

static void test_milestone_delete(void) {
  iota_milestone_t milestone;
  bool exist = false;

  store_test_milestone(&milestone);

  TEST_ASSERT(storage_milestone_exist(&connection, TEST_TX_HASH, &exist) == RC_OK);
  TEST_ASSERT_TRUE(exist);

  TEST_ASSERT(storage_milestone_delete(&connection, TEST_TX_HASH) == RC_OK);

  TEST_ASSERT(storage_milestone_exist(&connection, TEST_TX_HASH, &exist) == RC_OK);
  TEST_ASSERT_FALSE(exist);
}

static void test_state_delta_store(void) {
  iota_milestone_t milestone;
  state_delta_t state_delta = NULL;

  store_test_milestone(&milestone);

  TEST_ASSERT(state_delta_add(&state_delta, TEST_TX_HASH, 4242) == RC_OK);
  TEST_ASSERT(storage_state_delta_store(&connection, milestone.index, &state_delta) == RC_OK);

  state_delta_destroy(&state_delta);
}

static void test_state_delta_load_not_found(void) {
  state_delta_t state_delta = NULL;

  TEST_ASSERT(storage_state_delta_load(&connection, 42, &state_delta) == RC_OK);
  TEST_ASSERT_NULL(state_delta);
}

static void test_state_delta_load_found(void) {
  iota_milestone_t milestone;
  state_delta_t state_delta1 = NULL, state_delta2 = NULL;
  state_delta_entry_t *iter = NULL, *tmp = NULL;
  trit_t trits[HASH_LENGTH_TRIT] = {0};
  flex_trit_t hash[FLEX_TRIT_SIZE_243];

  store_test_milestone(&milestone);

  flex_trits_from_trits(hash, HASH_LENGTH_TRIT, trits, HASH_LENGTH_TRIT, HASH_LENGTH_TRIT);
  for (int64_t i = -100; i <= 100; i++) {
    flex_trits_from_trits(hash, HASH_LENGTH_TRIT, trits, HASH_LENGTH_TRIT, HASH_LENGTH_TRIT);
    TEST_ASSERT(state_delta_add(&state_delta1, hash, i) == RC_OK);
    add_assign(trits, HASH_LENGTH_TRIT, 1);
  }

  TEST_ASSERT(storage_state_delta_store(&connection, milestone.index, &state_delta1) == RC_OK);

  TEST_ASSERT(storage_state_delta_load(&connection, milestone.index, &state_delta2) == RC_OK);
  TEST_ASSERT(state_delta2 != NULL);

  int i = -100;
  iter = NULL;
  memset(trits, 0, sizeof(trits));
  HASH_ITER(hh, state_delta2, iter, tmp) {
    flex_trits_from_trits(hash, HASH_LENGTH_TRIT, trits, HASH_LENGTH_TRIT, HASH_LENGTH_TRIT);
    TEST_ASSERT_EQUAL_MEMORY(iter->hash, hash, FLEX_TRIT_SIZE_243);
    TEST_ASSERT_EQUAL_INT64(iter->value, i);
    add_assign(trits, HASH_LENGTH_TRIT, 1);
    i++;
  }

  state_delta_destroy(&state_delta1);
  state_delta_destroy(&state_delta2);
}

static void test_spent_address_store(void) {
  TEST_ASSERT(storage_spent_address_store(&connection, TEST_TX_HASH) == RC_OK);
}

static void test_spent_address_exist_false(void) {
  bool exist = true;

  TEST_ASSERT(storage_spent_address_exist(&connection, TEST_TX_HASH, &exist) == RC_OK);
  TEST_ASSERT_FALSE(exist);
}

static void test_spent_address_exist_true(void) {
  bool exist = false;

  TEST_ASSERT(storage_spent_address_store(&connection, TEST_TX_HASH) == RC_OK);

  TEST_ASSERT(storage_spent_address_exist(&connection, TEST_TX_HASH, &exist) == RC_OK);
  TEST_ASSERT_TRUE(exist);
}

static void test_spent_addresses_store(void) {
  bool exist = true;
  tryte_t address_trytes[HASH_LENGTH_TRYTE];
  flex_trit_t address_trits[FLEX_TRIT_SIZE_243];
  hash243_set_t addresses = NULL;

  memset(address_trytes, '9', HASH_LENGTH_TRYTE);
  address_trytes[0] = 'A';

  for (size_t i = 0; i < 26; i++) {
    flex_trits_from_trytes(address_trits, HASH_LENGTH_TRIT, address_trytes, HASH_LENGTH_TRYTE, HASH_LENGTH_TRYTE);
    if (i % 2) {
      TEST_ASSERT(hash243_set_add(&addresses, address_trits) == RC_OK);
    }
    address_trytes[0]++;
  }

  TEST_ASSERT(storage_spent_addresses_store(&connection, addresses) == RC_OK);

  address_trytes[0] = 'A';

  for (size_t i = 0; i < 26; i++) {
    flex_trits_from_trytes(address_trits, HASH_LENGTH_TRIT, address_trytes, HASH_LENGTH_TRYTE, HASH_LENGTH_TRYTE);
    if (i % 2) {
      exist = false;
      TEST_ASSERT(storage_spent_address_exist(&connection, address_trits, &exist) == RC_OK);
      TEST_ASSERT_TRUE(exist);
    } else {
      exist = true;
      TEST_ASSERT(storage_spent_address_exist(&connection, address_trits, &exist) == RC_OK);
      TEST_ASSERT_FALSE(exist);
    }
    address_trytes[0]++;
  }

  hash243_set_free(&addresses);
}

// void test_stored_load_hashes_by_address(void) {
//   flex_trit_t *hashes[5];
//   iota_stor_pack_t pack = {.models = (void **)hashes, .capacity = 5,
//   .num_loaded = 0, .insufficient_capacity = false}; for (size_t i =
//   0; i < 5; ++i) {
//     hashes[i] = (flex_trit_t *)malloc(FLEX_TRIT_SIZE_243);
//   }
//
//   flex_trit_t tx_test_trits[FLEX_TRIT_SIZE_8019];
//   flex_trits_from_trytes(tx_test_trits,
//   NUM_TRITS_SERIALIZED_TRANSACTION, TEST_TX_TRYTES,
//                          NUM_TRITS_SERIALIZED_TRANSACTION,
//                          NUM_TRYTES_SERIALIZED_TRANSACTION);
//   iota_transaction_t *test_tx =
//   transaction_deserialize(tx_test_trits, true);
//
//   TEST_ASSERT(storage_transaction_load_hashes(&connection,
//   TRANSACTION_FIELD_ADDRESS, transaction_address(test_tx),
//                                               &pack) == RC_OK);
//   TEST_ASSERT_EQUAL_INT(1, pack.num_loaded);
//   TEST_ASSERT_EQUAL_MEMORY(transaction_hash(test_tx), ((flex_trit_t
//   *)pack.models[0]), FLEX_TRIT_SIZE_243);
//
//   for (size_t i = 0; i < 5; ++i) {
//     free(hashes[i]);
//   }
//   transaction_free(test_tx);
// }
//
// void test_stored_load_hashes_of_approvers(void) {
//   flex_trit_t hashes[5][FLEX_TRIT_SIZE_243];
//   iota_stor_pack_t pack = {.models = (void **)hashes, .capacity = 5,
//   .num_loaded = 0, .insufficient_capacity = false};
//
//   flex_trit_t tx_test_trits[FLEX_TRIT_SIZE_8019];
//   flex_trits_from_trytes(tx_test_trits,
//   NUM_TRITS_SERIALIZED_TRANSACTION, TEST_TX_TRYTES,
//                          NUM_TRITS_SERIALIZED_TRANSACTION,
//                          NUM_TRYTES_SERIALIZED_TRANSACTION);
//   iota_transaction_t *test_tx =
//   transaction_deserialize(tx_test_trits, true);
//
//   TEST_ASSERT(storage_transaction_load_hashes_of_approvers(&connection,
//   transaction_address(test_tx), &pack, 0) ==
//               RC_OK);
//   TEST_ASSERT_EQUAL_INT(0, pack.num_loaded);
//
//   transaction_free(test_tx);
// }
//

// void test_transactions_update_solid_states_one_transaction(void) {
//   flex_trit_t tx_test_trits[FLEX_TRIT_SIZE_8019];
//   flex_trits_from_trytes(tx_test_trits,
//   NUM_TRITS_SERIALIZED_TRANSACTION, TEST_TX_TRYTES,
//                          NUM_TRITS_SERIALIZED_TRANSACTION,
//                          NUM_TRYTES_SERIALIZED_TRANSACTION);
//   iota_transaction_t *test_tx =
//   transaction_deserialize(tx_test_trits, true);
//   DECLARE_PACK_SINGLE_TX(tx, tx_ptr, pack);
//
//   hash243_set_t hashes = NULL;
//   hash243_set_add(&hashes, transaction_hash(test_tx));
//   TEST_ASSERT(storage_transactions_update_solid_state(&connection,
//   hashes, true) == RC_OK); hash_pack_reset(&pack);
//   TEST_ASSERT(storage_transaction_load_metadata(&connection,
//   transaction_hash(test_tx), &pack) == RC_OK);
//   TEST_ASSERT_EQUAL_INT(1, pack.num_loaded);
//   TEST_ASSERT(transaction_solid(&tx));
//   hash243_set_free(&hashes);
//   transaction_free(test_tx);
// }
//
// void test_transactions_update_solid_states_two_transaction(void) {
//   flex_trit_t tx_test_trits[FLEX_TRIT_SIZE_8019];
//   flex_trits_from_trytes(tx_test_trits,
//   NUM_TRITS_SERIALIZED_TRANSACTION, TEST_TX_TRYTES,
//                          NUM_TRITS_SERIALIZED_TRANSACTION,
//                          NUM_TRYTES_SERIALIZED_TRANSACTION);
//   iota_transaction_t *test_tx =
//   transaction_deserialize(tx_test_trits, true);
//   DECLARE_PACK_SINGLE_TX(tx, tx_ptr, pack);
//
//   iota_transaction_t second_test_transaction = *test_tx;
//   // Make them distinguishable
//   trit_t modified_trit = flex_trits_at(transaction_hash(test_tx),
//   FLEX_TRIT_SIZE_243, 0); if (abs(modified_trit) > 0) {
//     modified_trit = 0;
//   } else {
//     modified_trit = 1;
//   }
//   flex_trits_set_at(second_test_transaction.consensus.hash,
//   FLEX_TRIT_SIZE_243, 0, modified_trit);
//
//   TEST_ASSERT(storage_transaction_store(&connection,
//   &second_test_transaction) == RC_OK);
//
//   hash243_set_t hashes = NULL;
//   hash243_set_add(&hashes, transaction_hash(test_tx));
//   hash243_set_add(&hashes,
//   transaction_hash(&second_test_transaction));
//   TEST_ASSERT(storage_transactions_update_solid_state(&connection,
//   hashes, true) == RC_OK); hash_pack_reset(&pack);
//   TEST_ASSERT(storage_transaction_load_metadata(&connection,
//   transaction_hash(test_tx), &pack) == RC_OK);
//   TEST_ASSERT_EQUAL_INT(1, pack.num_loaded);
//   TEST_ASSERT(transaction_solid(&tx));
//
//   hash_pack_reset(&pack);
//   TEST_ASSERT(storage_transaction_load_metadata(&connection,
//   transaction_hash(&second_test_transaction), &pack) ==
//               RC_OK);
//   TEST_ASSERT_EQUAL_INT(1, pack.num_loaded);
//   TEST_ASSERT(transaction_solid(&tx));
//   hash243_set_free(&hashes);
//   transaction_free(test_tx);
// }
//
// void test_transactions_arrival_time(void) {
//   flex_trit_t tx_test_trits[FLEX_TRIT_SIZE_8019];
//   flex_trits_from_trytes(tx_test_trits,
//   NUM_TRITS_SERIALIZED_TRANSACTION, TEST_TX_TRYTES,
//                          NUM_TRITS_SERIALIZED_TRANSACTION,
//                          NUM_TRYTES_SERIALIZED_TRANSACTION);
//   iota_transaction_t *test_tx =
//   transaction_deserialize(tx_test_trits, true);
//   DECLARE_PACK_SINGLE_TX(tx, tx_ptr, pack);
//
//   iota_transaction_t second_test_transaction = *test_tx;
//   // Make them distinguishable
//   trit_t modified_trit = flex_trits_at(transaction_hash(test_tx),
//   FLEX_TRIT_SIZE_243, 2); if (abs(modified_trit) > 0) {
//     modified_trit = 0;
//   } else {
//     modified_trit = 1;
//   }
//   flex_trits_set_at(second_test_transaction.consensus.hash,
//   FLEX_TRIT_SIZE_243, 2, modified_trit);
//   TEST_ASSERT(storage_transaction_store(&connection,
//   &second_test_transaction) == RC_OK); hash_pack_reset(&pack);
//   TEST_ASSERT(storage_transaction_load_metadata(&connection,
//   transaction_hash(test_tx), &pack) == RC_OK);
//   TEST_ASSERT_EQUAL_INT(1, pack.num_loaded);
//   uint64_t test_tx_timestamp = transaction_arrival_timestamp(&tx);
//   hash_pack_reset(&pack);
//   TEST_ASSERT(storage_transaction_load_metadata(&connection,
//   transaction_hash(&second_test_transaction), &pack) ==
//               RC_OK);
//   TEST_ASSERT_EQUAL_INT(1, pack.num_loaded);
//   TEST_ASSERT(transaction_arrival_timestamp(&tx) >
//   test_tx_timestamp); transaction_free(test_tx);
// }
//
// void test_transactions_delete_two_transactions(void) {
//   flex_trit_t tx_test_trits[FLEX_TRIT_SIZE_8019];
//   flex_trits_from_trytes(tx_test_trits,
//   NUM_TRITS_SERIALIZED_TRANSACTION, TEST_TX_TRYTES,
//                          NUM_TRITS_SERIALIZED_TRANSACTION,
//                          NUM_TRYTES_SERIALIZED_TRANSACTION);
//   iota_transaction_t *test_tx =
//   transaction_deserialize(tx_test_trits, true); iota_transaction_t
//   second_test_transaction = *test_tx;
//   // Make them distinguishable
//   trit_t modified_trit = flex_trits_at(transaction_hash(test_tx),
//   FLEX_TRIT_SIZE_243, 0); if (abs(modified_trit) > 0) {
//     modified_trit = 0;
//   } else {
//     modified_trit = 1;
//   }
//   flex_trits_set_at(second_test_transaction.consensus.hash,
//   FLEX_TRIT_SIZE_243, 0, modified_trit);
//
//   hash243_set_t transactions_to_delete = NULL;
//
//   bool exist = false;
//   TEST_ASSERT(storage_transaction_exist(&connection,
//   TRANSACTION_FIELD_HASH, transaction_hash(test_tx), &exist) ==
//               RC_OK);
//   TEST_ASSERT(exist == true);
//
//   TEST_ASSERT(storage_transaction_exist(&connection,
//   TRANSACTION_FIELD_HASH,
//   transaction_hash(&second_test_transaction),
//                                         &exist) == RC_OK);
//   TEST_ASSERT(exist == true);
//
//   TEST_ASSERT(storage_transactions_delete(&connection,
//   transactions_to_delete) == RC_OK);
//
//   hash243_set_add(&transactions_to_delete,
//   transaction_hash(test_tx));
//   hash243_set_add(&transactions_to_delete,
//   transaction_hash(&second_test_transaction));
//
//   TEST_ASSERT(storage_transactions_delete(&connection,
//   transactions_to_delete) == RC_OK);
//
//   TEST_ASSERT(storage_transaction_exist(&connection,
//   TRANSACTION_FIELD_HASH, transaction_hash(test_tx), &exist) ==
//               RC_OK);
//   TEST_ASSERT(exist == false);
//   TEST_ASSERT(storage_transaction_exist(&connection,
//   TRANSACTION_FIELD_HASH,
//   transaction_hash(&second_test_transaction),
//                                         &exist) == RC_OK);
//   TEST_ASSERT(exist == false);
//
//   hash243_set_free(&transactions_to_delete);
//   transaction_free(test_tx);
// }

int main(void) {
  UNITY_BEGIN();
  TEST_ASSERT(storage_init() == RC_OK);

  config.db_path = tangle_test_db_path;

  connection_type = STORAGE_CONNECTION_TANGLE;

  RUN_TEST(test_connection_init_destroy);

  RUN_TEST(test_transaction_count);
  RUN_TEST(test_transaction_store);
  RUN_TEST(test_transaction_store_duplicate);
  RUN_TEST(test_transaction_load_not_found);
  RUN_TEST(test_transaction_load_found);
  RUN_TEST(test_transaction_load_essence_metadata);
  RUN_TEST(test_transaction_load_essence_attachment_metadata);
  RUN_TEST(test_transaction_load_essence_consensus);
  RUN_TEST(test_transaction_load_metadata);
  RUN_TEST(test_transaction_exist_false);
  RUN_TEST(test_transaction_exist_true);
  RUN_TEST(test_transaction_update_snapshot_index);
  RUN_TEST(test_transaction_update_solidity);
  RUN_TEST(test_transaction_update_validity);
  RUN_TEST(test_transaction_load_hashes);
  RUN_TEST(test_transaction_load_hashes_of_approvers);
  RUN_TEST(test_transaction_load_hashes_of_milestone_candidates);
  RUN_TEST(test_transaction_approvers_count);
  RUN_TEST(test_transaction_find);
  RUN_TEST(test_transaction_delete);
  RUN_TEST(test_transactions_metadata_clear);
  RUN_TEST(test_transactions_update_snapshot_index);
  RUN_TEST(test_transactions_update_solidity);
  RUN_TEST(test_transactions_delete);

  RUN_TEST(test_bundle_update_validity);

  RUN_TEST(test_milestone_clear);
  RUN_TEST(test_milestone_store);
  RUN_TEST(test_milestone_load_not_found);
  RUN_TEST(test_milestone_load_found);
  RUN_TEST(test_milestone_load_last);
  RUN_TEST(test_milestone_load_first);
  RUN_TEST(test_milestone_load_by_index);
  RUN_TEST(test_milestone_load_next);
  RUN_TEST(test_milestone_exist_false);
  RUN_TEST(test_milestone_exist_true);
  RUN_TEST(test_milestone_delete);

  RUN_TEST(test_state_delta_store);
  RUN_TEST(test_state_delta_load_not_found);
  RUN_TEST(test_state_delta_load_found);

  connection_type = STORAGE_CONNECTION_SPENT_ADDRESSES;

  RUN_TEST(test_connection_init_destroy);

  RUN_TEST(test_spent_address_store);
  RUN_TEST(test_spent_address_exist_false);
  RUN_TEST(test_spent_address_exist_true);
  RUN_TEST(test_spent_addresses_store);

  TEST_ASSERT(storage_destroy() == RC_OK);
  return UNITY_END();
}
