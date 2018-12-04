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
#include "common/storage/connection.h"
#include "common/storage/sql/defs.h"
#include "common/storage/storage.h"
#include "common/storage/tests/helpers/defs.h"
#include "utils/containers/hash/hash243_set.h"
#include "utils/files.h"

// TODO Remove after "Common definitions #329" is merged
#define HASH_LENGTH 243

#if defined(FLEX_TRIT_ENCODING_1_TRIT_PER_BYTE)
const flex_trit_t HASH[] = {
    1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1,
    0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0,
    0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0,
    1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1,
    0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0,
    0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0,
    1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1,
    0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0,
    0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0,
    1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0,
};
#elif defined(FLEX_TRIT_ENCODING_3_TRITS_PER_BYTE)
const flex_trit_t HASH[] = {
    65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65,
    65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65,
    65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65,
    65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65,
    65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65,
};
#elif defined(FLEX_TRIT_ENCODING_4_TRITS_PER_BYTE)
const flex_trit_t HASH[] = {
    65, 16, 4, 65, 16, 4, 65, 16, 4, 65, 16, 4, 65, 16, 4, 65, 16, 4, 65, 16, 4,
    65, 16, 4, 65, 16, 4, 65, 16, 4, 65, 16, 4, 65, 16, 4, 65, 16, 4, 65, 16, 4,
    65, 16, 4, 65, 16, 4, 65, 16, 4, 65, 16, 4, 65, 16, 4, 65, 16, 4, 1,
};
#elif defined(FLEX_TRIT_ENCODING_5_TRITS_PER_BYTE)
const flex_trit_t HASH[] = {
    28, 84, 9,  28, 84, 9,  28, 84, 9,  28, 84, 9,  28, 84, 9,  28, 84,
    9,  28, 84, 9,  28, 84, 9,  28, 84, 9,  28, 84, 9,  28, 84, 9,  28,
    84, 9,  28, 84, 9,  28, 84, 9,  28, 84, 9,  28, 84, 9,  1,
};
#endif

static connection_t conn;

void test_init_connection(void) {
  connection_config_t config;
  config.db_path = "common/storage/sql/sqlite3/tests/test.db";
  TEST_ASSERT(init_connection(&conn, &config) == RC_OK);
}

void test_initialized_db_empty_transaction(void) {
  bool exist = false;

  TEST_ASSERT(iota_stor_milestone_exist(&conn, HASH, &exist) == RC_OK);
  TEST_ASSERT(exist == false);
}

void test_initialized_db_empty_milestone(void) {
  bool exist = false;

  TRIT_ARRAY_DECLARE(hash, NUM_TRITS_HASH);
  memcpy(hash.trits, HASH, FLEX_TRIT_SIZE_243);
  TEST_ASSERT(iota_stor_transaction_exist(&conn, TRANSACTION_FIELD_HASH, &hash,
                                          &exist) == RC_OK);
  TEST_ASSERT(exist == false);
}

