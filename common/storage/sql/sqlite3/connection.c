/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdlib.h>
#include <string.h>

#include <sqlite3.h>

#include "common/storage/connection.h"
#include "common/storage/sql/sqlite3/wrappers.h"
#include "common/storage/sql/statements.h"
#include "utils/logger_helper.h"

#define SQLITE3_LOGGER_ID "sqlite3"

static retcode_t prepare_statements(connection_t const* const conn) {
  retcode_t ret = RC_OK;

  ret =
      prepare_statement((sqlite3*)conn->db,
                        &((iota_statements_t*)(conn->data))->transaction_insert,
                        iota_statement_transaction_insert);
  ret |= prepare_statement(
      (sqlite3*)conn->db,
      &((iota_statements_t*)(conn->data))->transaction_select_by_hash,
      iota_statement_transaction_select_by_hash);
  ret |= prepare_statement(
      (sqlite3*)conn->db,
      &((iota_statements_t*)(conn->data))->transaction_select_hashes_by_address,
      iota_statement_transaction_select_hashes_by_address);
  ret |=
      prepare_statement((sqlite3*)conn->db,
                        &((iota_statements_t*)(conn->data))
                             ->transaction_select_hashes_of_approvers,
                        iota_statement_transaction_select_hashes_of_approvers);
  ret |= prepare_statement(
      (sqlite3*)conn->db,
      &((iota_statements_t*)(conn->data))
           ->transaction_select_hashes_of_approvers_before_date,
      iota_statement_transaction_select_hashes_of_approvers_before_date);
  ret |= prepare_statement(
      (sqlite3*)conn->db,
      &((iota_statements_t*)(conn->data))
           ->transaction_select_hashes_of_transactions_to_request,
      iota_statement_transaction_select_hashes_of_transactions_to_request);
  ret |= prepare_statement(
      (sqlite3*)conn->db,
      &((iota_statements_t*)(conn->data))->transaction_select_hashes_of_tips,
      iota_statement_transaction_select_hashes_of_tips);
  ret |= prepare_statement(
      (sqlite3*)conn->db,
      &((iota_statements_t*)(conn->data))
           ->transaction_select_hashes_of_milestone_candidates,
      iota_statement_transaction_select_hashes_of_milestone_candidates);
  ret |= prepare_statement(
      (sqlite3*)conn->db,
      &((iota_statements_t*)(conn->data))->transaction_update_snapshot_index,
      iota_statement_transaction_update_snapshot_index);
  ret |= prepare_statement(
      (sqlite3*)conn->db,
      &((iota_statements_t*)(conn->data))->transaction_update_solid_state,
      iota_statement_transaction_update_solid_state);
  ret |=
      prepare_statement((sqlite3*)conn->db,
                        &((iota_statements_t*)(conn->data))->transaction_exist,
                        iota_statement_transaction_exist);
  ret |= prepare_statement(
      (sqlite3*)conn->db,
      &((iota_statements_t*)(conn->data))->transaction_exist_by_hash,
      iota_statement_transaction_exist_by_hash);
  ret |= prepare_statement(
      (sqlite3*)conn->db,
      &((iota_statements_t*)(conn->data))->transaction_approvers_count,
      iota_statement_transaction_approvers_count);
  ret |=
      prepare_statement((sqlite3*)conn->db,
                        &((iota_statements_t*)(conn->data))->transaction_count,
                        iota_statement_transaction_count);
  ret |=
      prepare_statement((sqlite3*)conn->db,
                        &((iota_statements_t*)(conn->data))
                             ->transaction_select_essence_and_metadata,
                        iota_statement_transaction_select_essence_and_metadata);
  ret |= prepare_statement(
      (sqlite3*)conn->db,
      &((iota_statements_t*)(conn->data))
           ->transaction_select_essence_attachment_and_metadata,
      iota_statement_transaction_select_essence_attachment_and_metadata);
  ret |= prepare_statement(
      (sqlite3*)conn->db,
      &((iota_statements_t*)(conn->data))
           ->transaction_select_essence_and_consensus,
      iota_statement_transaction_select_essence_and_consensus);
  ret |= prepare_statement(
      (sqlite3*)conn->db,
      &((iota_statements_t*)(conn->data))->transaction_select_metadata,
      iota_statement_transaction_select_metadata);
  ret |= prepare_statement(
      (sqlite3*)conn->db, &((iota_statements_t*)(conn->data))->milestone_insert,
      iota_statement_milestone_insert);
  ret |= prepare_statement(
      (sqlite3*)conn->db,
      &((iota_statements_t*)(conn->data))->milestone_select_by_hash,
      iota_statement_milestone_select_by_hash);
  ret |= prepare_statement(
      (sqlite3*)conn->db,
      &((iota_statements_t*)(conn->data))->milestone_select_first,
      iota_statement_milestone_select_first);
  ret |= prepare_statement(
      (sqlite3*)conn->db,
      &((iota_statements_t*)(conn->data))->milestone_select_last,
      iota_statement_milestone_select_last);
  ret |= prepare_statement(
      (sqlite3*)conn->db,
      &((iota_statements_t*)(conn->data))->milestone_select_next,
      iota_statement_milestone_select_next);
  ret |= prepare_statement((sqlite3*)conn->db,
                           &((iota_statements_t*)(conn->data))->milestone_exist,
                           iota_statement_milestone_exist);
  ret |= prepare_statement(
      (sqlite3*)conn->db,
      &((iota_statements_t*)(conn->data))->milestone_exist_by_hash,
      iota_statement_milestone_exist_by_hash);
  ret |=
      prepare_statement((sqlite3*)conn->db,
                        &((iota_statements_t*)(conn->data))->state_delta_store,
                        iota_statement_state_delta_store);
  ret |= prepare_statement(
      (sqlite3*)conn->db, &((iota_statements_t*)(conn->data))->state_delta_load,
      iota_statement_state_delta_load);

  if (ret != RC_OK) {
    log_error(SQLITE3_LOGGER_ID, "Preparing statements failed\n");
  }

  return ret;
}

