/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common/model/milestone.h"
#include "common/model/transaction.h"
#include "common/storage/sql/defs.h"
#include "common/storage/sql/statements.h"
#include "common/storage/storage.h"
#include "utils/logger_helper.h"

#include <logger.h>
#include <sqlite3.h>

#define SQLITE3_LOGGER_ID "stor_sqlite3"

retcode_t iota_stor_init(const connection_t* const conn,
                         const connection_config_t* const config) {
  logger_helper_init(SQLITE3_LOGGER_ID, LOGGER_INFO, true);
  init_connection(conn, config);
  return RC_OK;
}

retcode_t iota_stor_destroy(const connection_t* const conn) {
  logger_helper_destroy(SQLITE3_LOGGER_ID);
  destroy_connection(conn);
  return RC_OK;
}

static void select_transactions_populate_from_row(sqlite3_stmt* statement,
                                                  iota_transaction_t tx) {
  char const* buffer;

  buffer = sqlite3_column_blob(statement, 0);
  memcpy(tx->signature_or_message, buffer, FLEX_TRIT_SIZE_6561);
  buffer = sqlite3_column_blob(statement, 1);
  memcpy(tx->address, buffer, FLEX_TRIT_SIZE_243);
  tx->value = sqlite3_column_int64(statement, 2);
  buffer = sqlite3_column_blob(statement, 3);
  memcpy(tx->obsolete_tag, buffer, FLEX_TRIT_SIZE_81);
  tx->timestamp = sqlite3_column_int64(statement, 4);
  tx->current_index = sqlite3_column_int64(statement, 5);
  tx->last_index = sqlite3_column_int64(statement, 6);
  buffer = sqlite3_column_blob(statement, 7);
  memcpy(tx->bundle, buffer, FLEX_TRIT_SIZE_243);
  buffer = sqlite3_column_blob(statement, 8);
  memcpy(tx->trunk, buffer, FLEX_TRIT_SIZE_243);
  buffer = sqlite3_column_blob(statement, 9);
  memcpy(tx->branch, buffer, FLEX_TRIT_SIZE_243);
  buffer = sqlite3_column_blob(statement, 10);
  memcpy(tx->tag, buffer, FLEX_TRIT_SIZE_81);
  tx->attachment_timestamp = sqlite3_column_int64(statement, 11);
  tx->attachment_timestamp_upper = sqlite3_column_int64(statement, 12);
  tx->attachment_timestamp_lower = sqlite3_column_int64(statement, 13);
  buffer = sqlite3_column_blob(statement, 14);
  memcpy(tx->nonce, buffer, FLEX_TRIT_SIZE_81);
  buffer = sqlite3_column_blob(statement, 15);
  memcpy(tx->hash, buffer, FLEX_TRIT_SIZE_243);
}

static void select_milestones_populate_from_row(sqlite3_stmt* statement,
                                                iota_milestone_t* milestone) {
  char const* buffer;

  milestone->index = sqlite3_column_int64(statement, 0);
  buffer = sqlite3_column_blob(statement, 1);
  memcpy(milestone->hash, buffer, FLEX_TRIT_SIZE_243);
}

/*
 * Transaction operations
 */

