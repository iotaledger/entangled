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
  TEST_ASSERT(init_connection(&conn, &config) == RC_OK);
}

void initialized_db_empty(void) {
  bool exist = false;
  TEST_ASSERT(iota_stor_exist(&conn, NULL, NULL, &exist) == RC_OK);
  TEST_ASSERT(exist == false);
}

void test_stored_transaction(void) {
  TEST_ASSERT(iota_stor_store(&conn, &TEST_TRANSACTION) == RC_OK);
  bool exist = false;
  TEST_ASSERT(iota_stor_exist(&conn, NULL, NULL, &exist) == RC_OK);
  TEST_ASSERT(exist == true);

  iota_transaction_t txs[5];

  for (int i = 0; i < 5; ++i) {
    txs[i] = transaction_new();
  }

  int num_loaded;
  TEST_ASSERT(iota_stor_load(&conn, NULL, NULL, txs, 5, &num_loaded) == RC_OK);
  TEST_ASSERT_EQUAL_INT(1, num_loaded);

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

  for (int i = 0; i < 5; ++i) {
    transaction_free(txs[i]);
  }
}

int main(void) {
  UNITY_BEGIN();

  copy_file("common/storage/sql/sqlite3/tests/test.db",
            "common/storage/sql/sqlite3/tests/ciri.db");

  RUN_TEST(test_init_connection);
  RUN_TEST(initialized_db_empty);

  return UNITY_END();
}
