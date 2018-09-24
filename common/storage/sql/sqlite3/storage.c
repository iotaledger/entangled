/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <assert.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sqlite3.h>

#include "common/model/milestone.h"
#include "common/model/transaction.h"
#include "common/storage/sql/defs.h"
#include "common/storage/sql/statements.h"
#include "common/storage/storage.h"
#include "utils/logger_helper.h"

#define SQLITE3_LOGGER_ID "stor_sqlite3"

static void select_milestones_populate_from_row(sqlite3_stmt* statement,
                                                iota_milestone_t* milestone);

static retcode_t prepare_statement(sqlite3* db, sqlite3_stmt** sqlite_statement,
                                   char* const statement,
                                   char const** const err_msg) {
  int rc = sqlite3_prepare_v2(db, statement, -1, sqlite_statement, err_msg);
  if (rc != SQLITE_OK) {
    log_error(SQLITE3_LOGGER_ID,
              "Failed preparing statement, sqlite3 code is: %" PRIu64 "\n", rc);
    return RC_SQLITE3_FAILED_PREPARED_STATEMENT;
  }

  return RC_OK;
}

retcode_t iota_stor_init(const connection_t* const conn,
                         const connection_config_t* const config) {
  logger_helper_init(SQLITE3_LOGGER_ID, LOGGER_DEBUG, true);
  return init_connection(conn, config);
}

retcode_t iota_stor_destroy(const connection_t* const conn) {
  logger_helper_destroy(SQLITE3_LOGGER_ID);
  return destroy_connection(conn);
}

/*
 * Compression functions
 */

static retcode_t column_compress_bind(sqlite3_stmt* statement, size_t index,
                                      flex_trit_t* flex_trits,
                                      size_t num_bytes) {
  ssize_t i;
  for (i = num_bytes - 1; i >= 0 && flex_trits[i] == FLEX_TRIT_NULL_VALUE; --i)
    ;
  int rc = sqlite3_bind_blob(statement, index, flex_trits, i + 1, NULL);
  if (rc != SQLITE_OK) {
    log_error(SQLITE3_LOGGER_ID,
              "Failed in binding, sqlite3 code is: %" PRIu64 "\n", rc);
    return RC_SQLITE3_FAILED_BINDING;
  }
  return RC_OK;
}

static void column_decompress_load(sqlite3_stmt* statement, size_t index,
                                   flex_trit_t* flex_trits, size_t num_bytes) {
  char const* buffer;
  size_t column_size;

  buffer = sqlite3_column_blob(statement, index);
  if (buffer) {
    column_size = sqlite3_column_bytes(statement, index);
    memcpy(flex_trits, buffer, column_size);
    memset(flex_trits + column_size, FLEX_TRIT_NULL_VALUE,
           num_bytes - column_size);
  } else {
    memset(flex_trits, FLEX_TRIT_NULL_VALUE, num_bytes);
  }
}

/*
 * Transaction operations
 */

static void select_transactions_populate_from_row(sqlite3_stmt* statement,
                                                  iota_transaction_t tx) {
  column_decompress_load(statement, 0, tx->signature_or_message,
                         FLEX_TRIT_SIZE_6561);
  column_decompress_load(statement, 1, tx->address, FLEX_TRIT_SIZE_243);
  tx->value = sqlite3_column_int64(statement, 2);
  column_decompress_load(statement, 3, tx->obsolete_tag, FLEX_TRIT_SIZE_81);
  tx->timestamp = sqlite3_column_int64(statement, 4);
  tx->current_index = sqlite3_column_int64(statement, 5);
  tx->last_index = sqlite3_column_int64(statement, 6);
  column_decompress_load(statement, 7, tx->bundle, FLEX_TRIT_SIZE_243);
  column_decompress_load(statement, 8, tx->trunk, FLEX_TRIT_SIZE_243);
  column_decompress_load(statement, 9, tx->branch, FLEX_TRIT_SIZE_243);
  column_decompress_load(statement, 10, tx->tag, FLEX_TRIT_SIZE_81);
  tx->attachment_timestamp = sqlite3_column_int64(statement, 11);
  tx->attachment_timestamp_upper = sqlite3_column_int64(statement, 12);
  tx->attachment_timestamp_lower = sqlite3_column_int64(statement, 13);
  column_decompress_load(statement, 14, tx->nonce, FLEX_TRIT_SIZE_81);
  column_decompress_load(statement, 15, tx->hash, FLEX_TRIT_SIZE_243);
  tx->snapshot_index = sqlite3_column_int64(statement, 16);
}

