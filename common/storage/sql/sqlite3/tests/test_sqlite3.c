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
#include "common/storage/sql/defs.h"
#include "common/storage/sql/sqlite3/connection.h"
#include "common/storage/storage.h"
#include "common/storage/tests/helpers/defs.h"
#include "utils/containers/hash/hash243_set.h"
#include "utils/files.h"

static bool debug_mode = false;
static char *test_db_path = "common/storage/sql/sqlite3/tests/test.db";
static char *ciri_db_path = "common/storage/sql/sqlite3/tests/ciri.db";
// TODO Remove after "Common definitions #329" is merged
#define HASH_LENGTH 243

#if defined(FLEX_TRIT_ENCODING_1_TRIT_PER_BYTE)
const flex_trit_t HASH[] = {
    1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0,
    0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1,
    0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0,
    1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0,
    0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1,
    0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0,
    1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0,
};
#elif defined(FLEX_TRIT_ENCODING_3_TRITS_PER_BYTE)
const flex_trit_t HASH[] = {
    65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65,
    65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65,
    65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65,
};
#elif defined(FLEX_TRIT_ENCODING_4_TRITS_PER_BYTE)
const flex_trit_t HASH[] = {
    65, 16, 4, 65, 16, 4, 65, 16, 4, 65, 16, 4, 65, 16, 4, 65, 16, 4, 65, 16, 4,
    65, 16, 4, 65, 16, 4, 65, 16, 4, 65, 16, 4, 65, 16, 4, 65, 16, 4, 65, 16, 4,
    65, 16, 4, 65, 16, 4, 65, 16, 4, 65, 16, 4, 65, 16, 4, 65, 16, 4, 1,
};
#elif defined(FLEX_TRIT_ENCODING_5_TRITS_PER_BYTE)
const flex_trit_t HASH[] = {
    28, 84, 9,  28, 84, 9,  28, 84, 9,  28, 84, 9,  28, 84, 9,  28, 84, 9,  28, 84, 9,  28, 84, 9, 28,
    84, 9,  28, 84, 9,  28, 84, 9,  28, 84, 9,  28, 84, 9,  28, 84, 9,  28, 84, 9,  28, 84, 9,  1,
};
#endif

static storage_connection_t connection;

void test_init_connection(void) {
  connection_config_t config;
  config.db_path = test_db_path;
  TEST_ASSERT(connection_init(&connection, &config) == RC_OK);
}

void test_destroy_connection(void) { TEST_ASSERT(connection_destroy(&connection) == RC_OK); }

void test_initialized_db_empty_transaction(void) {
  bool exist = false;

  TEST_ASSERT(iota_stor_milestone_exist(&connection, HASH, &exist) == RC_OK);
  TEST_ASSERT(exist == false);
}

void test_initialized_db_empty_milestone(void) {
  bool exist = false;

  flex_trit_t hash[FLEX_TRIT_SIZE_243];
  memcpy(hash, HASH, FLEX_TRIT_SIZE_243);
  TEST_ASSERT(iota_stor_transaction_exist(&connection, TRANSACTION_FIELD_HASH, hash, &exist) == RC_OK);
  TEST_ASSERT(exist == false);
}

