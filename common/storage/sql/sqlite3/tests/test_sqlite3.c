/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://gitlab.com/iota-foundation/software/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdlib.h>
#include <string.h>
#include <unity/unity.h>

#include "common/helpers/files.h"
#include "common/storage/connection.h"
#include "common/storage/sql/defs.h"
#include "common/storage/storage.h"
#include "common/storage/tests/helpers/defs.h"

static connection_t conn;

void test_init_connection(void) {
  connection_config_t config;
  config.db_path = "common/storage/sql/sqlite3/tests/test.db";
  config.index_address = true;
  config.index_approvee = true;
  config.index_bundle = true;
  config.index_tag = true;
  config.index_hash = true;
  TEST_ASSERT(init_connection(&conn, &config) == RC_OK);
}

void test_initialized_db_empty(void) {
  bool exist = false;

  trit_array_p  col_value = trit_array_new(FLEX_TRIT_SIZE_243);
  col_value->trits = "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA";
  TEST_ASSERT(iota_stor_exist(&conn, COL_HASH,col_value , &exist) == RC_OK);
  TEST_ASSERT(exist == false);
}

void test_stored_transaction(void) {
  TEST_ASSERT(iota_stor_store(&conn, &TEST_TRANSACTION) == RC_OK);
  bool exist = false;
  trit_array_p  col_value = trit_array_new(FLEX_TRIT_SIZE_243);
  col_value->trits = TEST_TRANSACTION.hash;
  TEST_ASSERT(iota_stor_exist(&conn, COL_HASH, col_value, &exist) == RC_OK);
  TEST_ASSERT(exist == true);

  TEST_ASSERT(iota_stor_exist(&conn, NULL, NULL, &exist) == RC_OK);
  TEST_ASSERT(exist == true);

  iota_transactions_pack pack;
  iota_transaction_t txs[5];
  pack.txs = &txs;
  pack.num_loaded = 0;
  pack.txs_capacity = 5;

  for (int i = 0; i < 5; ++i) {
    pack.txs[i] = transaction_new();
  }

  size_t num_loaded;
  TEST_ASSERT(iota_stor_load(&conn, COL_HASH, col_value, &pack) == RC_OK);
  TEST_ASSERT_EQUAL_INT(1, pack.num_loaded);

  TEST_ASSERT_EQUAL_STRING(txs[0]->nonce, TEST_TRANSACTION.nonce);
  TEST_ASSERT_EQUAL_STRING(txs[0]->signature_or_message,
                           TEST_TRANSACTION.signature_or_message);
  TEST_ASSERT_EQUAL_STRING(txs[0]->address, TEST_TRANSACTION.address);
  TEST_ASSERT_EQUAL_STRING(txs[0]->branch, TEST_TRANSACTION.branch);
  TEST_ASSERT_EQUAL_STRING(txs[0]->trunk, TEST_TRANSACTION.trunk);
  TEST_ASSERT_EQUAL_STRING(txs[0]->bundle, TEST_TRANSACTION.bundle);
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
  TEST_ASSERT_EQUAL_STRING(txs[0]->hash, TEST_TRANSACTION.hash);

  for (int i = 0; i < 5; ++i) {
    transaction_free(pack.txs[i]);
  }
}

void test_stored_load_hashes_by_address(void) {
  trit_array_p hashes[5];
  iota_hashes_pack pack;
  pack.hashes = &hashes;
  pack.num_loaded = 0;
  pack.hashes_capacity = 5;
  for (int i = 0; i < pack.hashes_capacity; ++i) {
    pack.hashes[i] = trit_array_new(243);
  }
  pack.hashes_capacity = 5;
  trit_array_p key = trit_array_new(81);
  memcpy(key->trits, TEST_TRANSACTION.address, 81);
  TEST_ASSERT(iota_stor_load_hashes(&conn, COL_ADDRESS, key, &pack) == RC_OK);
  TEST_ASSERT_EQUAL_INT(1, pack.num_loaded);
  TEST_ASSERT_EQUAL_MEMORY(TEST_TRANSACTION.hash, pack.hashes[0]->trits, 81);

  for (int i = 0; i < pack.hashes_capacity; ++i) {
    trit_array_free(pack.hashes[i]);
  }
}

void test_stored_load_hashes_of_approvers(void) {
  trit_array_p hashes[5];
  iota_hashes_pack pack;
  pack.hashes = &hashes;
  pack.num_loaded = 0;
  pack.hashes_capacity = 5;
  for (int i = 0; i < pack.hashes_capacity; ++i) {
    pack.hashes[i] = trit_array_new(243);
  }
  pack.hashes_capacity = 5;
  trit_array_p key = trit_array_new(81);
  memcpy(key->trits, TEST_TRANSACTION.address, 81);
  TEST_ASSERT(iota_stor_load_hashes_approvers(&conn, key, &pack) == RC_OK);
  TEST_ASSERT_EQUAL_INT(0, pack.num_loaded);

  for (int i = 0; i < pack.hashes_capacity; ++i) {
    trit_array_free(pack.hashes[i]);
  }
  //TODO - complete the test with 2 transaction's that one approves the other
}

int main(void) {
  UNITY_BEGIN();

  copy_file("common/storage/sql/sqlite3/tests/test.db",
            "common/storage/sql/sqlite3/tests/ciri.db");

  RUN_TEST(test_init_connection);
  RUN_TEST(test_initialized_db_empty);
  RUN_TEST(test_stored_transaction);
  RUN_TEST(test_stored_load_hashes_by_address);
  RUN_TEST(test_stored_load_hashes_of_approvers);

  return UNITY_END();
}
