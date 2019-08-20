/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdlib.h>
#include <string.h>

#include <sqlite3.h>

#include "common/storage/sql/sqlite3/connection.h"
#include "common/storage/sql/sqlite3/wrappers.h"
#include "common/storage/sql/statements.h"
#include "utils/logger_helper.h"

#define SQLITE3_LOGGER_ID "sqlite3"

static logger_id_t logger_id;

static retcode_t prepare_tangle_statements(sqlite3_tangle_connection_t* const connection) {
  retcode_t ret = RC_OK;

  ret = prepare_statement(connection->db, (sqlite3_stmt**)(&connection->statements.transaction_insert),
                          storage_statement_transaction_insert);
  ret |= prepare_statement(connection->db, (sqlite3_stmt**)(&connection->statements.transaction_select_by_hash),
                           storage_statement_transaction_select_by_hash);
  ret |=
      prepare_statement(connection->db, (sqlite3_stmt**)(&connection->statements.transaction_select_hashes_by_address),
                        storage_statement_transaction_select_hashes_by_address);
  ret |= prepare_statement(connection->db,
                           (sqlite3_stmt**)(&connection->statements.transaction_select_hashes_of_approvers),
                           storage_statement_transaction_select_hashes_of_approvers);
  ret |= prepare_statement(connection->db,
                           (sqlite3_stmt**)(&connection->statements.transaction_select_hashes_of_approvers_before_date),
                           storage_statement_transaction_select_hashes_of_approvers_before_date);
  ret |= prepare_statement(connection->db,
                           (sqlite3_stmt**)(&connection->statements.transaction_select_hashes_of_milestone_candidates),
                           storage_statement_transaction_select_hashes_of_milestone_candidates);
  ret |= prepare_statement(connection->db, (sqlite3_stmt**)(&connection->statements.transaction_update_snapshot_index),
                           storage_statement_transaction_update_snapshot_index);
  ret |= prepare_statement(connection->db, (sqlite3_stmt**)(&connection->statements.transaction_update_solid_state),
                           storage_statement_transaction_update_solid_state);
  ret |= prepare_statement(connection->db, (sqlite3_stmt**)(&connection->statements.transaction_update_validity),
                           storage_statement_transaction_update_validity);
  ret |= prepare_statement(connection->db, (sqlite3_stmt**)(&connection->statements.transaction_exist),
                           storage_statement_transaction_exist);
  ret |= prepare_statement(connection->db, (sqlite3_stmt**)(&connection->statements.transaction_exist_by_hash),
                           storage_statement_transaction_exist_by_hash);
  ret |= prepare_statement(connection->db, (sqlite3_stmt**)(&connection->statements.transaction_approvers_count),
                           storage_statement_transaction_approvers_count);
  ret |= prepare_statement(connection->db, (sqlite3_stmt**)(&connection->statements.transaction_count),
                           storage_statement_transaction_count);
  ret |= prepare_statement(connection->db, (sqlite3_stmt**)(&connection->statements.transaction_delete),
                           storage_statement_transaction_delete);
  ret |= prepare_statement(connection->db,
                           (sqlite3_stmt**)(&connection->statements.transaction_select_essence_and_metadata),
                           storage_statement_transaction_select_essence_and_metadata);
  ret |= prepare_statement(connection->db,
                           (sqlite3_stmt**)(&connection->statements.transaction_select_essence_attachment_and_metadata),
                           storage_statement_transaction_select_essence_attachment_and_metadata);
  ret |= prepare_statement(connection->db,
                           (sqlite3_stmt**)(&connection->statements.transaction_select_essence_and_consensus),
                           storage_statement_transaction_select_essence_and_consensus);
  ret |= prepare_statement(connection->db, (sqlite3_stmt**)(&connection->statements.transaction_select_metadata),
                           storage_statement_transaction_select_metadata);
  ret |= prepare_statement(connection->db, (sqlite3_stmt**)(&connection->statements.transaction_metadata_clear),
                           storage_statement_transaction_metadata_clear);
  ret |= prepare_statement(connection->db, (sqlite3_stmt**)(&connection->statements.milestone_clear),
                           storage_statement_milestone_clear);
  ret |= prepare_statement(connection->db, (sqlite3_stmt**)(&connection->statements.milestone_insert),
                           storage_statement_milestone_insert);
  ret |= prepare_statement(connection->db, (sqlite3_stmt**)(&connection->statements.milestone_select_by_hash),
                           storage_statement_milestone_select_by_hash);
  ret |= prepare_statement(connection->db, (sqlite3_stmt**)(&connection->statements.milestone_select_last),
                           storage_statement_milestone_select_last);
  ret |= prepare_statement(connection->db, (sqlite3_stmt**)(&connection->statements.milestone_select_first),
                           storage_statement_milestone_select_first);
  ret |= prepare_statement(connection->db, (sqlite3_stmt**)(&connection->statements.milestone_select_by_index),
                           storage_statement_milestone_select_by_index);
  ret |= prepare_statement(connection->db, (sqlite3_stmt**)(&connection->statements.milestone_select_next),
                           storage_statement_milestone_select_next);
  ret |= prepare_statement(connection->db, (sqlite3_stmt**)(&connection->statements.milestone_exist),
                           storage_statement_milestone_exist);
  ret |= prepare_statement(connection->db, (sqlite3_stmt**)(&connection->statements.milestone_exist_by_hash),
                           storage_statement_milestone_exist_by_hash);
  ret |= prepare_statement(connection->db, (sqlite3_stmt**)(&connection->statements.milestone_delete_by_hash),
                           storage_statement_milestone_delete_by_hash);
  ret |= prepare_statement(connection->db, (sqlite3_stmt**)(&connection->statements.state_delta_store),
                           storage_statement_state_delta_store);
  ret |= prepare_statement(connection->db, (sqlite3_stmt**)(&connection->statements.state_delta_load),
                           storage_statement_state_delta_load);

  if (ret != RC_OK) {
    log_error(logger_id, "Preparing tangle statements failed\n");
  }

  return ret;
}