void test_stored_transaction(void) {
  flex_trit_t tx_test_trits[FLEX_TRIT_SIZE_8019];
  flex_trits_from_trytes(tx_test_trits, NUM_TRITS_SERIALIZED_TRANSACTION, TEST_TX_TRYTES,
                         NUM_TRITS_SERIALIZED_TRANSACTION, NUM_TRYTES_SERIALIZED_TRANSACTION);
  iota_transaction_t *test_tx = transaction_deserialize(tx_test_trits, true);

  TEST_ASSERT(iota_stor_transaction_store(&connection, test_tx) == RC_OK);
  // Test primary key constraint violation
  TEST_ASSERT(iota_stor_transaction_store(&connection, test_tx) == RC_SQLITE3_FAILED_STEP);
  bool exist = false;

  TEST_ASSERT(iota_stor_transaction_exist(&connection, TRANSACTION_FIELD_NONE, NULL, &exist) == RC_OK);
  TEST_ASSERT(exist == true);
  TEST_ASSERT(iota_stor_transaction_exist(&connection, TRANSACTION_FIELD_HASH, transaction_hash(test_tx), &exist) ==
              RC_OK);
  TEST_ASSERT(exist == true);

  TEST_ASSERT(iota_stor_transaction_exist(&connection, TRANSACTION_FIELD_NONE, NULL, &exist) == RC_OK);
  TEST_ASSERT(exist == true);

  iota_transaction_t *txs[5];
  iota_stor_pack_t pack = {.models = (void **)txs, .capacity = 5, .num_loaded = 0, .insufficient_capacity = false};

  for (int i = 0; i < 5; ++i) {
    pack.models[i] = transaction_new();
  }

  TEST_ASSERT(iota_stor_transaction_load(&connection, TRANSACTION_FIELD_HASH, transaction_hash(test_tx), &pack) ==
              RC_OK);
  TEST_ASSERT_EQUAL_INT(1, pack.num_loaded);

  TEST_ASSERT_EQUAL_MEMORY(transaction_nonce(txs[0]), transaction_nonce(test_tx), FLEX_TRIT_SIZE_81);
  TEST_ASSERT_EQUAL_MEMORY(transaction_signature(txs[0]), transaction_signature(test_tx), FLEX_TRIT_SIZE_6561);
  TEST_ASSERT_EQUAL_MEMORY(transaction_address(txs[0]), transaction_address(test_tx), FLEX_TRIT_SIZE_243);
  TEST_ASSERT_EQUAL_MEMORY(transaction_branch(txs[0]), transaction_branch(test_tx), FLEX_TRIT_SIZE_243);
  TEST_ASSERT_EQUAL_MEMORY(transaction_trunk(txs[0]), transaction_trunk(test_tx), FLEX_TRIT_SIZE_243);
  TEST_ASSERT_EQUAL_MEMORY(transaction_bundle(txs[0]), transaction_bundle(test_tx), FLEX_TRIT_SIZE_243);
  TEST_ASSERT_EQUAL_INT(transaction_value(txs[0]), transaction_value(test_tx));
  TEST_ASSERT_EQUAL_INT(transaction_attachment_timestamp_lower(txs[0]), transaction_attachment_timestamp(test_tx));
  TEST_ASSERT_EQUAL_INT(transaction_attachment_timestamp_upper(txs[0]),
                        transaction_attachment_timestamp_upper(test_tx));
  TEST_ASSERT_EQUAL_INT(transaction_attachment_timestamp_lower(txs[0]),
                        transaction_attachment_timestamp_lower(test_tx));
  TEST_ASSERT_EQUAL_INT(transaction_timestamp(txs[0]), transaction_timestamp(test_tx));
  TEST_ASSERT_EQUAL_INT(transaction_current_index(txs[0]), transaction_current_index(test_tx));
  TEST_ASSERT_EQUAL_INT(transaction_last_index(txs[0]), transaction_last_index(test_tx));
  TEST_ASSERT_EQUAL_MEMORY(transaction_hash(txs[0]), transaction_hash(test_tx), FLEX_TRIT_SIZE_243);

  for (int i = 0; i < 5; ++i) {
    transaction_free(pack.models[i]);
  }

  transaction_free(test_tx);
}

