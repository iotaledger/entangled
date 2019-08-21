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