retcode_t iota_stor_transaction_store(const connection_t* const conn,
                                      const iota_transaction_t data_in) {
  retcode_t ret = RC_OK;
  char statement[TRANSACTION_MAX_STORE_STATEMENT_SIZE];
  char const* err_msg = 0;
  sqlite3_stmt* sqlite_statement = 0;

  if ((ret = iota_transactions_insert_statement(
           data_in, statement, TRANSACTION_MAX_STORE_STATEMENT_SIZE))) {
    return ret;
  }

  int rc = sqlite3_prepare_v2((sqlite3*)conn->db, statement, -1,
                              &sqlite_statement, &err_msg);
  if (rc != SQLITE_OK) {
    log_error(SQLITE3_LOGGER_ID,
              "Failed preparing statement, sqlite3 code is: %\" PRIu64 \"", rc);
    return RC_SQLITE3_FAILED_PREPARED_STATEMENT;
  }
  rc = sqlite3_bind_blob(sqlite_statement, 1, data_in->signature_or_message,
                         FLEX_TRIT_SIZE_6561, NULL);
  rc |= sqlite3_bind_blob(sqlite_statement, 2, data_in->address,
                          FLEX_TRIT_SIZE_243, NULL);
  rc |= sqlite3_bind_blob(sqlite_statement, 3, data_in->obsolete_tag,
                          FLEX_TRIT_SIZE_81, NULL);
  rc |= sqlite3_bind_blob(sqlite_statement, 4, data_in->bundle,
                          FLEX_TRIT_SIZE_243, NULL);
  rc |= sqlite3_bind_blob(sqlite_statement, 5, data_in->trunk,
                          FLEX_TRIT_SIZE_243, NULL);
  rc |= sqlite3_bind_blob(sqlite_statement, 6, data_in->branch,
                          FLEX_TRIT_SIZE_243, NULL);
  rc |= sqlite3_bind_blob(sqlite_statement, 7, data_in->tag, FLEX_TRIT_SIZE_81,
                          NULL);
  rc |= sqlite3_bind_blob(sqlite_statement, 8, data_in->nonce,
                          FLEX_TRIT_SIZE_81, NULL);
  rc |= sqlite3_bind_blob(sqlite_statement, 9, data_in->hash,
                          FLEX_TRIT_SIZE_243, NULL);
  if (rc != SQLITE_OK) {
    log_error(SQLITE3_LOGGER_ID,
              "Failed in binding, sqlite3 code is: %\" PRIu64 \"", rc);
    return RC_SQLITE3_FAILED_BINDING;
  }
  rc = sqlite3_step(sqlite_statement);
  if (rc != SQLITE_OK && rc != SQLITE_DONE) {
    rc = sqlite3_finalize(
        sqlite_statement);  //  Finalize the prepared statement.
    log_error(SQLITE3_LOGGER_ID,
              "Failed in step, sqlite3 code is: %\" PRIu64 \"", rc);
    return RC_SQLITE3_FAILED_STEP;
  }

  rc = sqlite3_finalize(sqlite_statement);  //  Finalize the prepared statement.
  if (rc != SQLITE_OK) {
    log_error(SQLITE3_LOGGER_ID,
              "Failed finalizing statement, sqlite3 code is: %\" PRIu64 \"",
              rc);
    return RC_SQLITE3_FAILED_FINALIZE;
  }

  return RC_OK;
}
retcode_t iota_stor_transaction_load(const connection_t* const conn,
                                     const char* col_name,
                                     const trit_array_p key,
                                     iota_transactions_pack* pack) {
  assert(col_name && strcmp(col_name, ""));
  retcode_t ret = RC_OK;
  char statement[TRANSACTION_MAX_SELECT_STATEMENT_SIZE];
  char const* err_msg = 0;
  sqlite3_stmt* sqlite_statement = 0;

  if ((ret = iota_transactions_select_statement(
           col_name, statement, TRANSACTION_MAX_SELECT_STATEMENT_SIZE))) {
    return ret;
  }

  pack->insufficient_capacity = false;

  int rc = sqlite3_prepare_v2((sqlite3*)conn->db, statement, -1,
                              &sqlite_statement, &err_msg);
  if (rc != SQLITE_OK) {
    sqlite3_finalize(sqlite_statement);
    return RC_SQLITE3_FAILED_PREPARED_STATEMENT;
  }

  rc = sqlite3_bind_blob(sqlite_statement, 1, (void*)key->trits, key->num_bytes,
                         NULL);

  if (rc != SQLITE_OK) {
    sqlite3_finalize(sqlite_statement);  //  Finalize the prepared statement.
    log_error(SQLITE3_LOGGER_ID,
              "Failed in binding, sqlite3 code is: %\" PRIu64 \"", rc);
    return RC_SQLITE3_FAILED_BINDING;
  }

  pack->insufficient_capacity = false;
  while (sqlite3_step(sqlite_statement) ==
         SQLITE_ROW) {  // While query has result-rows.
    if (pack->num_loaded == pack->txs_capacity) {
      pack->insufficient_capacity = true;
      break;
    }
    select_transactions_populate_from_row(sqlite_statement,
                                          pack->txs[pack->num_loaded++]);
  }

  rc = sqlite3_finalize(sqlite_statement);  //  Finalize the prepared statement.
  if (rc != SQLITE_OK) {
    log_error(SQLITE3_LOGGER_ID,
              "Failed in finalizing, sqlite3 code is: %\" PRIu64 \"", rc);
    return RC_SQLITE3_FAILED_FINALIZE;
  }

  return RC_OK;
}
retcode_t iota_stor_transaction_exist(const connection_t* const conn,
                                      const char* index_name,
                                      const trit_array_p key, bool* exist) {
  retcode_t ret = RC_OK;
  char const* err_msg = 0;
  sqlite3_stmt* sqlite_statement = 0;
  char statement[TRANSACTION_MAX_SELECT_STATEMENT_SIZE];

  if ((ret = iota_transactions_exist_statement(
           index_name, statement, TRANSACTION_MAX_SELECT_STATEMENT_SIZE))) {
    return ret;
  }

  *exist = false;

  int rc = sqlite3_prepare_v2((sqlite3*)conn->db, statement, -1,
                              &sqlite_statement, &err_msg);

  if (rc != SQLITE_OK) {
    sqlite3_finalize(sqlite_statement);  //  Finalize the prepared statement.
    log_error(
        SQLITE3_LOGGER_ID,
        "Failed in preparing the statement, sqlite3 code is: %\" PRIu64 \"",
        rc);
    return RC_SQLITE3_FAILED_PREPARED_STATEMENT;
  }

  if (key) {
    rc = sqlite3_bind_blob(sqlite_statement, 1, (void*)key->trits,
                           key->num_bytes, NULL);

    if (rc != SQLITE_OK) {
      return RC_SQLITE3_FAILED_BINDING;
    }
  }

  if (sqlite3_step(sqlite_statement) == SQLITE_ROW) {
    *exist = true;
  }

  rc = sqlite3_finalize(sqlite_statement);  //  Finalize the prepared statement.
  if (rc != SQLITE_OK) {
    log_error(
        SQLITE3_LOGGER_ID,
        "Failed in finalizing the statement, sqlite3 code is: %\" PRIu64 \"",
        rc);
    return RC_SQLITE3_FAILED_FINALIZE;
  }

  return RC_OK;
}