void test_stored_milestone(void) {
  iota_milestone_t milestone;
  milestone.index = 42;
  memcpy(milestone.hash, HASH, FLEX_TRIT_SIZE_243);

  TEST_ASSERT(iota_stor_milestone_store(&connection, &milestone) == RC_OK);
  // Test id primary key constraint violation
  TEST_ASSERT(iota_stor_milestone_store(&connection, &milestone) == RC_SQLITE3_FAILED_STEP);
  // Test hash unique constraint violation
  milestone.index++;
  TEST_ASSERT(iota_stor_milestone_store(&connection, &milestone) == RC_SQLITE3_FAILED_STEP);

  // Test get last
  milestone.hash[0]++;
  TEST_ASSERT(iota_stor_milestone_store(&connection, &milestone) == RC_OK);

  DECLARE_PACK_SINGLE_MILESTONE(ms, ms_ptr, ms_pack);

  iota_stor_milestone_load_last(&connection, &ms_pack);
  TEST_ASSERT_EQUAL_INT(1, ms_pack.num_loaded);
  TEST_ASSERT_EQUAL_INT(ms.index, milestone.index);
  TEST_ASSERT_EQUAL_MEMORY(ms.hash, milestone.hash, FLEX_TRIT_SIZE_243);
  milestone.hash[0]--;
  milestone.index--;

  // Test get first
  ms_pack.num_loaded = 0;
  iota_stor_milestone_load_first(&connection, &ms_pack);
  TEST_ASSERT_EQUAL_INT(1, ms_pack.num_loaded);
  TEST_ASSERT_EQUAL_INT(ms.index, milestone.index);
  TEST_ASSERT_EQUAL_MEMORY(ms.hash, milestone.hash, FLEX_TRIT_SIZE_243);

  bool exist = false;
  TEST_ASSERT(iota_stor_milestone_exist(&connection, NULL, &exist) == RC_OK);
  TEST_ASSERT(exist == true);
  TEST_ASSERT(iota_stor_milestone_exist(&connection, HASH, &exist) == RC_OK);
  TEST_ASSERT(exist == true);
  TEST_ASSERT(iota_stor_milestone_exist(&connection, NULL, &exist) == RC_OK);
  TEST_ASSERT(exist == true);

  iota_milestone_t *milestones[5];
  iota_stor_pack_t pack = {
      .models = (void **)milestones, .num_loaded = 0, .capacity = 5, .insufficient_capacity = false};

  for (int i = 0; i < 5; ++i) {
    pack.models[i] = (iota_milestone_t *)malloc(sizeof(iota_milestone_t));
  }
  TEST_ASSERT(iota_stor_milestone_load(&connection, HASH, &pack) == RC_OK);
  TEST_ASSERT_EQUAL_INT(1, pack.num_loaded);
  TEST_ASSERT_EQUAL_INT(milestones[0]->index, 42);
  TEST_ASSERT_EQUAL_MEMORY(milestones[0]->hash, HASH, FLEX_TRIT_SIZE_243);
  for (int i = 0; i < 5; ++i) {
    free(pack.models[i]);
  }
}

void test_stored_load_hashes_by_address(void) {
  flex_trit_t *hashes[5];
  iota_stor_pack_t pack = {.models = (void **)hashes, .capacity = 5, .num_loaded = 0, .insufficient_capacity = false};
  for (size_t i = 0; i < 5; ++i) {
    hashes[i] = (flex_trit_t *)malloc(FLEX_TRIT_SIZE_243);
  }

  flex_trit_t tx_test_trits[FLEX_TRIT_SIZE_8019];
  flex_trits_from_trytes(tx_test_trits, NUM_TRITS_SERIALIZED_TRANSACTION, TEST_TX_TRYTES,
                         NUM_TRITS_SERIALIZED_TRANSACTION, NUM_TRYTES_SERIALIZED_TRANSACTION);
  iota_transaction_t *test_tx = transaction_deserialize(tx_test_trits, true);

  TEST_ASSERT(iota_stor_transaction_load_hashes(&connection, TRANSACTION_FIELD_ADDRESS, transaction_address(test_tx),
                                                &pack) == RC_OK);
  TEST_ASSERT_EQUAL_INT(1, pack.num_loaded);
  TEST_ASSERT_EQUAL_MEMORY(transaction_hash(test_tx), ((flex_trit_t *)pack.models[0]), FLEX_TRIT_SIZE_243);

  for (size_t i = 0; i < 5; ++i) {
    free(hashes[i]);
  }
  transaction_free(test_tx);
}