void test_stored_transaction(void) {
  TEST_ASSERT(iota_stor_transaction_store(
                  &conn, (iota_transaction_t)&TEST_TRANSACTION) == RC_OK);
  // Test primary key constraint violation
  TEST_ASSERT(iota_stor_transaction_store(
                  &conn, (iota_transaction_t)&TEST_TRANSACTION) ==
              RC_SQLITE3_FAILED_STEP);
  bool exist = false;

  TRIT_ARRAY_DECLARE(hash, NUM_TRITS_HASH);
  memcpy(hash.trits, TEST_TRANSACTION.hash, FLEX_TRIT_SIZE_243);
  TEST_ASSERT(iota_stor_transaction_exist(&conn, TRANSACTION_FIELD_NONE, NULL,
                                          &exist) == RC_OK);
  TEST_ASSERT(exist == true);
  TEST_ASSERT(iota_stor_transaction_exist(&conn, TRANSACTION_FIELD_HASH, &hash,
                                          &exist) == RC_OK);
  TEST_ASSERT(exist == true);

  TEST_ASSERT(iota_stor_transaction_exist(&conn, TRANSACTION_FIELD_NONE, NULL,
                                          &exist) == RC_OK);
  TEST_ASSERT(exist == true);

  iota_transaction_t txs[5];
  iota_stor_pack_t pack = {.models = (void **)txs,
                           .capacity = 5,
                           .num_loaded = 0,
                           .insufficient_capacity = false};

  for (int i = 0; i < 5; ++i) {
    pack.models[i] = transaction_new();
  }

  TEST_ASSERT(iota_stor_transaction_load(&conn, TRANSACTION_FIELD_HASH, &hash,
                                         &pack) == RC_OK);
  TEST_ASSERT_EQUAL_INT(1, pack.num_loaded);

  TEST_ASSERT_EQUAL_MEMORY(txs[0]->nonce, TEST_TRANSACTION.nonce,
                           FLEX_TRIT_SIZE_81);
  TEST_ASSERT_EQUAL_MEMORY(txs[0]->signature_or_message,
                           TEST_TRANSACTION.signature_or_message,
                           FLEX_TRIT_SIZE_6561);
  TEST_ASSERT_EQUAL_MEMORY(txs[0]->address, TEST_TRANSACTION.address,
                           FLEX_TRIT_SIZE_243);
  TEST_ASSERT_EQUAL_MEMORY(txs[0]->branch, TEST_TRANSACTION.branch,
                           FLEX_TRIT_SIZE_243);
  TEST_ASSERT_EQUAL_MEMORY(txs[0]->trunk, TEST_TRANSACTION.trunk,
                           FLEX_TRIT_SIZE_243);
  TEST_ASSERT_EQUAL_MEMORY(txs[0]->bundle, TEST_TRANSACTION.bundle,
                           FLEX_TRIT_SIZE_243);
  TEST_ASSERT_EQUAL_INT(txs[0]->value, TEST_TRANSACTION.value);
  TEST_ASSERT_EQUAL_INT(txs[0]->attachment_timestamp,
                        TEST_TRANSACTION.attachment_timestamp);
  TEST_ASSERT_EQUAL_INT(txs[0]->attachment_timestamp_upper,
                        TEST_TRANSACTION.attachment_timestamp_upper);
  TEST_ASSERT_EQUAL_INT(txs[0]->attachment_timestamp_lower,
                        TEST_TRANSACTION.attachment_timestamp_lower);
  TEST_ASSERT_EQUAL_INT(txs[0]->timestamp, TEST_TRANSACTION.timestamp);
  TEST_ASSERT_EQUAL_INT(txs[0]->current_index, TEST_TRANSACTION.current_index);
  TEST_ASSERT_EQUAL_INT(txs[0]->last_index, TEST_TRANSACTION.last_index);
  TEST_ASSERT_EQUAL_MEMORY(txs[0]->hash, TEST_TRANSACTION.hash,
                           FLEX_TRIT_SIZE_243);
  TEST_ASSERT_EQUAL_INT(txs[0]->snapshot_index,
                        TEST_TRANSACTION.snapshot_index);

  TEST_ASSERT_EQUAL_INT(txs[0]->solid, TEST_TRANSACTION.solid);

  for (int i = 0; i < 5; ++i) {
    transaction_free(pack.models[i]);
  }
}

