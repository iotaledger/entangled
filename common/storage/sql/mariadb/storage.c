/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <mysql.h>

#include "common/storage/sql/mariadb/connection.h"
#include "common/storage/sql/mariadb/wrappers.h"
#include "common/storage/storage.h"
#include "utils/logger_helper.h"
#include "utils/time.h"

#define MARIADB_LOGGER_ID "mariadb"

static logger_id_t logger_id;

/**
 * Private functions
 */

static void log_statement_error(MYSQL_STMT* const stmt) {
  log_error(logger_id, "Statement error with code: %d, state: %s and message: \"%s\"\n", mysql_stmt_errno(stmt),
            mysql_stmt_sqlstate(stmt), mysql_stmt_error(stmt));
}

static retcode_t execute_statement_exist(MYSQL_STMT* const mariadb_statement, bool* const exist) {
  if (mysql_stmt_execute(mariadb_statement) != 0) {
    log_statement_error(mariadb_statement);
    return RC_STORAGE_FAILED_EXECUTE;
  }

  if (mysql_stmt_store_result(mariadb_statement) != 0) {
    log_statement_error(mariadb_statement);
    return RC_STORAGE_FAILED_STORE_RESULT;
  }

  *exist = mysql_stmt_num_rows(mariadb_statement) != 0;

  return RC_OK;
}

/**
 * Public functions
 */

retcode_t storage_init() {
  logger_id = logger_helper_enable(MARIADB_LOGGER_ID, LOGGER_DEBUG, true);

  if (mysql_library_init(0, NULL, NULL) != 0) {
    return RC_STORAGE_FAILED_INIT;
  }

  return RC_OK;
}

retcode_t storage_destroy() {
  logger_helper_release(logger_id);

  mysql_library_end();

  return RC_OK;
}

retcode_t storage_transaction_count(storage_connection_t const* const connection, size_t* const count) {
  mariadb_tangle_connection_t const* mariadb_connection = (mariadb_tangle_connection_t*)connection->actual;
  MYSQL_STMT* mariadb_statement = mariadb_connection->statements.transaction_count;

  return RC_OK;
}

retcode_t storage_transaction_store(storage_connection_t const* const connection,
                                    iota_transaction_t const* const transaction) {
  mariadb_tangle_connection_t const* mariadb_connection = (mariadb_tangle_connection_t*)connection->actual;
  MYSQL_STMT* mariadb_statement = mariadb_connection->statements.transaction_insert;
  MYSQL_BIND bind[17];
  uint64_t ts = current_timestamp_ms();

  memset(bind, 0, sizeof(bind));

  column_compress_bind(bind, 0, transaction->data.signature_or_message, MYSQL_TYPE_BLOB, FLEX_TRIT_SIZE_6561);
  column_compress_bind(bind, 1, transaction->essence.address, MYSQL_TYPE_BLOB, FLEX_TRIT_SIZE_243);
  column_compress_bind(bind, 2, &transaction->essence.value, MYSQL_TYPE_LONGLONG, -1);
  column_compress_bind(bind, 3, transaction->essence.obsolete_tag, MYSQL_TYPE_BLOB, FLEX_TRIT_SIZE_81);
  column_compress_bind(bind, 4, &transaction->essence.timestamp, MYSQL_TYPE_LONGLONG, -1);
  column_compress_bind(bind, 5, &transaction->essence.current_index, MYSQL_TYPE_SHORT, -1);
  column_compress_bind(bind, 6, &transaction->essence.last_index, MYSQL_TYPE_SHORT, -1);
  column_compress_bind(bind, 7, transaction->essence.bundle, MYSQL_TYPE_BLOB, FLEX_TRIT_SIZE_243);
  column_compress_bind(bind, 8, transaction->attachment.trunk, MYSQL_TYPE_BLOB, FLEX_TRIT_SIZE_243);
  column_compress_bind(bind, 9, transaction->attachment.branch, MYSQL_TYPE_BLOB, FLEX_TRIT_SIZE_243);
  column_compress_bind(bind, 10, transaction->attachment.tag, MYSQL_TYPE_BLOB, FLEX_TRIT_SIZE_81);
  column_compress_bind(bind, 11, &transaction->attachment.attachment_timestamp, MYSQL_TYPE_LONGLONG, -1);
  column_compress_bind(bind, 12, &transaction->attachment.attachment_timestamp_upper, MYSQL_TYPE_LONGLONG, -1);
  column_compress_bind(bind, 13, &transaction->attachment.attachment_timestamp_lower, MYSQL_TYPE_LONGLONG, -1);
  column_compress_bind(bind, 14, transaction->attachment.nonce, MYSQL_TYPE_BLOB, FLEX_TRIT_SIZE_81);
  column_compress_bind(bind, 15, transaction->consensus.hash, MYSQL_TYPE_BLOB, FLEX_TRIT_SIZE_243);
  column_compress_bind(bind, 16, &ts, MYSQL_TYPE_LONGLONG, -1);

  if (mysql_stmt_bind_param(mariadb_statement, bind) != 0) {
    log_statement_error(mariadb_statement);
    return RC_STORAGE_FAILED_BINDING;
  }

  if (mysql_stmt_execute(mariadb_statement) != 0) {
    log_statement_error(mariadb_statement);
    return RC_STORAGE_FAILED_EXECUTE;
  }

  return RC_OK;
}

