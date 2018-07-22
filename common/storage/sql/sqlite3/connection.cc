/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "common/storage/connection.h"
#include "common/storage/sql/defs.h"

#include <sqlite3.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

retcode_t create_index(const connection_t* const conn,
                       const char* const indexName, const char* const colName) {
  char* errMsg = 0;

  char statement[MAX_CREATE_INDEX_STATEMENT_SIZE];

  int res = snprintf(statement, MAX_CREATE_INDEX_STATEMENT_SIZE,
                     "CREATE INDEX IF NOT EXISTS %s ON %s(%s)",
                     TRANSACTION_TABLE_NAME, indexName, colName);

  if (res < 0 || res == MAX_CREATE_INDEX_STATEMENT_SIZE) {
    // TODO - log error
    return RC_SQLITE3_FAILED_WRITE_STATEMENT;
  }

  int rc = sqlite3_exec((sqlite3*)conn->db, statement, 0, 0, &errMsg);
  if (rc != SQLITE_OK) {
    // TODO - log
    sqlite3_free(errMsg);
    return RC_SQLITE3_FAILED_CREATE_INDEX_DB;
  }

  //TODO - implement connections pool so no two threads
  //will access db through same connection simultaneously
  sqlite3_config(SQLITE_CONFIG_MULTITHREAD);
  sqlite3_config(SQLITE_CONFIG_MEMSTATUS, 0);

  return RC_OK;
}

retcode_t init_connection(connection_t* const conn,
                          const connection_config_t* const config) {
  retcode_t retcode = RC_OK;
  int rc;

  if (config->dbPath == "") {
    rc = sqlite3_open(":memory:", (sqlite3**)&conn->db);
  } else {
    rc = sqlite3_open(config->dbPath, (sqlite3**)&conn->db);
  }

  if (rc) {
    // TODO - log
    // LOG(FATAL) << __FUNCTION__ << " Can't open database: \n"
    //          << sqlite3_errmsg((sqlite3*)conn->db);
  } else {
    // TODO - log
    // LOG(INFO) << "Opened database successfully";
  }

  if (config->indexApprovee) {
    if (retcode = create_index(conn, "trunk_index", "trunk")) {
      return retcode;
    }
    if (create_index(conn, "branch_index", "branch")) {
      return retcode;
    }
  }

  if (config->indexAddress) {
    if (retcode = create_index(conn, "address_index", "address")) {
      return retcode;
    }
  }

  if (config->indexBundle) {
    if (retcode = create_index(conn, "bundle_index", "bundle")) {
      return retcode;
    }
  }

  if (config->indexTag) {
    if (retcode = create_index(conn, "tag_index", "tag")) {
      return retcode;
    }
  }

  return retcode;
}

retcode_t destroy_connection(connection_t* const conn) {
  sqlite3_close((sqlite3*)conn->db);
}
