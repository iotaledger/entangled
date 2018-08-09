/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "common/storage/connection.h"
#include "common/storage/sql/defs.h"
#include "utils/logger_helper.h"

#include <sqlite3.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define CONNECTION_LOGGER_ID "stor_sqlite3_conn"

retcode_t create_index_if_not_exists(const connection_t* const conn,
                                     const char* const table_name,
                                     const char* const indexName,
                                     const char* const colName) {
  char* errMsg = 0;
  logger_id_t id = logger_id_unknown;

  char statement[MAX_CREATE_INDEX_STATEMENT_SIZE];

  int res = snprintf(statement, MAX_CREATE_INDEX_STATEMENT_SIZE,
                     "CREATE INDEX IF NOT EXISTS %s ON %s(%s)", indexName,
                     TRANSACTION_TABLE_NAME, colName);

  if (res < 0 || res == MAX_CREATE_INDEX_STATEMENT_SIZE) {
    log_error(CONNECTION_LOGGER_ID, "Failed to write statement, statement: %s",
              statement);
    return RC_SQLITE3_FAILED_WRITE_STATEMENT;
  }

  int rc = sqlite3_exec((sqlite3*)conn->db, statement, 0, 0, &errMsg);
  if (rc != SQLITE_OK) {
    log_error(CONNECTION_LOGGER_ID,
              "Failed in creating index, statement: %s, errMsg: %s\n",
              statement, errMsg);
    sqlite3_free(errMsg);
    return RC_SQLITE3_FAILED_CREATE_INDEX_DB;
  }

  log_info(CONNECTION_LOGGER_ID, "created index: %s on column: %s successfully",
           indexName, colName);

  return RC_OK;
}

retcode_t init_connection(connection_t* const conn,
                          const connection_config_t* const config) {
  retcode_t retcode = RC_OK;
  int rc;
  char* err_msg = 0;
  char* sql;

  if (config->db_path == NULL) {
    log_critical(CONNECTION_LOGGER_ID, "No path for db specified");
    return RC_SQLITE3_NO_PATH_FOR_DB_SPECIFIED;
  } else {
    rc = sqlite3_open_v2(config->db_path, (sqlite3**)&conn->db,
                         SQLITE_OPEN_READWRITE, NULL);
  }

  if (rc) {
    log_critical(CONNECTION_LOGGER_ID, "Failed to open db on path: %s",
                 config->db_path);
  } else {
    log_info(CONNECTION_LOGGER_ID,
             "Opened database (from path: %s ) successfully", config->db_path);
  }

  if (config->index_approvee) {
    if (retcode = create_index_if_not_exists(conn, TRANSACTION_TABLE_NAME,
                                             TRUNK_INDEX, COL_TRUNK)) {
      return retcode;
    }
    if (create_index_if_not_exists(conn, TRANSACTION_TABLE_NAME, BRANCH_INDEX,
                                   COL_BRANCH)) {
      return retcode;
    }
  }

  if (config->index_address) {
    if (retcode = create_index_if_not_exists(conn, TRANSACTION_TABLE_NAME,
                                             ADDRESS_INDEX, COL_ADDRESS)) {
      return retcode;
    }
  }

  if (config->index_bundle) {
    if (retcode = create_index_if_not_exists(conn, TRANSACTION_TABLE_NAME,
                                             BUNDLE_INDEX, COL_BUNDLE)) {
      return retcode;
    }
  }

  if (config->index_tag) {
    if (retcode = create_index_if_not_exists(conn, TRANSACTION_TABLE_NAME,
                                             TAG_INDEX, COL_TAG)) {
      return retcode;
    }
  }

  // TODO - implement connections pool so no two threads
  // will access db through same connection simultaneously
  sqlite3_config(SQLITE_CONFIG_MULTITHREAD);
  sqlite3_config(SQLITE_CONFIG_MEMSTATUS, 0);
  sqlite3_busy_timeout((sqlite3*)conn->db, 500);
  sql = "PRAGMA journal_mode = WAL";
  rc = sqlite3_exec((sqlite3*)conn->db, sql, 0, 0, &err_msg);

  if (rc != SQLITE_OK) {
    log_error(CONNECTION_LOGGER_ID, "Failed in statement: %s", sql);
    sqlite3_free(err_msg);
    return RC_SQLITE3_FAILED_INSERT_DB;
  }

  sql = "PRAGMA foreign_keys = ON";
  rc = sqlite3_exec((sqlite3*)conn->db, sql, 0, 0, &err_msg);

  if (rc != SQLITE_OK) {
    log_error(CONNECTION_LOGGER_ID, "Failed in statement: %s", sql);
    sqlite3_free(err_msg);
    return RC_SQLITE3_FAILED_INSERT_DB;
  }

  log_info(CONNECTION_LOGGER_ID, "connection initialised successfully");

  return retcode;
}

retcode_t destroy_connection(connection_t* const conn) {
  log_info(CONNECTION_LOGGER_ID, "Destroying connection");
  sqlite3_close((sqlite3*)conn->db);
}