enum load_model {
  MODEL_TRANSACTION,
  MODEL_HASH,
  MODEL_MILESTONE,
};

static retcode_t execute_statement_load_gen(sqlite3_stmt* sqlite_statement,
                                            iota_stor_pack_t* pack,
                                            uint32_t max_records,
                                            enum load_model model) {
  pack->insufficient_capacity = false;
  while (sqlite3_step(sqlite_statement) ==
         SQLITE_ROW) {  // While query has result-rows.
    if (pack->num_loaded == pack->capacity || pack->num_loaded == max_records) {
      pack->insufficient_capacity = (pack->num_loaded == pack->capacity);
      break;
    }
    if (model == MODEL_HASH) {
      column_decompress_load(
          sqlite_statement, 0,
          ((trit_array_p)pack->models[pack->num_loaded++])->trits,
          FLEX_TRIT_SIZE_243);
    } else if (model == MODEL_TRANSACTION) {
      select_transactions_populate_from_row(sqlite_statement,
                                            pack->models[pack->num_loaded++]);
    } else if (model == MODEL_MILESTONE) {
      select_milestones_populate_from_row(sqlite_statement,
                                          pack->models[pack->num_loaded++]);
    } else {
      return RC_SQLITE3_FAILED_NOT_IMPLEMENTED;
    }
  }
  int rc =
      sqlite3_finalize(sqlite_statement);  //  Finalize the prepared statement.
  if (rc != SQLITE_OK) {
    log_error(SQLITE3_LOGGER_ID,
              "Failed in finalizing, sqlite3 code is: %" PRIu64 "\n", rc);
    return RC_SQLITE3_FAILED_FINALIZE;
  }

  return RC_OK;
}

static retcode_t execute_statement_load_milestones(
    sqlite3_stmt* sqlite_statement, iota_stor_pack_t* pack,
    uint32_t max_records) {
  return execute_statement_load_gen(sqlite_statement, pack, max_records,
                                    MODEL_MILESTONE);
}

static retcode_t execute_statement_load_transactions(
    sqlite3_stmt* sqlite_statement, iota_stor_pack_t* pack) {
  return execute_statement_load_gen(sqlite_statement, pack, pack->capacity,
                                    MODEL_TRANSACTION);
}

static retcode_t execute_statement_load_hashes(sqlite3_stmt* sqlite_statement,
                                               iota_stor_pack_t* pack) {
  return execute_statement_load_gen(sqlite_statement, pack, pack->capacity,
                                    MODEL_HASH);
}

static retcode_t execute_statement_store(sqlite3_stmt* sqlite_statement) {
  int rc = sqlite3_step(sqlite_statement);
  if (rc != SQLITE_OK && rc != SQLITE_DONE) {
    rc = sqlite3_finalize(
        sqlite_statement);  //  Finalize the prepared statement.
    log_error(SQLITE3_LOGGER_ID,
              "Failed in step, sqlite3 code is: %" PRIu64 "\n", rc);
    return RC_SQLITE3_FAILED_STEP;
  }

  rc = sqlite3_finalize(sqlite_statement);  //  Finalize the prepared statement.
  if (rc != SQLITE_OK) {
    log_error(SQLITE3_LOGGER_ID,
              "Failed finalizing statement, sqlite3 code is: %" PRIu64 "\n",
              rc);
    return RC_SQLITE3_FAILED_FINALIZE;
  }

  return RC_OK;
}

static retcode_t execute_statement_exist(sqlite3_stmt* sqlite_statement,
                                         bool* exist) {
  *exist = false;
  if (sqlite3_step(sqlite_statement) == SQLITE_ROW) {
    *exist = true;
  }

  int rc =
      sqlite3_finalize(sqlite_statement);  //  Finalize the prepared statement.
  if (rc != SQLITE_OK) {
    log_error(SQLITE3_LOGGER_ID,
              "Failed in finalizing the statement, sqlite3 code is: %" PRIu64
              "\n",
              rc);
    return RC_SQLITE3_FAILED_FINALIZE;
  }

  return RC_OK;
}