static retcode_t prepare_spent_addresses_statements(sqlite3_spent_addresses_connection_t* const connection) {
  retcode_t ret = RC_OK;

  ret = prepare_statement(connection->db, (sqlite3_stmt**)(&connection->statements.spent_address_insert),
                          storage_statement_spent_address_insert);
  ret |= prepare_statement(connection->db, (sqlite3_stmt**)(&connection->statements.spent_address_exist),
                           storage_statement_spent_address_exist);

  if (ret != RC_OK) {
    log_error(logger_id, "Preparing spent addresses statements failed\n");
  }

  return ret;
}

static retcode_t finalize_tangle_statements(sqlite3_tangle_connection_t* const connection) {
  retcode_t ret = RC_OK;

  ret = finalize_statement(connection->statements.transaction_insert);
  ret |= finalize_statement(connection->statements.transaction_select_by_hash);
  ret |= finalize_statement(connection->statements.transaction_select_hashes_by_address);
  ret |= finalize_statement(connection->statements.transaction_select_hashes_of_approvers);
  ret |= finalize_statement(connection->statements.transaction_select_hashes_of_approvers_before_date);
  ret |= finalize_statement(connection->statements.transaction_select_hashes_of_milestone_candidates);
  ret |= finalize_statement(connection->statements.transaction_update_snapshot_index);
  ret |= finalize_statement(connection->statements.transaction_update_solid_state);
  ret |= finalize_statement(connection->statements.transaction_update_validity);
  ret |= finalize_statement(connection->statements.transaction_exist);
  ret |= finalize_statement(connection->statements.transaction_exist_by_hash);
  ret |= finalize_statement(connection->statements.transaction_approvers_count);
  ret |= finalize_statement(connection->statements.transaction_count);
  ret |= finalize_statement(connection->statements.transaction_select_essence_and_metadata);
  ret |= finalize_statement(connection->statements.transaction_select_essence_attachment_and_metadata);
  ret |= finalize_statement(connection->statements.transaction_select_essence_and_consensus);
  ret |= finalize_statement(connection->statements.transaction_select_metadata);
  ret |= finalize_statement(connection->statements.transaction_metadata_clear);
  ret |= finalize_statement(connection->statements.transaction_delete);
  ret |= finalize_statement(connection->statements.milestone_clear);
  ret |= finalize_statement(connection->statements.milestone_insert);
  ret |= finalize_statement(connection->statements.milestone_select_by_hash);
  ret |= finalize_statement(connection->statements.milestone_select_last);
  ret |= finalize_statement(connection->statements.milestone_select_first);
  ret |= finalize_statement(connection->statements.milestone_select_by_index);
  ret |= finalize_statement(connection->statements.milestone_select_next);
  ret |= finalize_statement(connection->statements.milestone_exist);
  ret |= finalize_statement(connection->statements.milestone_exist_by_hash);
  ret |= finalize_statement(connection->statements.milestone_delete_by_hash);
  ret |= finalize_statement(connection->statements.state_delta_store);
  ret |= finalize_statement(connection->statements.state_delta_load);

  if (ret != RC_OK) {
    log_error(logger_id, "Finalizing tangle statements failed\n");
  }

  return ret;
}