void test_stored_milestone(void) {
  iota_milestone_t milestone;
  milestone.index = 42;
  memcpy(milestone.hash, HASH, FLEX_TRIT_SIZE_243);

  TEST_ASSERT(iota_stor_milestone_store(&conn, &milestone) == RC_OK);
  // Test id primary key constraint violation
  TEST_ASSERT(iota_stor_milestone_store(&conn, &milestone) ==
              RC_SQLITE3_FAILED_STEP);
  // Test hash unique constraint violation
  milestone.index++;
  TEST_ASSERT(iota_stor_milestone_store(&conn, &milestone) ==
              RC_SQLITE3_FAILED_STEP);

  // Test get last
  milestone.hash[0]++;
  TEST_ASSERT(iota_stor_milestone_store(&conn, &milestone) == RC_OK);

  DECLARE_PACK_SINGLE_MILESTONE(ms, ms_ptr, ms_pack);

  iota_stor_milestone_load_last(&conn, &ms_pack);
  TEST_ASSERT_EQUAL_INT(1, ms_pack.num_loaded);
  TEST_ASSERT_EQUAL_INT(ms.index, milestone.index);
  TEST_ASSERT_EQUAL_MEMORY(ms.hash, milestone.hash, FLEX_TRIT_SIZE_243);
  milestone.hash[0]--;
  milestone.index--;

  // Test get first
  ms_pack.num_loaded = 0;
  iota_stor_milestone_load_first(&conn, &ms_pack);
  TEST_ASSERT_EQUAL_INT(1, ms_pack.num_loaded);
  TEST_ASSERT_EQUAL_INT(ms.index, milestone.index);
  TEST_ASSERT_EQUAL_MEMORY(ms.hash, milestone.hash, FLEX_TRIT_SIZE_243);

  bool exist = false;
  TEST_ASSERT(iota_stor_milestone_exist(&conn, NULL, &exist) == RC_OK);
  TEST_ASSERT(exist == true);
  TEST_ASSERT(iota_stor_milestone_exist(&conn, HASH, &exist) == RC_OK);
  TEST_ASSERT(exist == true);
  TEST_ASSERT(iota_stor_milestone_exist(&conn, NULL, &exist) == RC_OK);
  TEST_ASSERT(exist == true);

  iota_milestone_t *milestones[5];
  iota_stor_pack_t pack = {.models = (void **)milestones,
                           .num_loaded = 0,
                           .capacity = 5,
                           .insufficient_capacity = false};

  for (int i = 0; i < 5; ++i) {
    pack.models[i] = malloc(sizeof(iota_milestone_t));
  }
  TEST_ASSERT(iota_stor_milestone_load(&conn, HASH, &pack) == RC_OK);
  TEST_ASSERT_EQUAL_INT(1, pack.num_loaded);
  TEST_ASSERT_EQUAL_INT(milestones[0]->index, 42);
  TEST_ASSERT_EQUAL_MEMORY(milestones[0]->hash, HASH, FLEX_TRIT_SIZE_243);
  for (int i = 0; i < 5; ++i) {
    free(pack.models[i]);
  }
}

void test_stored_load_hashes_by_address(void) {
  trit_array_p hashes[5];
  iota_stor_pack_t pack = {.models = (void **)hashes,
                           .capacity = 5,
                           .num_loaded = 0,
                           .insufficient_capacity = false};

  for (int i = 0; i < pack.capacity; ++i) {
    pack.models[i] = trit_array_new(NUM_TRITS_ADDRESS);
  }
  pack.capacity = 5;
  TRIT_ARRAY_DECLARE(key, NUM_TRITS_HASH);
  memcpy(key.trits, TEST_TRANSACTION.address, FLEX_TRIT_SIZE_243);
  TEST_ASSERT(iota_stor_transaction_load_hashes(
                  &conn, TRANSACTION_FIELD_ADDRESS, &key, &pack) == RC_OK);
  TEST_ASSERT_EQUAL_INT(1, pack.num_loaded);
  TEST_ASSERT_EQUAL_MEMORY(TEST_TRANSACTION.hash,
                           ((trit_array_p)pack.models[0])->trits,
                           FLEX_TRIT_SIZE_243);

  for (int i = 0; i < pack.capacity; ++i) {
    trit_array_free(pack.models[i]);
  }
}

void test_stored_load_hashes_of_approvers(void) {
  trit_array_p hashes[5];
  iota_stor_pack_t pack = {.models = (void **)hashes,
                           .capacity = 5,
                           .num_loaded = 0,
                           .insufficient_capacity = false};

  for (int i = 0; i < pack.capacity; ++i) {
    pack.models[i] = trit_array_new(NUM_TRITS_HASH);
  }

  TRIT_ARRAY_DECLARE(key, NUM_TRITS_HASH);
  memcpy(key.trits, TEST_TRANSACTION.address, FLEX_TRIT_SIZE_243);
  TEST_ASSERT(iota_stor_transaction_load_hashes_of_approvers(&conn, key.trits,
                                                             &pack) == RC_OK);
  TEST_ASSERT_EQUAL_INT(0, pack.num_loaded);

  for (int i = 0; i < pack.capacity; ++i) {
    trit_array_free(pack.models[i]);
  }
}

