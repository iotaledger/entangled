/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdlib.h>

#include "common/storage/sql/mariadb/connection.h"
#include "common/storage/sql/mariadb/wrappers.h"
#include "utils/logger_helper.h"
#include "utils/macros.h"

#define MARIADB_LOGGER_ID "mariadb"

static logger_id_t logger_id;

static retcode_t prepare_tangle_statements(mariadb_tangle_connection_t* const connection) {
  retcode_t ret = RC_OK;

  ret = prepare_statement(&connection->db, (MYSQL_STMT**)(&connection->statements.transaction_insert),
                          storage_statement_transaction_insert);
  ret |= prepare_statement(&connection->db, (MYSQL_STMT**)(&connection->statements.transaction_select_by_hash),
                           storage_statement_transaction_select_by_hash);
  ret |=
      prepare_statement(&connection->db, (MYSQL_STMT**)(&connection->statements.transaction_select_hashes_by_address),
                        storage_statement_transaction_select_hashes_by_address);
  ret |=
      prepare_statement(&connection->db, (MYSQL_STMT**)(&connection->statements.transaction_select_hashes_of_approvers),
                        storage_statement_transaction_select_hashes_of_approvers);
  ret |= prepare_statement(&connection->db,
                           (MYSQL_STMT**)(&connection->statements.transaction_select_hashes_of_approvers_before_date),
                           storage_statement_transaction_select_hashes_of_approvers_before_date);
  ret |= prepare_statement(&connection->db,
                           (MYSQL_STMT**)(&connection->statements.transaction_select_hashes_of_milestone_candidates),
                           storage_statement_transaction_select_hashes_of_milestone_candidates);
  ret |= prepare_statement(&connection->db, (MYSQL_STMT**)(&connection->statements.transaction_update_snapshot_index),
                           storage_statement_transaction_update_snapshot_index);
  ret |= prepare_statement(&connection->db, (MYSQL_STMT**)(&connection->statements.transaction_update_solid_state),
                           storage_statement_transaction_update_solid_state);
  ret |= prepare_statement(&connection->db, (MYSQL_STMT**)(&connection->statements.transaction_update_validity),
                           storage_statement_transaction_update_validity);
  ret |= prepare_statement(&connection->db, (MYSQL_STMT**)(&connection->statements.transaction_exist),
                           storage_statement_transaction_exist);
  ret |= prepare_statement(&connection->db, (MYSQL_STMT**)(&connection->statements.transaction_exist_by_hash),
                           storage_statement_transaction_exist_by_hash);
  ret |= prepare_statement(&connection->db, (MYSQL_STMT**)(&connection->statements.transaction_approvers_count),
                           storage_statement_transaction_approvers_count);
  ret |= prepare_statement(&connection->db, (MYSQL_STMT**)(&connection->statements.transaction_count),
                           storage_statement_transaction_count);
  ret |= prepare_statement(&connection->db, (MYSQL_STMT**)(&connection->statements.transaction_delete),
                           storage_statement_transaction_delete);
  ret |= prepare_statement(&connection->db, (MYSQL_STMT**)(&connection->statements.transaction_select_essence_metadata),
                           storage_statement_transaction_select_essence_metadata);
  ret |= prepare_statement(&connection->db,
                           (MYSQL_STMT**)(&connection->statements.transaction_select_essence_attachment_metadata),
                           storage_statement_transaction_select_essence_attachment_metadata);
  ret |=
      prepare_statement(&connection->db, (MYSQL_STMT**)(&connection->statements.transaction_select_essence_consensus),
                        storage_statement_transaction_select_essence_consensus);
  ret |= prepare_statement(&connection->db, (MYSQL_STMT**)(&connection->statements.transaction_select_metadata),
                           storage_statement_transaction_select_metadata);
  ret |= prepare_statement(&connection->db, (MYSQL_STMT**)(&connection->statements.transaction_metadata_clear),
                           storage_statement_transaction_metadata_clear);
  ret |= prepare_statement(&connection->db, (MYSQL_STMT**)(&connection->statements.milestone_clear),
                           storage_statement_milestone_clear);
  ret |= prepare_statement(&connection->db, (MYSQL_STMT**)(&connection->statements.milestone_insert),
                           storage_statement_milestone_insert);
  ret |= prepare_statement(&connection->db, (MYSQL_STMT**)(&connection->statements.milestone_select_by_hash),
                           storage_statement_milestone_select_by_hash);
  ret |= prepare_statement(&connection->db, (MYSQL_STMT**)(&connection->statements.milestone_select_last),
                           storage_statement_milestone_select_last);
  ret |= prepare_statement(&connection->db, (MYSQL_STMT**)(&connection->statements.milestone_select_first),
                           storage_statement_milestone_select_first);
  ret |= prepare_statement(&connection->db, (MYSQL_STMT**)(&connection->statements.milestone_select_by_index),
                           storage_statement_milestone_select_by_index);
  ret |= prepare_statement(&connection->db, (MYSQL_STMT**)(&connection->statements.milestone_select_next),
                           storage_statement_milestone_select_next);
  ret |= prepare_statement(&connection->db, (MYSQL_STMT**)(&connection->statements.milestone_exist),
                           storage_statement_milestone_exist);
  ret |= prepare_statement(&connection->db, (MYSQL_STMT**)(&connection->statements.milestone_exist_by_hash),
                           storage_statement_milestone_exist_by_hash);
  ret |= prepare_statement(&connection->db, (MYSQL_STMT**)(&connection->statements.milestone_delete_by_hash),
                           storage_statement_milestone_delete_by_hash);
  ret |= prepare_statement(&connection->db, (MYSQL_STMT**)(&connection->statements.state_delta_store),
                           storage_statement_state_delta_store);
  ret |= prepare_statement(&connection->db, (MYSQL_STMT**)(&connection->statements.state_delta_load),
                           storage_statement_state_delta_load);

  if (ret != RC_OK) {
    log_error(logger_id, "Preparing tangle statements failed\n");
  }

  return ret;
}