void test_stored_load_hashes_of_approvers(void) {
  flex_trit_t hashes[5][FLEX_TRIT_SIZE_243];
  iota_stor_pack_t pack = {.models = (void **)hashes, .capacity = 5, .num_loaded = 0, .insufficient_capacity = false};

  flex_trit_t tx_test_trits[FLEX_TRIT_SIZE_8019];
  flex_trits_from_trytes(tx_test_trits, NUM_TRITS_SERIALIZED_TRANSACTION, TEST_TX_TRYTES,
                         NUM_TRITS_SERIALIZED_TRANSACTION, NUM_TRYTES_SERIALIZED_TRANSACTION);
  iota_transaction_t *test_tx = transaction_deserialize(tx_test_trits, true);

  TEST_ASSERT(iota_stor_transaction_load_hashes_of_approvers(&connection, transaction_address(test_tx), &pack, 0) ==
              RC_OK);
  TEST_ASSERT_EQUAL_INT(0, pack.num_loaded);

  transaction_free(test_tx);
}

void test_transaction_update_snapshot_index(void) {
  flex_trit_t tx_test_trits[FLEX_TRIT_SIZE_8019];
  flex_trits_from_trytes(tx_test_trits, NUM_TRITS_SERIALIZED_TRANSACTION, TEST_TX_TRYTES,
                         NUM_TRITS_SERIALIZED_TRANSACTION, NUM_TRYTES_SERIALIZED_TRANSACTION);
  iota_transaction_t *test_tx = transaction_deserialize(tx_test_trits, true);
  DECLARE_PACK_SINGLE_TX(tx, tx_ptr, pack);

  TEST_ASSERT(iota_stor_transaction_load(&connection, TRANSACTION_FIELD_HASH, transaction_hash(test_tx), &pack) ==
              RC_OK);
  TEST_ASSERT_EQUAL_INT(1, pack.num_loaded);
  TEST_ASSERT(iota_stor_transaction_update_snapshot_index(&connection, transaction_hash(test_tx), 123456) == RC_OK);
  hash_pack_reset(&pack);
  TEST_ASSERT(iota_stor_transaction_load_metadata(&connection, transaction_hash(test_tx), &pack) == RC_OK);
  TEST_ASSERT_EQUAL_INT(1, pack.num_loaded);
  TEST_ASSERT_EQUAL_INT(tx.metadata.snapshot_index, 123456);
  transaction_free(test_tx);
}

void test_milestone_state_delta(void) {
  state_delta_t state_delta1 = NULL, state_delta2 = NULL;
  state_delta_entry_t *iter = NULL, *tmp = NULL;
  trit_t trits[HASH_LENGTH] = {1};
  flex_trit_t hash[FLEX_TRIT_SIZE_243];
  flex_trit_t *hashed_hash;

  flex_trits_from_trits(hash, HASH_LENGTH, trits, HASH_LENGTH, HASH_LENGTH);
  for (int64_t i = -1000; i <= 1000; i++) {
    hashed_hash = iota_flex_digest(hash, HASH_LENGTH);
    memcpy(hash, hashed_hash, FLEX_TRIT_SIZE_243);
    free(hashed_hash);
    TEST_ASSERT(state_delta_add(&state_delta1, hash, i) == RC_OK);
  }

  TEST_ASSERT(iota_stor_state_delta_store(&connection, 42, &state_delta1) == RC_OK);

  TEST_ASSERT(iota_stor_state_delta_load(&connection, 43, &state_delta2) == RC_OK);
  TEST_ASSERT(state_delta2 == NULL);

  TEST_ASSERT(iota_stor_state_delta_load(&connection, 42, &state_delta2) == RC_OK);
  TEST_ASSERT(state_delta2 != NULL);

  int i = -1000;
  flex_trits_from_trits(hash, HASH_LENGTH, trits, HASH_LENGTH, HASH_LENGTH);
  iter = NULL;
  HASH_ITER(hh, state_delta2, iter, tmp) {
    hashed_hash = iota_flex_digest(hash, HASH_LENGTH);
    memcpy(hash, hashed_hash, FLEX_TRIT_SIZE_243);
    free(hashed_hash);
    TEST_ASSERT_EQUAL_MEMORY(iter->hash, hash, FLEX_TRIT_SIZE_243);
    TEST_ASSERT_EQUAL_INT64(iter->value, i);
    i++;
  }

  state_delta_destroy(&state_delta1);
  state_delta_destroy(&state_delta2);
}

