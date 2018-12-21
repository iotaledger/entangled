/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <sqlite3.h>

#include "common/storage/connection.h"
#include "common/storage/defs.h"
#include "common/storage/sql/defs.h"
#include "utils/logger_helper.h"

#define SQLITE3_LOGGER_ID "sqlite3"

retcode_t init_connection(connection_t const* const conn,
                          connection_config_t const* const config) {
  char* err_msg = NULL;
  char* sql = NULL;
  int rc = 0;

  if (config->db_path == NULL) {
    log_critical(SQLITE3_LOGGER_ID, "No path for db specified\n");
    return RC_SQLITE3_NO_PATH_FOR_DB_SPECIFIED;
  } else {
    rc = sqlite3_open_v2(config->db_path, (sqlite3**)&conn->db,
                         SQLITE_OPEN_READWRITE, NULL);
  }

  if (rc) {
    log_critical(SQLITE3_LOGGER_ID, "Failed to open db on path: %s\n",
                 config->db_path);
    return RC_SQLITE3_FAILED_OPEN_DB;
  } else {
    log_info(SQLITE3_LOGGER_ID, "Connection to database %s created\n",
             config->db_path);
  }

  if ((rc = sqlite3_busy_timeout((sqlite3*)conn->db, 500)) != SQLITE_OK) {
    return RC_SQLITE3_FAILED_CONFIG;
  }

  sql = "PRAGMA journal_mode = WAL";
  rc = sqlite3_exec((sqlite3*)conn->db, sql, 0, 0, &err_msg);

  if (rc != SQLITE_OK) {
    sqlite3_free(err_msg);
    return RC_SQLITE3_FAILED_INSERT_DB;
  }

  sql = "PRAGMA foreign_keys = ON";
  rc = sqlite3_exec((sqlite3*)conn->db, sql, 0, 0, &err_msg);

  if (rc != SQLITE_OK) {
    sqlite3_free(err_msg);
    return RC_SQLITE3_FAILED_INSERT_DB;
  }

  log_info(SQLITE3_LOGGER_ID, "Connection to database %s initialized\n",
           config->db_path);

  return RC_OK;
}

retcode_t destroy_connection(connection_t* const conn) {
  if (conn->db != NULL) {
    log_info(SQLITE3_LOGGER_ID, "Destroying connection\n");
    sqlite3_close((sqlite3*)conn->db);
    conn->db = NULL;
  }
  return RC_OK;
}