void test_transaction_update_snapshot_index(void) {
  DECLARE_PACK_SINGLE_TX(tx, tx_ptr, pack);
  struct _trit_array hash = {(flex_trit_t *)TEST_TRANSACTION.hash,
                             NUM_TRITS_HASH, FLEX_TRIT_SIZE_243, 0};

  TEST_ASSERT(iota_stor_transaction_load(&conn, TRANSACTION_FIELD_HASH, &hash,
                                         &pack) == RC_OK);
  TEST_ASSERT_EQUAL_INT(1, pack.num_loaded);
  TEST_ASSERT_EQUAL_INT(tx.snapshot_index, TEST_TRANSACTION.snapshot_index);
  TEST_ASSERT(iota_stor_transaction_update_snapshot_index(
                  &conn, (flex_trit_t *)TEST_TRANSACTION.hash, 123456) ==
              RC_OK);
  hash_pack_reset(&pack);
  TEST_ASSERT(iota_stor_transaction_load(&conn, TRANSACTION_FIELD_HASH, &hash,
                                         &pack) == RC_OK);
  TEST_ASSERT_EQUAL_INT(1, pack.num_loaded);
  TEST_ASSERT_EQUAL_INT(tx.snapshot_index, 123456);
}

void test_milestone_state_delta(void) {
  state_delta_t state_delta1 = NULL, state_delta2 = NULL;
  state_delta_entry_t *iter = NULL, *tmp = NULL;
  trit_t trits[HASH_LENGTH] = {1};
  flex_trit_t hash[FLEX_TRIT_SIZE_243];
  flex_trit_t *hashed_hash;

  flex_trits_from_trits(hash, HASH_LENGTH, trits, HASH_LENGTH, HASH_LENGTH);
  for (int i = -1000; i <= 1000; i++) {
    hashed_hash = iota_flex_digest(hash, HASH_LENGTH);
    memcpy(hash, hashed_hash, FLEX_TRIT_SIZE_243);
    free(hashed_hash);
    TEST_ASSERT(state_delta_add(&state_delta1, hash, i) == RC_OK);
  }

  TEST_ASSERT(iota_stor_state_delta_store(&conn, 42, &state_delta1) == RC_OK);

  TEST_ASSERT(iota_stor_state_delta_load(&conn, 43, &state_delta2) == RC_OK);
  TEST_ASSERT(state_delta2 == NULL);

  TEST_ASSERT(iota_stor_state_delta_load(&conn, 42, &state_delta2) == RC_OK);
  TEST_ASSERT(state_delta2 != NULL);

  int i = -1000;
  flex_trits_from_trits(hash, HASH_LENGTH, trits, HASH_LENGTH, HASH_LENGTH);
  iter = NULL;
  HASH_ITER(hh, state_delta2, iter, tmp) {
    hashed_hash = iota_flex_digest(hash, HASH_LENGTH);
    memcpy(hash, hashed_hash, FLEX_TRIT_SIZE_243);
    free(hashed_hash);
    TEST_ASSERT_EQUAL_MEMORY(iter->hash, hash, FLEX_TRIT_SIZE_243);
    TEST_ASSERT_EQUAL_INT(iter->value, i);
    i++;
  }

  state_delta_destroy(&state_delta1);
  state_delta_destroy(&state_delta2);
}

void test_transaction_update_solid_state(void) {
  DECLARE_PACK_SINGLE_TX(tx, tx_ptr, pack);
  struct _trit_array hash = {(flex_trit_t *)TEST_TRANSACTION.hash,
                             NUM_TRITS_HASH, FLEX_TRIT_SIZE_243, 0};

  TEST_ASSERT(iota_stor_transaction_load(&conn, TRANSACTION_FIELD_HASH, &hash,
                                         &pack) == RC_OK);
  TEST_ASSERT_EQUAL_INT(1, pack.num_loaded);
  bool old_solid_tate = tx.solid;
  bool new_solid_state = !old_solid_tate;
  TEST_ASSERT(iota_stor_transaction_update_solid_state(
                  &conn, (flex_trit_t *)TEST_TRANSACTION.hash,
                  new_solid_state) == RC_OK);
  hash_pack_reset(&pack);
  TEST_ASSERT(iota_stor_transaction_load(&conn, TRANSACTION_FIELD_HASH, &hash,
                                         &pack) == RC_OK);
  TEST_ASSERT_EQUAL_INT(1, pack.num_loaded);
  TEST_ASSERT(tx.solid == new_solid_state);
}

