/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdint.h>
#include <string>

#include "common/storage/sql/defs.h"
#include "common/storage/sql/statements.h"
#include "common/storage/storage.h"

#include <glog/logging.h>
#include <sqlite3.h>

retcode_t iota_stor_store(const connection_t* const conn,
                          const iota_transaction_t data_in) {
  // TODO - input more reasonable size
  char statement[8192];
  iota_transactions_insert_statement(data_in, statement);

  char* err_msg = 0;
  int rc = sqlite3_exec((sqlite3*)conn->db, statement, 0, 0, &err_msg);

  if (rc != SQLITE_OK) {
    LOG(ERROR) << "SQL error: " << err_msg;
    sqlite3_free(err_msg);
    return RC_SQLITE3_FAILED_INSERT_DB;
  }

  return RC_OK;
}
retcode_t iota_stor_load(const connection_t* const conn, const char* index_name,
                         const trit_array_p key, iota_transaction_t data_out) {}
retcode_t iota_stor_exist(const connection_t* const conn,
                          const char* index_name, const trit_array_p key,
                          bool* exist) {}
retcode_t iota_stor_find(const connection_t* const conn, const char* index_name,
                         const trit_array_p key, bool* found,
                         iota_transaction_t data_out) {}
retcode_t iota_stor_update(const connection_t* const conn,
                           const char* index_name, const trit_array_p key,
                           const iota_transaction_t data_in) {}