retcode_t iota_stor_transaction_store(const connection_t* const conn,
                                      const iota_transaction_t data_in) {
  retcode_t ret = RC_OK;
  char statement[TRANSACTION_MAX_STORE_STATEMENT_SIZE];
  char const* err_msg = 0;
  sqlite3_stmt* sqlite_statement = 0;

  if ((ret = iota_statement_transaction_insert(
           data_in, statement, TRANSACTION_MAX_STORE_STATEMENT_SIZE))) {
    return ret;
  }

  if ((ret = prepare_statement((sqlite3*)conn->db, &sqlite_statement, statement,
                               &err_msg))) {
    return ret;
  }

  ret = column_compress_bind(sqlite_statement, 1, data_in->signature_or_message,
                             FLEX_TRIT_SIZE_6561);
  ret |= column_compress_bind(sqlite_statement, 2, data_in->address,
                              FLEX_TRIT_SIZE_243);
  ret |= column_compress_bind(sqlite_statement, 3, data_in->obsolete_tag,
                              FLEX_TRIT_SIZE_81);
  ret |= column_compress_bind(sqlite_statement, 4, data_in->bundle,
                              FLEX_TRIT_SIZE_243);
  ret |= column_compress_bind(sqlite_statement, 5, data_in->trunk,
                              FLEX_TRIT_SIZE_243);
  ret |= column_compress_bind(sqlite_statement, 6, data_in->branch,
                              FLEX_TRIT_SIZE_243);
  ret |= column_compress_bind(sqlite_statement, 7, data_in->tag,
                              FLEX_TRIT_SIZE_81);
  ret |= column_compress_bind(sqlite_statement, 8, data_in->nonce,
                              FLEX_TRIT_SIZE_81);
  ret |= column_compress_bind(sqlite_statement, 9, data_in->hash,
                              FLEX_TRIT_SIZE_243);
  if (ret) {
    return RC_SQLITE3_FAILED_BINDING;
  }

  if ((ret = execute_statement_store(sqlite_statement))) {
    return ret;
  }

  return RC_OK;
}
retcode_t iota_stor_transaction_load(const connection_t* const conn,
                                     const char* col_name,
                                     const trit_array_p key,
                                     iota_stor_pack_t* pack) {
  assert(col_name && strcmp(col_name, ""));
  retcode_t ret = RC_OK;
  char statement[TRANSACTION_MAX_SELECT_STATEMENT_SIZE];
  char const* err_msg = 0;
  sqlite3_stmt* sqlite_statement = 0;

  if ((ret = iota_statement_transaction_select(
           col_name, statement, TRANSACTION_MAX_SELECT_STATEMENT_SIZE))) {
    return ret;
  }
  if ((ret = prepare_statement((sqlite3*)conn->db, &sqlite_statement, statement,
                               &err_msg))) {
    return ret;
  }
  if ((ret = column_compress_bind(sqlite_statement, 1, key->trits,
                                  key->num_bytes))) {
    return ret;
  }
  if ((ret = execute_statement_load_transactions(sqlite_statement, pack))) {
    return ret;
  }

  return RC_OK;
}
retcode_t iota_stor_transaction_exist(const connection_t* const conn,
                                      const char* index_name,
                                      const trit_array_p key, bool* exist) {
  retcode_t ret = RC_OK;
  char const* err_msg = 0;
  sqlite3_stmt* sqlite_statement = 0;
  char statement[TRANSACTION_MAX_EXIST_STATEMENT_SIZE];

  if ((ret = iota_statement_transaction_exist(
           index_name, statement, TRANSACTION_MAX_EXIST_STATEMENT_SIZE))) {
    return ret;
  }
  if ((ret = prepare_statement((sqlite3*)conn->db, &sqlite_statement, statement,
                               &err_msg))) {
    return ret;
  }

  if (key) {
    if ((ret = column_compress_bind(sqlite_statement, 1, (void*)key->trits,
                                    key->num_bytes))) {
      return ret;
    }
  }

  if ((ret = execute_statement_exist(sqlite_statement, exist))) {
    return ret;
  }

  return RC_OK;
}

extern retcode_t iota_stor_transaction_load_hashes(
    const connection_t* const conn, const char* col_name,
    const trit_array_p key, iota_stor_pack_t* pack) {
  assert(col_name && strcmp(col_name, ""));
  retcode_t ret = RC_OK;
  char statement[TRANSACTION_MAX_SELECT_STATEMENT_SIZE];
  char const* err_msg = 0;
  sqlite3_stmt* sqlite_statement = 0;

  if ((ret = iota_statement_transaction_select_hashes(
           col_name, statement, TRANSACTION_MAX_SELECT_STATEMENT_SIZE))) {
    return ret;
  }

  if ((ret = prepare_statement((sqlite3*)conn->db, &sqlite_statement, statement,
                               &err_msg))) {
    return ret;
  }

  if ((ret = column_compress_bind(sqlite_statement, 1, (void*)key->trits,
                                  key->num_bytes))) {
    return ret;
  }

  if ((ret = execute_statement_load_hashes(sqlite_statement, pack))) {
    return ret;
  }

  return RC_OK;
}

