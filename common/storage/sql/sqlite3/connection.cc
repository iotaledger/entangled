/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "common/storage/connection.h"

#include <gflags/gflags.h>
#include <glog/logging.h>
#include <string>

#include <sqlite3.h>

#define TRANSACTION_TABLE_NAME "iota_transaction"

retcode_t create_index(const connection_t* const conn, const std::string& indexName, const std::string& colName){

  char *errMsg = 0;
  std::string  createIndexStatement = std::string("CREATE UNIQUE INDEX IF NOT EXISTS ") + indexName + " ON " + TRANSACTION_TABLE_NAME + "(" + colName + ")";

  int rc = sqlite3_exec((sqlite3*)conn->db, createIndexStatement.c_str(), 0, 0, &errMsg);
  if( rc!=SQLITE_OK ){
    LOG(ERROR)<<"SQL error: "<< errMsg;
    sqlite3_free(errMsg);
    //TODO - return appropriate error
    return RC_UNKNOWN;
  }

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
    LOG(FATAL) << __FUNCTION__ << " Can't open database: \n"
               << sqlite3_errmsg((sqlite3*)conn->db);
  } else {
    LOG(INFO) << "Opened database successfully";
  }

  if (config->indexApprovee) {
    if (retcode = create_index(conn, "trunk_index", "trunk")){
      return retcode;
    }
    if (create_index(conn, "branch_index", "branch")){
      return retcode;
    }
  }

  if (config->indexAddress){
    if (retcode = create_index(conn, "address_index", "address")){
      return retcode;
    }
  }

  if (config->indexBundle){
    if (retcode = create_index(conn, "bundle_index", "bundle")){
      return retcode;
    }
  }

  if (config->indexTag){
    if (retcode = create_index(conn, "tag_index", "tag")){
      return retcode;
    }
  }

  return retcode;
}

retcode_t destroy_connection(connection_t* const conn) {
  sqlite3_close((sqlite3*)conn->db);
}