void test_transaction_update_solid_state(void) {
  flex_trit_t tx_test_trits[FLEX_TRIT_SIZE_8019];
  flex_trits_from_trytes(tx_test_trits, NUM_TRITS_SERIALIZED_TRANSACTION, TEST_TX_TRYTES,
                         NUM_TRITS_SERIALIZED_TRANSACTION, NUM_TRYTES_SERIALIZED_TRANSACTION);
  iota_transaction_t *test_tx = transaction_deserialize(tx_test_trits, true);
  DECLARE_PACK_SINGLE_TX(tx, tx_ptr, pack);

  TEST_ASSERT(iota_stor_transaction_load(&connection, TRANSACTION_FIELD_HASH, transaction_hash(test_tx), &pack) ==
              RC_OK);
  TEST_ASSERT_EQUAL_INT(1, pack.num_loaded);
  TEST_ASSERT(iota_stor_transaction_update_solid_state(&connection, transaction_hash(test_tx), true) == RC_OK);
  hash_pack_reset(&pack);
  TEST_ASSERT(iota_stor_transaction_load_metadata(&connection, transaction_hash(test_tx), &pack) == RC_OK);
  TEST_ASSERT_EQUAL_INT(1, pack.num_loaded);
  TEST_ASSERT(tx.metadata.solid == true);
  transaction_free(test_tx);
}

void test_transactions_update_solid_states_one_transaction(void) {
  flex_trit_t tx_test_trits[FLEX_TRIT_SIZE_8019];
  flex_trits_from_trytes(tx_test_trits, NUM_TRITS_SERIALIZED_TRANSACTION, TEST_TX_TRYTES,
                         NUM_TRITS_SERIALIZED_TRANSACTION, NUM_TRYTES_SERIALIZED_TRANSACTION);
  iota_transaction_t *test_tx = transaction_deserialize(tx_test_trits, true);
  DECLARE_PACK_SINGLE_TX(tx, tx_ptr, pack);

  hash243_set_t hashes = NULL;
  hash243_set_add(&hashes, transaction_hash(test_tx));
  TEST_ASSERT(iota_stor_transactions_update_solid_state(&connection, hashes, true) == RC_OK);
  hash_pack_reset(&pack);
  TEST_ASSERT(iota_stor_transaction_load_metadata(&connection, transaction_hash(test_tx), &pack) == RC_OK);
  TEST_ASSERT_EQUAL_INT(1, pack.num_loaded);
  TEST_ASSERT(transaction_solid(&tx));
  hash243_set_free(&hashes);
  transaction_free(test_tx);
}

void test_transactions_update_solid_states_two_transaction(void) {
  flex_trit_t tx_test_trits[FLEX_TRIT_SIZE_8019];
  flex_trits_from_trytes(tx_test_trits, NUM_TRITS_SERIALIZED_TRANSACTION, TEST_TX_TRYTES,
                         NUM_TRITS_SERIALIZED_TRANSACTION, NUM_TRYTES_SERIALIZED_TRANSACTION);
  iota_transaction_t *test_tx = transaction_deserialize(tx_test_trits, true);
  DECLARE_PACK_SINGLE_TX(tx, tx_ptr, pack);

  iota_transaction_t second_test_transaction = *test_tx;
  // Make them distinguishable
  trit_t modified_trit = flex_trits_at(transaction_hash(test_tx), FLEX_TRIT_SIZE_243, 0);
  if (abs(modified_trit) > 0) {
    modified_trit = 0;
  } else {
    modified_trit = 1;
  }
  flex_trits_set_at(second_test_transaction.consensus.hash, FLEX_TRIT_SIZE_243, 0, modified_trit);

  TEST_ASSERT(iota_stor_transaction_store(&connection, &second_test_transaction) == RC_OK);

  hash243_set_t hashes = NULL;
  hash243_set_add(&hashes, transaction_hash(test_tx));
  hash243_set_add(&hashes, transaction_hash(&second_test_transaction));
  TEST_ASSERT(iota_stor_transactions_update_solid_state(&connection, hashes, true) == RC_OK);
  hash_pack_reset(&pack);
  TEST_ASSERT(iota_stor_transaction_load_metadata(&connection, transaction_hash(test_tx), &pack) == RC_OK);
  TEST_ASSERT_EQUAL_INT(1, pack.num_loaded);
  TEST_ASSERT(transaction_solid(&tx));

  hash_pack_reset(&pack);
  TEST_ASSERT(iota_stor_transaction_load_metadata(&connection, transaction_hash(&second_test_transaction), &pack) ==
              RC_OK);
  TEST_ASSERT_EQUAL_INT(1, pack.num_loaded);
  TEST_ASSERT(transaction_solid(&tx));
  hash243_set_free(&hashes);
  transaction_free(test_tx);
}

