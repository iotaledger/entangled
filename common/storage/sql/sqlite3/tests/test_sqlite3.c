/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://gitlab.com/iota-foundation/software/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdlib.h>
#include <string.h>
#include <unity/unity.h>

#include "common/storage/connection.h"
#include "common/storage/sql/defs.h"
#include "common/storage/storage.h"
#include "common/storage/tests/helpers/defs.h"
#include "utils/files.h"

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
  TEST_ASSERT(iota_stor_exist(&conn, NULL, NULL, &exist) == RC_OK);
  TEST_ASSERT(exist == false);
}

void test_stored_transaction(void) {
  TEST_ASSERT(iota_stor_store(&conn, &TEST_TRANSACTION) == RC_OK);
  bool exist = false;
  TEST_ASSERT(iota_stor_exist(&conn, NULL, NULL, &exist) == RC_OK);
  TEST_ASSERT(exist == true);

  iota_transactions_pack pack;
  iota_transaction_t txs[5];
  pack.txs = &txs;
  pack.num_loaded = 0;
  pack.max_txs = 5;

  for (int i = 0; i < 5; ++i) {
    pack.txs[i] = transaction_new();
  }

  size_t num_loaded;
  TEST_ASSERT(iota_stor_load(&conn, NULL, NULL, &pack) == RC_OK);
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
  pack.max_hashes = 5;
  for (int i = 0; i < pack.max_hashes; ++i) {
    pack.hashes[i] = trit_array_new(243);
  }
  pack.max_hashes = 5;
  trit_array_p key = trit_array_new(243);
  memcpy(key->trits, TEST_TRANSACTION.address, FLEX_TRIT_SIZE_243);
  TEST_ASSERT(iota_stor_load_hashes(&conn, COL_ADDRESS, key, &pack) == RC_OK);
  TEST_ASSERT_EQUAL_INT(1, pack.num_loaded);
  TEST_ASSERT_EQUAL_MEMORY(TEST_TRANSACTION.hash, pack.hashes[0]->trits, 81);

  for (int i = 0; i < pack.max_hashes; ++i) {
    trit_array_free(pack.hashes[i]);
  }
}

int main(void) {
  UNITY_BEGIN();

  copy_file("common/storage/sql/sqlite3/tests/test.db",
            "common/storage/sql/sqlite3/tests/ciri.db");

  RUN_TEST(test_init_connection);
  RUN_TEST(test_initialized_db_empty);
  RUN_TEST(test_stored_transaction);
  RUN_TEST(test_stored_load_hashes_by_address);
  // TODO - Add test to find transaction by any column name (When #2 is merged)

  return UNITY_END();
}
