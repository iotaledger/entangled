/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <assert.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdlib.h>

#include <sqlite3.h>

#include "common/model/milestone.h"
#include "common/model/transaction.h"
#include "common/storage/sql/defs.h"
#include "common/storage/sql/statements.h"
#include "common/storage/storage.h"
#include "utils/logger_helper.h"

#define SQLITE3_LOGGER_ID "storage_sqlite3"

retcode_t iota_stor_init(connection_t const* const conn,
                         connection_config_t const* const config) {
  logger_helper_init(SQLITE3_LOGGER_ID, LOGGER_DEBUG, true);
  return init_connection(conn, config);
}

retcode_t iota_stor_destroy(connection_t const* const conn) {
  logger_helper_destroy(SQLITE3_LOGGER_ID);
  return destroy_connection(conn);
}

/*
 * Binding functions
 */

static retcode_t column_compress_bind(sqlite3_stmt* statement, size_t index,
                                      flex_trit_t const* const flex_trits,
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
 * Generic function
 */

static void select_milestones_populate_from_row(sqlite3_stmt* statement,
                                                iota_milestone_t* milestone);

static void select_transactions_populate_from_row(sqlite3_stmt* statement,
                                                  iota_transaction_t tx);

static retcode_t prepare_statement(sqlite3* db, sqlite3_stmt** sqlite_statement,
                                   char* const statement) {
  char const* err_msg = NULL;

  int rc = sqlite3_prepare_v2(db, statement, -1, sqlite_statement, &err_msg);
  if (rc != SQLITE_OK) {
    log_error(SQLITE3_LOGGER_ID,
              "Failed preparing statement, sqlite3 code is: %" PRIu64 "\n", rc);
    return RC_SQLITE3_FAILED_PREPARED_STATEMENT;
  }

  return RC_OK;
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

static retcode_t execute_statement_load_hashes(sqlite3_stmt* sqlite_statement,
                                               iota_stor_pack_t* pack) {
  return execute_statement_load_gen(sqlite_statement, pack, pack->capacity,
                                    MODEL_HASH);
}

static retcode_t execute_statement_store_update(
    sqlite3_stmt* sqlite_statement) {
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

/*
 * Transaction operations
 */

static retcode_t execute_statement_load_transactions(
    sqlite3_stmt* const sqlite_statement, iota_stor_pack_t* const pack) {
  return execute_statement_load_gen(sqlite_statement, pack, pack->capacity,
                                    MODEL_TRANSACTION);
}

static void select_transactions_populate_from_row(sqlite3_stmt* const statement,
                                                  iota_transaction_t const tx) {
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

retcode_t iota_stor_transaction_store(connection_t const* const conn,
                                      iota_transaction_t const tx) {
  retcode_t ret = RC_OK;
  sqlite3_stmt* sqlite_statement = NULL;

  if ((ret = prepare_statement((sqlite3*)conn->db, &sqlite_statement,
                               iota_statement_transaction_insert))) {
    return ret;
  }

  ret = column_compress_bind(sqlite_statement, 1, tx->signature_or_message,
                             FLEX_TRIT_SIZE_6561);
  ret |= column_compress_bind(sqlite_statement, 2, tx->address,
                              FLEX_TRIT_SIZE_243);
  sqlite3_bind_int64(sqlite_statement, 3, tx->value);
  ret |= column_compress_bind(sqlite_statement, 4, tx->obsolete_tag,
                              FLEX_TRIT_SIZE_81);
  sqlite3_bind_int64(sqlite_statement, 5, tx->timestamp);
  sqlite3_bind_int64(sqlite_statement, 6, tx->current_index);
  sqlite3_bind_int64(sqlite_statement, 7, tx->last_index);
  ret |=
      column_compress_bind(sqlite_statement, 8, tx->bundle, FLEX_TRIT_SIZE_243);
  ret |=
      column_compress_bind(sqlite_statement, 9, tx->trunk, FLEX_TRIT_SIZE_243);
  ret |= column_compress_bind(sqlite_statement, 10, tx->branch,
                              FLEX_TRIT_SIZE_243);
  ret |= column_compress_bind(sqlite_statement, 11, tx->tag, FLEX_TRIT_SIZE_81);
  sqlite3_bind_int64(sqlite_statement, 12, tx->attachment_timestamp);
  sqlite3_bind_int64(sqlite_statement, 13, tx->attachment_timestamp_lower);
  sqlite3_bind_int64(sqlite_statement, 14, tx->attachment_timestamp_upper);
  ret |=
      column_compress_bind(sqlite_statement, 15, tx->nonce, FLEX_TRIT_SIZE_81);
  ret |=
      column_compress_bind(sqlite_statement, 16, tx->hash, FLEX_TRIT_SIZE_243);
  sqlite3_bind_int64(sqlite_statement, 17, tx->snapshot_index);
  if (ret) {
    return RC_SQLITE3_FAILED_BINDING;
  }

  if ((ret = execute_statement_store_update(sqlite_statement))) {
    return ret;
  }

  return ret;
}

retcode_t iota_stor_transaction_load(connection_t const* const conn,
                                     transaction_field_t const field,
                                     trit_array_t const* const key,
                                     iota_stor_pack_t* const pack) {
  retcode_t ret = RC_OK;
  char* statement = NULL;
  sqlite3_stmt* sqlite_statement = NULL;

  switch (field) {
    case TRANSACTION_FIELD_HASH:
      statement = iota_statement_transaction_select_by_hash;
      break;
    default:
      break;
  }

  if ((ret = prepare_statement((sqlite3*)conn->db, &sqlite_statement,
                               statement))) {
    return ret;
  }

  if ((ret = column_compress_bind(sqlite_statement, 1, key->trits,
                                  key->num_bytes))) {
    return ret;
  }

  if ((ret = execute_statement_load_transactions(sqlite_statement, pack))) {
    return ret;
  }

  return ret;
}

retcode_t iota_stor_transaction_load_hashes(connection_t const* const conn,
                                            transaction_field_t const field,
                                            trit_array_t const* const key,
                                            iota_stor_pack_t* const pack) {
  retcode_t ret = RC_OK;
  char* statement = NULL;
  sqlite3_stmt* sqlite_statement = NULL;

  switch (field) {
    case TRANSACTION_FIELD_ADDRESS:
      statement = iota_statement_transaction_select_hashes_by_address;
      break;
    default:
      break;
  }

  if ((ret = prepare_statement((sqlite3*)conn->db, &sqlite_statement,
                               statement))) {
    return ret;
  }

  if ((ret = column_compress_bind(sqlite_statement, 1, key->trits,
                                  key->num_bytes))) {
    return ret;
  }

  if ((ret = execute_statement_load_hashes(sqlite_statement, pack))) {
    return ret;
  }

  return ret;
}

retcode_t iota_stor_transaction_load_hashes_of_approvers(
    connection_t const* const conn, flex_trit_t const* const approvee_hash,
    iota_stor_pack_t* const pack) {
  retcode_t ret = RC_OK;
  sqlite3_stmt* sqlite_statement = NULL;

  if ((ret = prepare_statement(
           (sqlite3*)conn->db, &sqlite_statement,
           iota_statement_transaction_select_hashes_of_approvers))) {
    return ret;
  }

  if ((ret = column_compress_bind(sqlite_statement, 1, approvee_hash,
                                  FLEX_TRIT_SIZE_243))) {
    return ret;
  }
  if ((ret = column_compress_bind(sqlite_statement, 2, approvee_hash,
                                  FLEX_TRIT_SIZE_243))) {
    return ret;
  }

  if ((ret = execute_statement_load_hashes(sqlite_statement, pack))) {
    return ret;
  }

  return ret;
}

retcode_t iota_stor_transaction_update_snapshot_index(
    connection_t const* const conn, flex_trit_t const* const hash,
    uint64_t const snapshot_index) {
  retcode_t ret = RC_OK;
  sqlite3_stmt* sqlite_statement = NULL;

  if ((ret = prepare_statement(
           (sqlite3*)conn->db, &sqlite_statement,
           iota_statement_transaction_update_snapshot_index))) {
    return ret;
  }

  sqlite3_bind_int64(sqlite_statement, 1, snapshot_index);
  column_compress_bind(sqlite_statement, 2, hash, FLEX_TRIT_SIZE_243);

  if ((ret = execute_statement_store_update(sqlite_statement))) {
    return ret;
  }

  return ret;
}

retcode_t iota_stor_transaction_exist(connection_t const* const conn,
                                      transaction_field_t const field,
                                      trit_array_t const* const key,
                                      bool* const exist) {
  retcode_t ret = RC_OK;
  char* statement = NULL;
  sqlite3_stmt* sqlite_statement = NULL;

  switch (field) {
    case TRANSACTION_FIELD_NONE:
      statement = iota_statement_transaction_exist;
      break;
    case TRANSACTION_FIELD_HASH:
      statement = iota_statement_transaction_exist_by_hash;
      break;
    default:
      break;
  }

  if ((ret = prepare_statement((sqlite3*)conn->db, &sqlite_statement,
                               statement))) {
    return ret;
  }

  if (field != TRANSACTION_FIELD_NONE && key) {
    if ((ret = column_compress_bind(sqlite_statement, 1, (void*)key->trits,
                                    key->num_bytes))) {
      return ret;
    }
  }

  if ((ret = execute_statement_exist(sqlite_statement, exist))) {
    return ret;
  }

  return ret;
}

/*
 * Milestone operations
 */

static retcode_t execute_statement_load_milestones(
    sqlite3_stmt* const sqlite_statement, iota_stor_pack_t* const pack,
    uint32_t const max_records) {
  return execute_statement_load_gen(sqlite_statement, pack, max_records,
                                    MODEL_MILESTONE);
}

static void select_milestones_populate_from_row(
    sqlite3_stmt* const statement, iota_milestone_t* const milestone) {
  milestone->index = sqlite3_column_int64(statement, 0);
  column_decompress_load(statement, 1, milestone->hash, FLEX_TRIT_SIZE_243);
}

retcode_t iota_stor_milestone_store(connection_t const* const conn,
                                    iota_milestone_t const* const milestone) {
  assert(milestone);
  retcode_t ret = RC_OK;
  sqlite3_stmt* sqlite_statement = NULL;

  if ((ret = prepare_statement((sqlite3*)conn->db, &sqlite_statement,
                               iota_statement_milestone_insert))) {
    return ret;
  }

  sqlite3_bind_int64(sqlite_statement, 1, milestone->index);
  if ((ret = column_compress_bind(sqlite_statement, 2,
                                  (flex_trit_t*)milestone->hash,
                                  FLEX_TRIT_SIZE_243))) {
    return ret;
  }

  if ((ret = execute_statement_store_update(sqlite_statement))) {
    return ret;
  }

  return RC_OK;
}

retcode_t iota_stor_milestone_load(connection_t const* const conn,
                                   trit_array_t const* const hash,
                                   iota_stor_pack_t* const pack) {
  retcode_t ret = RC_OK;
  sqlite3_stmt* sqlite_statement = NULL;

  if ((ret = prepare_statement((sqlite3*)conn->db, &sqlite_statement,
                               iota_statement_milestone_select_by_hash)) !=
      RC_OK) {
    return ret;
  }

  if ((ret = column_compress_bind(sqlite_statement, 1, hash->trits,
                                  hash->num_bytes)) != RC_OK) {
    return ret;
  }

  if ((ret = execute_statement_load_milestones(sqlite_statement, pack,
                                               pack->capacity)) != RC_OK) {
    return ret;
  }

  return ret;
}

retcode_t iota_stor_milestone_load_first(connection_t const* const conn,
                                         iota_stor_pack_t* const pack) {
  retcode_t ret = RC_OK;
  sqlite3_stmt* sqlite_statement = NULL;

  if ((ret = prepare_statement((sqlite3*)conn->db, &sqlite_statement,
                               iota_statement_milestone_select_first)) !=
      RC_OK) {
    return ret;
  }

  if ((ret = execute_statement_load_milestones(sqlite_statement, pack, 1)) !=
      RC_OK) {
    return ret;
  }

  return ret;
}

retcode_t iota_stor_milestone_load_last(connection_t const* const conn,
                                        iota_stor_pack_t* const pack) {
  retcode_t ret = RC_OK;
  sqlite3_stmt* sqlite_statement = NULL;

  if ((ret = prepare_statement((sqlite3*)conn->db, &sqlite_statement,
                               iota_statement_milestone_select_last)) !=
      RC_OK) {
    return ret;
  }

  if ((ret = execute_statement_load_milestones(sqlite_statement, pack, 1)) !=
      RC_OK) {
    return ret;
  }

  return ret;
}

retcode_t iota_stor_milestone_load_next(connection_t const* const conn,
                                        uint64_t const index,
                                        iota_stor_pack_t* const pack) {
  retcode_t ret = RC_OK;
  sqlite3_stmt* sqlite_statement = NULL;
  int rc = 0;

  if ((ret = prepare_statement((sqlite3*)conn->db, &sqlite_statement,
                               iota_statement_milestone_select_next)) !=
      RC_OK) {
    return ret;
  }

  if ((rc = sqlite3_bind_int(sqlite_statement, 1, index)) != SQLITE_OK) {
    sqlite3_finalize(sqlite_statement);
    log_error(SQLITE3_LOGGER_ID,
              "Failed in binding, sqlite3 code is: %" PRIu64 "\n", rc);
    return RC_SQLITE3_FAILED_BINDING;
  }

  if ((ret = execute_statement_load_milestones(sqlite_statement, pack, 1)) !=
      RC_OK) {
    return ret;
  }

  return ret;
}

retcode_t iota_stor_milestone_exist(connection_t const* const conn,
                                    trit_array_t const* const key,
                                    bool* const exist) {
  retcode_t ret = RC_OK;
  char* statement = NULL;
  sqlite3_stmt* sqlite_statement = NULL;

  if (key) {
    statement = iota_statement_milestone_exist_by_hash;
  } else {
    statement = iota_statement_milestone_exist;
  }

  if ((ret = prepare_statement((sqlite3*)conn->db, &sqlite_statement,
                               statement))) {
    return ret;
  }

  if (key) {
    if ((ret = column_compress_bind(sqlite_statement, 1, key->trits,
                                    key->num_bytes))) {
      return ret;
    }
  }

  if ((ret = execute_statement_exist(sqlite_statement, exist))) {
    return ret;
  }

  return ret;
}
