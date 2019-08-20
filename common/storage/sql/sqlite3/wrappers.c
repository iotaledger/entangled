/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "common/storage/sql/sqlite3/wrappers.h"

retcode_t prepare_statement(sqlite3* const db, sqlite3_stmt** const sqlite_statement, char const* const statement) {
  if (sqlite3_prepare_v2(db, statement, -1, sqlite_statement, NULL) != SQLITE_OK) {
    return RC_STORAGE_FAILED_PREPARED_STATEMENT;
  }

  return RC_OK;
}

retcode_t finalize_statement(sqlite3_stmt* const sqlite_statement) {
  if (sqlite_statement == NULL) {
    return RC_OK;
  }

  if (sqlite3_finalize(sqlite_statement) != SQLITE_OK) {
    return RC_STORAGE_FAILED_FINALIZE;
  }

  return RC_OK;
}

retcode_t begin_transaction(sqlite3* const db) {
  if ((sqlite3_exec(db, "BEGIN TRANSACTION;", NULL, NULL, NULL)) != SQLITE_OK) {
    return RC_STORAGE_FAILED_BEGIN;
  }
  return RC_OK;
}

retcode_t end_transaction(sqlite3* const db) {
  if (sqlite3_exec(db, "END TRANSACTION;", NULL, NULL, NULL) != SQLITE_OK) {
    return RC_STORAGE_FAILED_END;
  }
  return RC_OK;
}

retcode_t rollback_transaction(sqlite3* const db) {
  if (sqlite3_exec(db, "ROLLBACK TRANSACTION;", NULL, NULL, NULL) != SQLITE_OK) {
    return RC_STORAGE_FAILED_ROLLBACK;
  }
  return RC_OK;
}

retcode_t column_compress_bind(sqlite3_stmt* const statement, size_t const index, flex_trit_t const* const flex_trits,
                               size_t const num_bytes) {
  ssize_t i = num_bytes - 1;
  int rc = 0;

  for (; i >= 0 && flex_trits[i] == FLEX_TRIT_NULL_VALUE; --i)
    ;
  if ((rc = sqlite3_bind_blob(statement, index, flex_trits, i + 1, NULL)) != SQLITE_OK) {
    return RC_STORAGE_FAILED_BINDING;
  }
  return RC_OK;
}

void column_decompress_load(sqlite3_stmt* const statement, size_t const index, flex_trit_t* const flex_trits,
                            size_t const num_bytes) {
  char const* buffer = NULL;
  size_t column_size = 0;

  if ((buffer = sqlite3_column_blob(statement, index))) {
    column_size = sqlite3_column_bytes(statement, index);
    memcpy(flex_trits, buffer, column_size);
    memset(flex_trits + column_size, FLEX_TRIT_NULL_VALUE, num_bytes - column_size);
  } else {
    memset(flex_trits, FLEX_TRIT_NULL_VALUE, num_bytes);
  }
}