extern retcode_t iota_stor_transaction_load_hashes(
    const connection_t* const conn, const char* col_name,
    const trit_array_p key, iota_hashes_pack* pack) {
  assert(col_name && strcmp(col_name, ""));
  retcode_t ret = RC_OK;
  char statement[TRANSACTION_MAX_SELECT_STATEMENT_SIZE];
  char const* err_msg = 0;
  sqlite3_stmt* sqlite_statement = 0;
  char const* buffer;

  if ((ret = iota_transactions_select_hashes_statement(
           col_name, statement, TRANSACTION_MAX_SELECT_STATEMENT_SIZE))) {
    return ret;
  }

  int rc = sqlite3_prepare_v2((sqlite3*)conn->db, statement, -1,
                              &sqlite_statement, &err_msg);
  if (rc != SQLITE_OK) {
    sqlite3_finalize(sqlite_statement);  //  Finalize the prepared statement.
    log_error(
        SQLITE3_LOGGER_ID,
        "Failed in preparing the statement, sqlite3 code is: %\" PRIu64 \"",
        rc);
    return RC_SQLITE3_FAILED_PREPARED_STATEMENT;
  }

  rc = sqlite3_bind_blob(sqlite_statement, 1, (void*)key->trits, key->num_bytes,
                         NULL);

  if (rc != SQLITE_OK) {
    sqlite3_finalize(sqlite_statement);  //  Finalize the prepared statement.
    log_error(SQLITE3_LOGGER_ID,
              "Failed in binding, sqlite3 code is: %\" PRIu64 \"", rc);
    return RC_SQLITE3_FAILED_BINDING;
  }

  pack->insufficient_capacity = false;
  while (sqlite3_step(sqlite_statement) ==
         SQLITE_ROW) {  // While query has result-rows.
    if (pack->num_loaded == pack->hashes_capacity) {
      pack->insufficient_capacity = true;
      break;
    }
    buffer = sqlite3_column_blob(sqlite_statement, 0);
    memcpy(pack->hashes[pack->num_loaded++]->trits, buffer, FLEX_TRIT_SIZE_243);
  }

  rc = sqlite3_finalize(sqlite_statement);  //  Finalize the prepared statement.
  if (rc != SQLITE_OK) {
    log_error(SQLITE3_LOGGER_ID,
              "Failed in finalizing, sqlite3 code is: %\" PRIu64 \"", rc);
    return RC_SQLITE3_FAILED_FINALIZE;
  }

  return RC_OK;
}