void test_transactions_arrival_time(void) {
  flex_trit_t tx_test_trits[FLEX_TRIT_SIZE_8019];
  flex_trits_from_trytes(tx_test_trits, NUM_TRITS_SERIALIZED_TRANSACTION, TEST_TX_TRYTES,
                         NUM_TRITS_SERIALIZED_TRANSACTION, NUM_TRYTES_SERIALIZED_TRANSACTION);
  iota_transaction_t *test_tx = transaction_deserialize(tx_test_trits, true);
  DECLARE_PACK_SINGLE_TX(tx, tx_ptr, pack);

  iota_transaction_t second_test_transaction = *test_tx;
  // Make them distinguishable
  trit_t modified_trit = flex_trits_at(transaction_hash(test_tx), FLEX_TRIT_SIZE_243, 2);
  if (abs(modified_trit) > 0) {
    modified_trit = 0;
  } else {
    modified_trit = 1;
  }
  flex_trits_set_at(second_test_transaction.consensus.hash, FLEX_TRIT_SIZE_243, 2, modified_trit);
  TEST_ASSERT(iota_stor_transaction_store(&connection, &second_test_transaction) == RC_OK);
  hash_pack_reset(&pack);
  TEST_ASSERT(iota_stor_transaction_load_metadata(&connection, transaction_hash(test_tx), &pack) == RC_OK);
  TEST_ASSERT_EQUAL_INT(1, pack.num_loaded);
  int64_t test_tx_timestamp = transaction_arrival_timestamp(&tx);
  hash_pack_reset(&pack);
  TEST_ASSERT(iota_stor_transaction_load_metadata(&connection, transaction_hash(&second_test_transaction), &pack) ==
              RC_OK);
  TEST_ASSERT_EQUAL_INT(1, pack.num_loaded);
  TEST_ASSERT(transaction_arrival_timestamp(&tx) > test_tx_timestamp);
  transaction_free(test_tx);
}

int main(int argc, char *argv[]) {
  UNITY_BEGIN();
  TEST_ASSERT(storage_init() == RC_OK);

  if (argc >= 2) {
    debug_mode = true;
  }
  if (debug_mode) {
    test_db_path = "test.db";
    ciri_db_path = "ciri.db";
  }

  copy_file(test_db_path, ciri_db_path);

  RUN_TEST(test_init_connection);
  RUN_TEST(test_initialized_db_empty_transaction);
  RUN_TEST(test_initialized_db_empty_milestone);
  RUN_TEST(test_stored_transaction);
  RUN_TEST(test_stored_milestone);
  RUN_TEST(test_stored_load_hashes_by_address);
  RUN_TEST(test_stored_load_hashes_of_approvers);
  RUN_TEST(test_milestone_state_delta);
  RUN_TEST(test_transaction_update_snapshot_index);
  RUN_TEST(test_transaction_update_solid_state);
  RUN_TEST(test_transactions_update_solid_states_one_transaction);
  RUN_TEST(test_transactions_update_solid_states_two_transaction);
  RUN_TEST(test_transactions_arrival_time);
  RUN_TEST(test_destroy_connection);

  TEST_ASSERT(storage_destroy() == RC_OK);
  return UNITY_END();
}