retcode_t iota_stor_transaction_load_hashes_of_approvers(
    const connection_t* const conn, const flex_trit_t* approvee_hash,
    iota_stor_pack_t* pack) {
  retcode_t ret = RC_OK;
  char statement[TRANSACTION_MAX_SELECT_STATEMENT_SIZE];
  char const* err_msg = 0;
  sqlite3_stmt* sqlite_statement = 0;

  if ((ret = iota_statement_transaction_select_hashes_approvers(
           approvee_hash, statement, TRANSACTION_MAX_SELECT_STATEMENT_SIZE))) {
    return ret;
  }

  if ((ret = prepare_statement((sqlite3*)conn->db, &sqlite_statement, statement,
                               &err_msg))) {
    return ret;
  }

  if ((ret = column_compress_bind(sqlite_statement, 1, (void*)approvee_hash,
                                  FLEX_TRIT_SIZE_243))) {
    return ret;
  }

  if ((ret = column_compress_bind(sqlite_statement, 2, (void*)approvee_hash,
                                  FLEX_TRIT_SIZE_243))) {
    return ret;
  }

  if ((ret = execute_statement_load_hashes(sqlite_statement, pack))) {
    return ret;
  }

  return RC_OK;
}

retcode_t iota_stor_transaction_update(const connection_t* const conn,
                                       const char* index_name,
                                       const trit_array_p key,
                                       const iota_transaction_t data_in) {
  // TODO - implement
  return RC_SQLITE3_FAILED_NOT_IMPLEMENTED;
}

/*
 * Milestone operations
 */

static void select_milestones_populate_from_row(sqlite3_stmt* statement,
                                                iota_milestone_t* milestone) {
  milestone->index = sqlite3_column_int64(statement, 0);
  column_decompress_load(statement, 1, milestone->hash, FLEX_TRIT_SIZE_243);
}

retcode_t iota_stor_milestone_store(const connection_t* const conn,
                                    const iota_milestone_t* data_in) {
  assert(data_in);
  retcode_t ret = RC_OK;
  char statement[MILESTONE_MAX_STORE_STATEMENT_SIZE];
  char const* err_msg = 0;
  sqlite3_stmt* sqlite_statement = 0;

  if ((ret = iota_statement_milestone_insert(
           data_in, statement, MILESTONE_MAX_STORE_STATEMENT_SIZE))) {
    return ret;
  }

  if ((ret = prepare_statement((sqlite3*)conn->db, &sqlite_statement, statement,
                               &err_msg))) {
    return ret;
  }

  if ((ret = column_compress_bind(sqlite_statement, 1,
                                  (flex_trit_t*)data_in->hash,
                                  FLEX_TRIT_SIZE_243))) {
    return ret;
  }

  if ((ret = execute_statement_store(sqlite_statement))) {
    return ret;
  }

  return RC_OK;
}

retcode_t iota_stor_milestone_load(const connection_t* const conn,
                                   const char* col_name, const trit_array_p key,
                                   iota_stor_pack_t* pack) {
  assert(col_name && strcmp(col_name, ""));
  retcode_t ret = RC_OK;
  char statement[MILESTONE_MAX_SELECT_STATEMENT_SIZE];
  char const* err_msg = 0;
  sqlite3_stmt* sqlite_statement = 0;

  if ((ret = iota_statement_milestone_select(
           col_name, statement, MILESTONE_MAX_SELECT_STATEMENT_SIZE))) {
    return ret;
  }

  if ((ret = prepare_statement((sqlite3*)conn->db, &sqlite_statement, statement,
                               &err_msg))) {
    return ret;
  }

  if ((ret = column_compress_bind(sqlite_statement, 1, (void*)key->trits,
                                  key->num_bytes))) {
    return ret;
  }

  if ((ret = execute_statement_load_milestones(sqlite_statement, pack,
                                               pack->capacity))) {
    return ret;
  }

  return RC_OK;
}