retcode_t storage_transaction_load(storage_connection_t const* const connection, transaction_field_t const field,
                                   flex_trit_t const* const key, iota_stor_pack_t* const pack) {
  mariadb_tangle_connection_t const* mariadb_connection = (mariadb_tangle_connection_t*)connection->actual;
  MYSQL_STMT* mariadb_statement = mariadb_connection->statements.transaction_select_by_hash;

  return RC_OK;
}

retcode_t storage_transaction_load_essence_and_metadata(storage_connection_t const* const connection,
                                                        flex_trit_t const* const hash, iota_stor_pack_t* const pack) {
  mariadb_tangle_connection_t const* mariadb_connection = (mariadb_tangle_connection_t*)connection->actual;
  MYSQL_STMT* mariadb_statement = mariadb_connection->statements.transaction_select_essence_and_metadata;

  return RC_OK;
}

retcode_t storage_transaction_load_essence_attachment_and_metadata(storage_connection_t const* const connection,
                                                                   flex_trit_t const* const hash,
                                                                   iota_stor_pack_t* const pack) {
  mariadb_tangle_connection_t const* mariadb_connection = (mariadb_tangle_connection_t*)connection->actual;
  MYSQL_STMT* mariadb_statement = mariadb_connection->statements.transaction_select_essence_attachment_and_metadata;

  return RC_OK;
}

retcode_t storage_transaction_load_essence_and_consensus(storage_connection_t const* const connection,
                                                         flex_trit_t const* const hash, iota_stor_pack_t* const pack) {
  mariadb_tangle_connection_t const* mariadb_connection = (mariadb_tangle_connection_t*)connection->actual;
  MYSQL_STMT* mariadb_statement = mariadb_connection->statements.transaction_select_essence_and_consensus;

  return RC_OK;
}

retcode_t storage_transaction_load_metadata(storage_connection_t const* const connection, flex_trit_t const* const hash,
                                            iota_stor_pack_t* const pack) {
  mariadb_tangle_connection_t const* mariadb_connection = (mariadb_tangle_connection_t*)connection->actual;
  MYSQL_STMT* mariadb_statement = mariadb_connection->statements.transaction_select_metadata;

  return RC_OK;
}

retcode_t storage_transaction_exist(storage_connection_t const* const connection, transaction_field_t const field,
                                    flex_trit_t const* const key, bool* const exist) {
  mariadb_tangle_connection_t const* mariadb_connection = (mariadb_tangle_connection_t*)connection->actual;
  MYSQL_STMT* mariadb_statement = NULL;
  MYSQL_BIND bind[1];
  retcode_t ret = RC_OK;
  size_t num_bytes_key;

  switch (field) {
    case TRANSACTION_FIELD_NONE:
      mariadb_statement = mariadb_connection->statements.transaction_exist;
      break;
    case TRANSACTION_FIELD_HASH:
      mariadb_statement = mariadb_connection->statements.transaction_exist_by_hash;
      num_bytes_key = FLEX_TRIT_SIZE_243;
      break;
    default:
      return RC_STORAGE_FAILED_NOT_IMPLEMENTED;
  }

  if (field != TRANSACTION_FIELD_NONE && key) {
    memset(bind, 0, sizeof(bind));
    column_compress_bind(bind, 0, key, MYSQL_TYPE_BLOB, num_bytes_key);
    if (mysql_stmt_bind_param(mariadb_statement, bind) != 0) {
      log_statement_error(mariadb_statement);
      return RC_STORAGE_FAILED_BINDING;
    }
  }

  return execute_statement_exist(mariadb_statement, exist);
}