void test_transactions_update_solid_states_one_transaction(void) {
  DECLARE_PACK_SINGLE_TX(tx, tx_ptr, pack);
  struct _trit_array hash = {(flex_trit_t *)TEST_TRANSACTION.hash,
                             NUM_TRITS_HASH, FLEX_TRIT_SIZE_243, 0};
  hash243_set_t hashes = NULL;
  hash243_set_add(&hashes, TEST_TRANSACTION.hash);
  TEST_ASSERT(iota_stor_transactions_update_solid_state(&conn, hashes, true) ==
              RC_OK);
  hash_pack_reset(&pack);
  TEST_ASSERT(iota_stor_transaction_load(&conn, TRANSACTION_FIELD_HASH, &hash,
                                         &pack) == RC_OK);
  TEST_ASSERT_EQUAL_INT(1, pack.num_loaded);
  TEST_ASSERT(tx.solid);
  hash243_set_free(&hashes);
}

void test_transactions_update_solid_states_two_transaction(void) {
  DECLARE_PACK_SINGLE_TX(tx, tx_ptr, pack);
  struct _trit_array hash = {(flex_trit_t *)TEST_TRANSACTION.hash,
                             NUM_TRITS_HASH, FLEX_TRIT_SIZE_243, 0};

  struct _iota_transaction second_test_transaction = TEST_TRANSACTION;
  // Make them distinguishable
  second_test_transaction.hash[FLEX_TRIT_SIZE_243] =
      second_test_transaction.hash[0];

  TEST_ASSERT(iota_stor_transaction_store(
                  &conn, (iota_transaction_t)&TEST_TRANSACTION) ==
              RC_SQLITE3_FAILED_STEP);
  bool exist = false;

  TEST_ASSERT(iota_stor_transaction_exist(&conn, TRANSACTION_FIELD_NONE, NULL,
                                          &exist) == RC_OK);
  TEST_ASSERT(exist == true);

  TEST_ASSERT(iota_stor_transaction_update_solid_state(
                  &conn, (flex_trit_t *)TEST_TRANSACTION.hash, false) == RC_OK);

  TEST_ASSERT(iota_stor_transaction_update_solid_state(
                  &conn, (flex_trit_t *)second_test_transaction.hash, false) ==
              RC_OK);

  hash243_set_t hashes = NULL;
  hash243_set_add(&hashes, TEST_TRANSACTION.hash);
  hash243_set_add(&hashes, second_test_transaction.hash);
  TEST_ASSERT(iota_stor_transactions_update_solid_state(&conn, hashes, true) ==
              RC_OK);
  hash_pack_reset(&pack);
  TEST_ASSERT(iota_stor_transaction_load(&conn, TRANSACTION_FIELD_HASH, &hash,
                                         &pack) == RC_OK);
  TEST_ASSERT_EQUAL_INT(1, pack.num_loaded);
  TEST_ASSERT(tx.solid);

  hash_pack_reset(&pack);
  hash.trits = second_test_transaction.hash;
  TEST_ASSERT(iota_stor_transaction_load(&conn, TRANSACTION_FIELD_HASH, &hash,
                                         &pack) == RC_OK);
  TEST_ASSERT_EQUAL_INT(1, pack.num_loaded);
  TEST_ASSERT(tx.solid);
  hash243_set_free(&hashes);
}

int main(void) {
  UNITY_BEGIN();

  copy_file("common/storage/sql/sqlite3/tests/test.db",
            "common/storage/sql/sqlite3/tests/ciri.db");

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

  return UNITY_END();
}