extern retcode_t iota_stor_milestone_load_first(const connection_t* const conn,
                                                iota_stor_pack_t* pack) {
  retcode_t ret = RC_OK;
  char statement[MILESTONE_MAX_SELECT_STATEMENT_SIZE];
  char const* err_msg = 0;
  sqlite3_stmt* sqlite_statement = 0;

  if ((ret = iota_statement_milestone_select_first(
           statement, MILESTONE_MAX_SELECT_STATEMENT_SIZE))) {
    return ret;
  }

  int rc = sqlite3_prepare_v2((sqlite3*)conn->db, statement, -1,
                              &sqlite_statement, &err_msg);
  if (rc != SQLITE_OK) {
    sqlite3_finalize(sqlite_statement);
    return RC_SQLITE3_FAILED_PREPARED_STATEMENT;
  }

  pack->insufficient_capacity = false;
  if (pack->capacity >= 1) {
    if (sqlite3_step(sqlite_statement) ==
        SQLITE_ROW) {  // If query has at least one result-row.
      select_milestones_populate_from_row(sqlite_statement, *pack->models);
      pack->num_loaded++;
    }
  } else {
    pack->insufficient_capacity = true;
  }

  rc = sqlite3_finalize(sqlite_statement);  //  Finalize the prepared statement
  if (rc != SQLITE_OK) {
    log_error(SQLITE3_LOGGER_ID,
              "Failed in finalizing, sqlite3 code is: %\" PRIu64 \"\n", rc);
    return RC_SQLITE3_FAILED_FINALIZE;
  }

  return RC_OK;
}

extern retcode_t iota_stor_milestone_load_last(const connection_t* const conn,
                                               iota_stor_pack_t* pack) {
  retcode_t ret = RC_OK;
  char statement[MILESTONE_MAX_SELECT_STATEMENT_SIZE];
  char const* err_msg = 0;
  sqlite3_stmt* sqlite_statement = 0;

  if ((ret = iota_statement_milestone_select_last(
           statement, MILESTONE_MAX_SELECT_STATEMENT_SIZE))) {
    return ret;
  }

  if ((ret = prepare_statement((sqlite3*)conn->db, &sqlite_statement, statement,
                               &err_msg))) {
    return ret;
  }

  if ((ret = execute_statement_load_milestones(sqlite_statement, pack, 1))) {
    return ret;
  }

  return RC_OK;
}

retcode_t iota_stor_milestone_load_next(const connection_t* const conn,
                                        uint64_t index,
                                        iota_stor_pack_t* pack) {
  retcode_t ret = RC_OK;
  char statement[MILESTONE_MAX_SELECT_STATEMENT_SIZE];
  char const* err_msg = 0;
  sqlite3_stmt* sqlite_statement = 0;

  if ((ret = iota_statement_milestone_select_next(
           statement, MILESTONE_MAX_SELECT_STATEMENT_SIZE))) {
    return ret;
  }

  if ((ret = prepare_statement((sqlite3*)conn->db, &sqlite_statement, statement,
                               &err_msg))) {
    return ret;
  }

  int rc = sqlite3_bind_int(sqlite_statement, 1, index);
  if (rc != SQLITE_OK) {
    sqlite3_finalize(sqlite_statement);  //  Finalize the prepared statement.
    log_error(SQLITE3_LOGGER_ID,
              "Failed in binding, sqlite3 code is: %" PRIu64 "\n", rc);
    return RC_SQLITE3_FAILED_BINDING;
  }

  if ((ret = execute_statement_load_milestones(sqlite_statement, pack, 1))) {
    return ret;
  }

  return RC_OK;
}

retcode_t iota_stor_milestone_exist(const connection_t* const conn,
                                    const char* col_name,
                                    const trit_array_p key, bool* exist) {
  retcode_t ret = RC_OK;
  char const* err_msg = 0;
  sqlite3_stmt* sqlite_statement = 0;
  char statement[MILESTONE_MAX_EXIST_STATEMENT_SIZE];

  if ((ret = iota_statement_milestone_exist(
           col_name, statement, MILESTONE_MAX_EXIST_STATEMENT_SIZE))) {
    return ret;
  }

  if ((ret = prepare_statement((sqlite3*)conn->db, &sqlite_statement, statement,
                               &err_msg))) {
    return ret;
  }

  if (key) {
    if ((ret = column_compress_bind(sqlite_statement, 1, (void*)key->trits,
                                    key->num_bytes))) {
      return ret;
    }
  }

  if ((ret = execute_statement_exist(sqlite_statement, exist))) {
    return ret;
  }
  return RC_OK;
}