static retcode_t finalize_spent_addresses_statements(sqlite3_spent_addresses_connection_t* const connection) {
  retcode_t ret = RC_OK;

  ret = finalize_statement(connection->statements.spent_address_insert);
  ret |= finalize_statement(connection->statements.spent_address_exist);

  if (ret != RC_OK) {
    log_error(logger_id, "Finalizing spent addresses statements failed\n");
  }

  return ret;
}

retcode_t storage_connection_init(storage_connection_t* const connection,
                                  storage_connection_config_t const* const config,
                                  storage_connection_type_t const type) {
  retcode_t ret = RC_OK;
  sqlite3** db = NULL;
  char* err_msg = NULL;
  char* sql = NULL;
  int rc = 0;

  if (connection == NULL) {
    return RC_NULL_PARAM;
  }

  logger_id = logger_helper_enable(SQLITE3_LOGGER_ID, LOGGER_DEBUG, true);

  if (type == STORAGE_CONNECTION_TANGLE) {
    if ((connection->actual = calloc(1, sizeof(sqlite3_tangle_connection_t))) == NULL) {
      return RC_OOM;
    }
    db = &((sqlite3_tangle_connection_t*)connection->actual)->db;
  } else if (type == STORAGE_CONNECTION_SPENT_ADDRESSES) {
    if ((connection->actual = calloc(1, sizeof(sqlite3_spent_addresses_connection_t))) == NULL) {
      return RC_OOM;
    }
    db = &((sqlite3_spent_addresses_connection_t*)connection->actual)->db;
  }
  connection->type = type;

  if (config->db_path == NULL) {
    log_critical(logger_id, "No path for db specified\n");
    return RC_SQLITE3_NO_PATH_FOR_DB_SPECIFIED;
  }

  if ((rc = sqlite3_open_v2(config->db_path, db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_NOMUTEX, NULL)) != SQLITE_OK) {
    log_critical(logger_id, "Failed to open db on path: %s\n", config->db_path);
    return RC_SQLITE3_FAILED_OPEN_DB;
  }

  if ((rc = sqlite3_busy_timeout(*db, 10000)) != SQLITE_OK) {
    return RC_SQLITE3_FAILED_CONFIG;
  }

  sql = "PRAGMA journal_mode = WAL;PRAGMA foreign_keys = ON";

  if ((rc = sqlite3_exec(*db, sql, NULL, NULL, &err_msg)) != SQLITE_OK) {
    sqlite3_free(err_msg);
    return RC_SQLITE3_FAILED_INSERT_DB;
  }

  if (type == STORAGE_CONNECTION_TANGLE) {
    ret = prepare_tangle_statements(connection->actual);
  } else if (type == STORAGE_CONNECTION_SPENT_ADDRESSES) {
    ret = prepare_spent_addresses_statements(connection->actual);
  }

  return ret;
}

retcode_t storage_connection_destroy(storage_connection_t* const connection) {
  retcode_t ret = RC_OK;

  if (connection == NULL) {
    return RC_NULL_PARAM;
  }

  if (connection->type == STORAGE_CONNECTION_TANGLE) {
    sqlite3_tangle_connection_t* sqlite3_connection = (sqlite3_tangle_connection_t*)connection->actual;

    ret = finalize_tangle_statements(sqlite3_connection);
    sqlite3_close(sqlite3_connection->db);
    sqlite3_connection->db = NULL;
  } else if (connection->type == STORAGE_CONNECTION_SPENT_ADDRESSES) {
    sqlite3_spent_addresses_connection_t* sqlite3_connection =
        (sqlite3_spent_addresses_connection_t*)connection->actual;

    ret = finalize_spent_addresses_statements(sqlite3_connection);
    sqlite3_close(sqlite3_connection->db);
    sqlite3_connection->db = NULL;
  }

  free(connection->actual);

  return ret;
}
