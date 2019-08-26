/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "common/storage/sql/mariadb/wrappers.h"

retcode_t prepare_statement(MYSQL* const db, MYSQL_STMT** const mariadb_statement, char const* const statement) {
  if (db == NULL || mariadb_statement == NULL || statement == NULL) {
    return RC_NULL_PARAM;
  }

  if ((*mariadb_statement = mysql_stmt_init(db)) == NULL) {
    return RC_STORAGE_FAILED_PREPARED_STATEMENT;
  }

  if (mysql_stmt_prepare(*mariadb_statement, statement, -1) != 0) {
    return RC_STORAGE_FAILED_PREPARED_STATEMENT;
  }

  return RC_OK;
}

retcode_t finalize_statement(MYSQL_STMT* const mariadb_statement) {
  if (mariadb_statement == NULL) {
    return RC_NULL_PARAM;
  }

  if (mysql_stmt_close(mariadb_statement) != 0) {
    return RC_STORAGE_FAILED_FINALIZE;
  }

  return RC_OK;
}

retcode_t start_transaction(MYSQL* const db) {
  if (mysql_query(db, "START TRANSACTION") != 0) {
    return RC_STORAGE_FAILED_EXECUTE;
  }

  return RC_OK;
}

retcode_t commit_transaction(MYSQL* const db) {
  if (mysql_query(db, "COMMIT") != 0) {
    return RC_STORAGE_FAILED_EXECUTE;
  }

  return RC_OK;
}

retcode_t rollback_transaction(MYSQL* const db) {
  if (mysql_query(db, "ROLLBACK") != 0) {
    return RC_STORAGE_FAILED_EXECUTE;
  }

  return RC_OK;
}

void column_compress_bind(MYSQL_BIND* const bind, size_t const index, void const* const data,
                          enum enum_field_types const type, size_t const num_bytes) {
  ssize_t i = num_bytes - 1;

  bind[index].buffer = (void*)data;
  bind[index].buffer_type = type;
  if (type == MYSQL_TYPE_BLOB) {
    for (; i >= 0 && ((flex_trit_t*)data)[i] == FLEX_TRIT_NULL_VALUE; --i) {
    }
    bind[index].buffer_length = i + 1;
  }
  bind[index].is_null = 0;
}