static retcode_t finalize_statements(connection_t const* const conn) {
  retcode_t ret = RC_OK;

  ret = finalize_statement(
      ((iota_statements_t*)(conn->data))->transaction_insert);
  ret |= finalize_statement(
      ((iota_statements_t*)(conn->data))->transaction_select_by_hash);
  ret |= finalize_statement(
      ((iota_statements_t*)(conn->data))->transaction_select_hashes_by_address);
  ret |= finalize_statement(((iota_statements_t*)(conn->data))
                                ->transaction_select_hashes_of_approvers);
  ret |= finalize_statement(
      ((iota_statements_t*)(conn->data))
          ->transaction_select_hashes_of_approvers_before_date);
  ret |= finalize_statement(
      ((iota_statements_t*)(conn->data))
          ->transaction_select_hashes_of_transactions_to_request);
  ret |= finalize_statement(
      ((iota_statements_t*)(conn->data))->transaction_select_hashes_of_tips);
  ret |= finalize_statement(
      ((iota_statements_t*)(conn->data))
          ->transaction_select_hashes_of_milestone_candidates);
  ret |= finalize_statement(
      ((iota_statements_t*)(conn->data))->transaction_update_snapshot_index);
  ret |= finalize_statement(
      ((iota_statements_t*)(conn->data))->transaction_update_solid_state);
  ret |=
      finalize_statement(((iota_statements_t*)(conn->data))->transaction_exist);
  ret |= finalize_statement(
      ((iota_statements_t*)(conn->data))->transaction_exist_by_hash);
  ret |= finalize_statement(
      ((iota_statements_t*)(conn->data))->transaction_approvers_count);
  ret |=
      finalize_statement(((iota_statements_t*)(conn->data))->transaction_count);
  ret |= finalize_statement(((iota_statements_t*)(conn->data))
                                ->transaction_select_essence_and_metadata);
  ret |= finalize_statement(
      ((iota_statements_t*)(conn->data))
          ->transaction_select_essence_attachment_and_metadata);
  ret |= finalize_statement(((iota_statements_t*)(conn->data))
                                ->transaction_select_essence_and_consensus);
  ret |= finalize_statement(
      ((iota_statements_t*)(conn->data))->transaction_select_metadata);
  ret |=
      finalize_statement(((iota_statements_t*)(conn->data))->milestone_insert);
  ret |= finalize_statement(
      ((iota_statements_t*)(conn->data))->milestone_select_by_hash);
  ret |= finalize_statement(
      ((iota_statements_t*)(conn->data))->milestone_select_first);
  ret |= finalize_statement(
      ((iota_statements_t*)(conn->data))->milestone_select_last);
  ret |= finalize_statement(
      ((iota_statements_t*)(conn->data))->milestone_select_next);
  ret |=
      finalize_statement(((iota_statements_t*)(conn->data))->milestone_exist);
  ret |= finalize_statement(
      ((iota_statements_t*)(conn->data))->milestone_exist_by_hash);
  ret |=
      finalize_statement(((iota_statements_t*)(conn->data))->state_delta_store);
  ret |=
      finalize_statement(((iota_statements_t*)(conn->data))->state_delta_load);

  if (ret != RC_OK) {
    log_error(SQLITE3_LOGGER_ID, "Finalizing statements failed\n");
  }

  return ret;
}

retcode_t connection_init(connection_t* const conn,
                          connection_config_t const* const config) {
  char* err_msg = NULL;
  char* sql = NULL;
  int rc = 0;

  if (config->db_path == NULL) {
    log_critical(SQLITE3_LOGGER_ID, "No path for db specified\n");
    return RC_SQLITE3_NO_PATH_FOR_DB_SPECIFIED;
  }

  if ((rc = sqlite3_open_v2(config->db_path, (sqlite3**)&conn->db,
                            SQLITE_OPEN_READWRITE | SQLITE_OPEN_NOMUTEX,
                            NULL)) != SQLITE_OK) {
    log_critical(SQLITE3_LOGGER_ID, "Failed to open db on path: %s\n",
                 config->db_path);
    return RC_SQLITE3_FAILED_OPEN_DB;
  }

  if ((rc = sqlite3_busy_timeout((sqlite3*)conn->db, 10000)) != SQLITE_OK) {
    return RC_SQLITE3_FAILED_CONFIG;
  }

  sql = "PRAGMA journal_mode = WAL;PRAGMA foreign_keys = ON";

  if ((rc = sqlite3_exec((sqlite3*)conn->db, sql, NULL, NULL, &err_msg)) !=
      SQLITE_OK) {
    sqlite3_free(err_msg);
    return RC_SQLITE3_FAILED_INSERT_DB;
  }

  if ((conn->data = calloc(1, sizeof(iota_statements_t))) == NULL) {
    return RC_OOM;
  }

  return prepare_statements(conn);
}

retcode_t connection_destroy(connection_t* const conn) {
  retcode_t ret = RC_OK;

  if (conn->db != NULL) {
    log_info(SQLITE3_LOGGER_ID, "Destroying connection\n");
    ret = finalize_statements(conn);
    free(conn->data);
    conn->data = NULL;
    sqlite3_close((sqlite3*)conn->db);
    conn->db = NULL;
  }
  return ret;
}