retcode_t iota_stor_transaction_load_hashes_of_approvers(
    const connection_t* const conn, const trit_array_p approvee_hash,
    iota_hashes_pack* pack) {
  retcode_t ret = RC_OK;
  char statement[TRANSACTION_MAX_SELECT_STATEMENT_SIZE];
  char const* err_msg = 0;
  sqlite3_stmt* sqlite_statement = 0;
  char const* buffer;

  if ((ret = iota_transactions_select_hashes_approvers_statement(
           approvee_hash, statement, TRANSACTION_MAX_SELECT_STATEMENT_SIZE))) {
    return ret;
  }

  int rc = sqlite3_prepare_v2((sqlite3*)conn->db, statement, -1,
                              &sqlite_statement, &err_msg);
  if (rc != SQLITE_OK) {
    return RC_SQLITE3_FAILED_PREPARED_STATEMENT;
  }

  rc = sqlite3_bind_blob(sqlite_statement, 1, (void*)approvee_hash->trits,
                         approvee_hash->num_bytes, NULL);
  if (rc != SQLITE_OK) {
    sqlite3_finalize(sqlite_statement);  //  Finalize the prepared statement.
    log_error(SQLITE3_LOGGER_ID,
              "Failed in binding, sqlite3 code is: %\" PRIu64 \"", rc);
    return RC_SQLITE3_FAILED_BINDING;
  }

  rc = sqlite3_bind_blob(sqlite_statement, 2, (void*)approvee_hash->trits,
                         approvee_hash->num_bytes, NULL);
  if (rc != SQLITE_OK) {
    sqlite3_finalize(sqlite_statement);  //  Finalize the prepared statement.
    log_error(SQLITE3_LOGGER_ID,
              "Failed in binding, sqlite3 code is: %\" PRIu64 \"", rc);
    return RC_SQLITE3_FAILED_BINDING;
  }

  pack->insufficient_capacity = false;
  while (sqlite3_step(sqlite_statement) ==
         SQLITE_ROW) {  // While query has result-rows.
    if (pack->num_loaded == pack->hashes_capacity) {
      pack->insufficient_capacity = true;
      break;
    }
    buffer = sqlite3_column_blob(sqlite_statement, 0);
    memcpy(pack->hashes[pack->num_loaded++]->trits, buffer, FLEX_TRIT_SIZE_243);
  }

  rc = sqlite3_finalize(sqlite_statement);  //  Finalize the prepared statement.
  if (rc != SQLITE_OK) {
    sqlite3_finalize(sqlite_statement);  //  Finalize the prepared statement.
    log_error(SQLITE3_LOGGER_ID,
              "Failed in finalizing, sqlite3 code is: %\" PRIu64 \"", rc);
    return RC_SQLITE3_FAILED_FINALIZE;
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

retcode_t iota_stor_milestone_store(const connection_t* const conn,
                                    const iota_milestone_t* data_in) {
  retcode_t ret = RC_OK;
  char statement[MILESTONE_MAX_STORE_STATEMENT_SIZE];
  char const* err_msg = 0;
  sqlite3_stmt* sqlite_statement = 0;

  if ((ret = iota_milestone_insert_statement(
           data_in, statement, MILESTONE_MAX_STORE_STATEMENT_SIZE))) {
    return ret;
  }

  int rc = sqlite3_prepare_v2((sqlite3*)conn->db, statement, -1,
                              &sqlite_statement, &err_msg);
  if (rc != SQLITE_OK) {
    log_error(SQLITE3_LOGGER_ID,
              "Failed preparing statement, sqlite3 code is: %\" PRIu64 \"\n",
              rc);
    return RC_SQLITE3_FAILED_PREPARED_STATEMENT;
  }
  rc = sqlite3_bind_blob(sqlite_statement, 1, data_in->hash, FLEX_TRIT_SIZE_243,
                         NULL);
  if (rc != SQLITE_OK) {
    log_error(SQLITE3_LOGGER_ID,
              "Failed in binding, sqlite3 code is: %\" PRIu64 \"\n", rc);
    return RC_SQLITE3_FAILED_BINDING;
  }
  rc = sqlite3_step(sqlite_statement);
  if (rc != SQLITE_OK && rc != SQLITE_DONE) {
    rc = sqlite3_finalize(
        sqlite_statement);  //  Finalize the prepared statement.
    log_error(SQLITE3_LOGGER_ID,
              "Failed in step, sqlite3 code is: %\" PRIu64 \"", rc);
    return RC_SQLITE3_FAILED_STEP;
  }

  rc = sqlite3_finalize(sqlite_statement);  //  Finalize the prepared statement.
  if (rc != SQLITE_OK) {
    log_error(SQLITE3_LOGGER_ID,
              "Failed finalizing statement, sqlite3 code is: %\" PRIu64 \"",
              rc);
    return RC_SQLITE3_FAILED_FINALIZE;
  }

  return RC_OK;
}

retcode_t iota_stor_milestone_load(const connection_t* const conn,
                                   const char* col_name, const trit_array_p key,
                                   iota_milestones_pack* pack) {
  assert(col_name && strcmp(col_name, ""));
  retcode_t ret = RC_OK;
  char statement[MILESTONE_MAX_SELECT_STATEMENT_SIZE];
  char const* err_msg = 0;
  sqlite3_stmt* sqlite_statement = 0;

  if ((ret = iota_milestone_select_statement(
           col_name, statement, MILESTONE_MAX_SELECT_STATEMENT_SIZE))) {
    return ret;
  }

  pack->insufficient_capacity = false;

  int rc = sqlite3_prepare_v2((sqlite3*)conn->db, statement, -1,
                              &sqlite_statement, &err_msg);
  if (rc != SQLITE_OK) {
    sqlite3_finalize(sqlite_statement);
    return RC_SQLITE3_FAILED_PREPARED_STATEMENT;
  }

  rc = sqlite3_bind_blob(sqlite_statement, 1, (void*)key->trits, key->num_bytes,
                         NULL);

  if (rc != SQLITE_OK) {
    sqlite3_finalize(sqlite_statement);  //  Finalize the prepared statement.
    log_error(SQLITE3_LOGGER_ID,
              "Failed in binding, sqlite3 code is: %\" PRIu64 \"", rc);
    return RC_SQLITE3_FAILED_BINDING;
  }

  pack->insufficient_capacity = false;
  while (sqlite3_step(sqlite_statement) ==
         SQLITE_ROW) {  // While query has result-rows.
    if (pack->num_loaded == pack->txs_capacity) {
      pack->insufficient_capacity = true;
      break;
    }
    select_milestones_populate_from_row(sqlite_statement,
                                        pack->milestones[pack->num_loaded++]);
  }

  rc = sqlite3_finalize(sqlite_statement);  //  Finalize the prepared statement
  if (rc != SQLITE_OK) {
    log_error(SQLITE3_LOGGER_ID,
              "Failed in finalizing, sqlite3 code is: %\" PRIu64 \"", rc);
    return RC_SQLITE3_FAILED_FINALIZE;
  }

  return RC_OK;
}

retcode_t iota_stor_milestone_exist(const connection_t* const conn,
                                    const char* index_name,
                                    const trit_array_p key, bool* exist) {
  retcode_t ret = RC_OK;
  char const* err_msg = 0;
  sqlite3_stmt* sqlite_statement = 0;
  char statement[MILESTONE_MAX_SELECT_STATEMENT_SIZE];

  if ((ret = iota_milestone_exist_statement(
           index_name, statement, MILESTONE_MAX_SELECT_STATEMENT_SIZE))) {
    return ret;
  }

  *exist = false;

  int rc = sqlite3_prepare_v2((sqlite3*)conn->db, statement, -1,
                              &sqlite_statement, &err_msg);

  if (rc != SQLITE_OK) {
    sqlite3_finalize(sqlite_statement);  //  Finalize the prepared statement.
    log_error(
        SQLITE3_LOGGER_ID,
        "Failed in preparing the statement, sqlite3 code is: %\" PRIu64 \"",
        rc);
    return RC_SQLITE3_FAILED_PREPARED_STATEMENT;
  }

  if (key) {
    rc = sqlite3_bind_blob(sqlite_statement, 1, (void*)key->trits,
                           key->num_bytes, NULL);

    if (rc != SQLITE_OK) {
      return RC_SQLITE3_FAILED_BINDING;
    }
  }

  if (sqlite3_step(sqlite_statement) == SQLITE_ROW) {
    *exist = true;
  }

  rc = sqlite3_finalize(sqlite_statement);  //  Finalize the prepared statement
  if (rc != SQLITE_OK) {
    log_error(
        SQLITE3_LOGGER_ID,
        "Failed in finalizing the statement, sqlite3 code is: %\" PRIu64 \"",
        rc);
    return RC_SQLITE3_FAILED_FINALIZE;
  }

  return RC_OK;
}

retcode_t iota_stor_milestone_update(const connection_t* const conn,
                                     const char* index_name,
                                     const trit_array_p key,
                                     const iota_milestone_t* data_in) {
  // TODO - implement
  return RC_SQLITE3_FAILED_NOT_IMPLEMENTED;
}