retcode_t storage_transaction_update_snapshot_index(storage_connection_t const* const connection,
                                                    flex_trit_t const* const hash, uint64_t const snapshot_index) {
  mariadb_tangle_connection_t const* mariadb_connection = (mariadb_tangle_connection_t*)connection->actual;
  MYSQL_STMT* mariadb_statement = mariadb_connection->statements.transaction_update_snapshot_index;

  return RC_OK;
}

retcode_t storage_transaction_update_solid_state(storage_connection_t const* const connection,
                                                 flex_trit_t const* const hash, bool const is_solid) {
  mariadb_tangle_connection_t const* mariadb_connection = (mariadb_tangle_connection_t*)connection->actual;
  MYSQL_STMT* mariadb_statement = mariadb_connection->statements.transaction_update_solid_state;

  return RC_OK;
}

retcode_t storage_transaction_load_hashes(storage_connection_t const* const connection, transaction_field_t const field,
                                          flex_trit_t const* const key, iota_stor_pack_t* const pack) {
  mariadb_tangle_connection_t const* mariadb_connection = (mariadb_tangle_connection_t*)connection->actual;
  MYSQL_STMT* mariadb_statement = mariadb_connection->statements.transaction_select_hashes_by_address;

  return RC_OK;
}

retcode_t storage_transaction_load_hashes_of_approvers(storage_connection_t const* const connection,
                                                       flex_trit_t const* const approvee_hash,
                                                       iota_stor_pack_t* const pack, int64_t before_timestamp) {
  mariadb_tangle_connection_t const* mariadb_connection = (mariadb_tangle_connection_t*)connection->actual;
  MYSQL_STMT* mariadb_statement = mariadb_connection->statements.transaction_select_hashes_of_approvers;

  return RC_OK;
}

retcode_t storage_transaction_load_hashes_of_milestone_candidates(storage_connection_t const* const connection,
                                                                  iota_stor_pack_t* const pack,
                                                                  flex_trit_t const* const coordinator) {
  mariadb_tangle_connection_t const* mariadb_connection = (mariadb_tangle_connection_t*)connection->actual;
  MYSQL_STMT* mariadb_statement = mariadb_connection->statements.transaction_select_hashes_of_milestone_candidates;

  return RC_OK;
}

retcode_t storage_transaction_approvers_count(storage_connection_t const* const connection,
                                              flex_trit_t const* const hash, size_t* const count) {
  mariadb_tangle_connection_t const* mariadb_connection = (mariadb_tangle_connection_t*)connection->actual;
  MYSQL_STMT* mariadb_statement = mariadb_connection->statements.transaction_approvers_count;

  return RC_OK;
}

retcode_t storage_transaction_find(storage_connection_t const* const connection, hash243_queue_t const bundles,
                                   hash243_queue_t const addresses, hash81_queue_t const tags,
                                   hash243_queue_t const approvees, iota_stor_pack_t* const pack) {
  mariadb_tangle_connection_t const* mariadb_connection = (mariadb_tangle_connection_t*)connection->actual;

  return RC_OK;
}

retcode_t storage_transaction_metadata_clear(storage_connection_t const* const connection) {
  mariadb_tangle_connection_t const* mariadb_connection = (mariadb_tangle_connection_t*)connection->actual;
  MYSQL_STMT* mariadb_statement = mariadb_connection->statements.transaction_metadata_clear;
  return RC_OK;
}

retcode_t storage_transactions_update_snapshot_index(storage_connection_t const* const connection,
                                                     hash243_set_t const hashes, uint64_t const snapshot_index) {
  mariadb_tangle_connection_t const* mariadb_connection = (mariadb_tangle_connection_t*)connection->actual;

  return RC_OK;
}

retcode_t storage_transactions_update_solid_state(storage_connection_t const* const connection,
                                                  hash243_set_t const hashes, bool const is_solid) {
  mariadb_tangle_connection_t const* mariadb_connection = (mariadb_tangle_connection_t*)connection->actual;

  return RC_OK;
}

retcode_t storage_transactions_delete(storage_connection_t const* const connection, hash243_set_t const hashes) {
  mariadb_tangle_connection_t const* mariadb_connection = (mariadb_tangle_connection_t*)connection->actual;
  MYSQL_STMT* mariadb_statement = mariadb_connection->statements.transaction_delete;

  return RC_OK;
}