static retcode_t prepare_spent_addresses_statements(mariadb_spent_addresses_connection_t* const connection) {
  retcode_t ret = RC_OK;

  ret = prepare_statement(&connection->db, (MYSQL_STMT**)(&connection->statements.spent_address_insert),
                          storage_statement_spent_address_insert);
  ret |= prepare_statement(&connection->db, (MYSQL_STMT**)(&connection->statements.spent_address_exist),
                           storage_statement_spent_address_exist);

  if (ret != RC_OK) {
    log_error(logger_id, "Preparing spent addresses statements failed\n");
  }

  return ret;
}

static retcode_t finalize_tangle_statements(mariadb_tangle_connection_t* const connection) {
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
  ret |= finalize_statement(connection->statements.transaction_select_essence_metadata);
  ret |= finalize_statement(connection->statements.transaction_select_essence_attachment_metadata);
  ret |= finalize_statement(connection->statements.transaction_select_essence_consensus);
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

static retcode_t finalize_spent_addresses_statements(mariadb_spent_addresses_connection_t* const connection) {
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
  MYSQL* db = NULL;
  char* database = NULL;
  UNUSED(config);

  if (connection == NULL) {
    return RC_NULL_PARAM;
  }

  logger_id = logger_helper_enable(MARIADB_LOGGER_ID, LOGGER_DEBUG, true);

  if (type == STORAGE_CONNECTION_TANGLE) {
    if ((connection->actual = calloc(1, sizeof(mariadb_tangle_connection_t))) == NULL) {
      return RC_OOM;
    }
    db = &((mariadb_tangle_connection_t*)connection->actual)->db;
    database = "tangle";
  } else if (type == STORAGE_CONNECTION_SPENT_ADDRESSES) {
    if ((connection->actual = calloc(1, sizeof(mariadb_spent_addresses_connection_t))) == NULL) {
      return RC_OOM;
    }
    db = &((mariadb_spent_addresses_connection_t*)connection->actual)->db;
    database = "spent-addresses";
  }
  connection->type = type;

  if (mysql_init(db) == NULL) {
    return RC_STORAGE_FAILED_INIT;
  }

  if (mysql_real_connect(db, "localhost", "ciri", NULL, database, 0, NULL, 0) == 0) {
    log_error(logger_id, "Connection to database %s failed: %s\n", database, mysql_error(db));
    return RC_STORAGE_FAILED_OPEN_DB;
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
  MYSQL* db = NULL;

  if (connection == NULL) {
    return RC_NULL_PARAM;
  }

  if (connection->type == STORAGE_CONNECTION_TANGLE) {
    mariadb_tangle_connection_t* mariadb_connection = (mariadb_tangle_connection_t*)connection->actual;

    db = &((mariadb_tangle_connection_t*)connection->actual)->db;
    ret = finalize_tangle_statements(mariadb_connection);
  } else if (connection->type == STORAGE_CONNECTION_SPENT_ADDRESSES) {
    mariadb_spent_addresses_connection_t* mariadb_connection =
        (mariadb_spent_addresses_connection_t*)connection->actual;

    db = &((mariadb_spent_addresses_connection_t*)connection->actual)->db;
    ret = finalize_spent_addresses_statements(mariadb_connection);
  }

  mysql_close(db);

  free(connection->actual);

  return ret;
}
