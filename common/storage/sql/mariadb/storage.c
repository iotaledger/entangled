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
#include "utils/macros.h"
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

static void storage_transaction_load_bind_essence(MYSQL_BIND* const bind, iota_transaction_t* const transaction,
                                                  size_t* const index) {
  bind[*index].buffer = (char*)transaction->essence.address;
  bind[*index].buffer_type = MYSQL_TYPE_BLOB;
  bind[*index].buffer_length = FLEX_TRIT_SIZE_243;
  transaction->loaded_columns_mask.essence |= MASK_ESSENCE_ADDRESS;
  (*index)++;

  bind[*index].buffer = (char*)&transaction->essence.value;
  bind[*index].buffer_type = MYSQL_TYPE_LONGLONG;
  transaction->loaded_columns_mask.essence |= MASK_ESSENCE_VALUE;
  (*index)++;

  bind[*index].buffer = (char*)transaction->essence.obsolete_tag;
  bind[*index].buffer_type = MYSQL_TYPE_BLOB;
  bind[*index].buffer_length = FLEX_TRIT_SIZE_81;
  transaction->loaded_columns_mask.essence |= MASK_ESSENCE_OBSOLETE_TAG;
  (*index)++;

  bind[*index].buffer = (char*)&transaction->essence.timestamp;
  bind[*index].buffer_type = MYSQL_TYPE_LONGLONG;
  transaction->loaded_columns_mask.essence |= MASK_ESSENCE_TIMESTAMP;
  (*index)++;

  bind[*index].buffer = (char*)&transaction->essence.current_index;
  bind[*index].buffer_type = MYSQL_TYPE_SHORT;
  transaction->loaded_columns_mask.essence |= MASK_ESSENCE_CURRENT_INDEX;
  (*index)++;

  bind[*index].buffer = (char*)&transaction->essence.last_index;
  bind[*index].buffer_type = MYSQL_TYPE_SHORT;
  transaction->loaded_columns_mask.essence |= MASK_ESSENCE_LAST_INDEX;
  (*index)++;

  bind[*index].buffer = (char*)transaction->essence.bundle;
  bind[*index].buffer_type = MYSQL_TYPE_BLOB;
  bind[*index].buffer_length = FLEX_TRIT_SIZE_243;
  transaction->loaded_columns_mask.essence |= MASK_ESSENCE_BUNDLE;
  (*index)++;
}

static void storage_transaction_load_bind_attachment(MYSQL_BIND* const bind, iota_transaction_t* const transaction,
                                                     size_t* const index) {
  bind[*index].buffer = (char*)transaction->attachment.trunk;
  bind[*index].buffer_type = MYSQL_TYPE_BLOB;
  bind[*index].buffer_length = FLEX_TRIT_SIZE_243;
  transaction->loaded_columns_mask.attachment |= MASK_ATTACHMENT_TRUNK;
  (*index)++;

  bind[*index].buffer = (char*)transaction->attachment.branch;
  bind[*index].buffer_type = MYSQL_TYPE_BLOB;
  bind[*index].buffer_length = FLEX_TRIT_SIZE_243;
  transaction->loaded_columns_mask.attachment |= MASK_ATTACHMENT_BRANCH;
  (*index)++;

  bind[*index].buffer = (char*)&transaction->attachment.attachment_timestamp;
  bind[*index].buffer_type = MYSQL_TYPE_LONGLONG;
  transaction->loaded_columns_mask.attachment |= MASK_ATTACHMENT_TIMESTAMP;
  (*index)++;

  bind[*index].buffer = (char*)&transaction->attachment.attachment_timestamp_lower;
  bind[*index].buffer_type = MYSQL_TYPE_LONGLONG;
  transaction->loaded_columns_mask.attachment |= MASK_ATTACHMENT_TIMESTAMP_LOWER;
  (*index)++;

  bind[*index].buffer = (char*)&transaction->attachment.attachment_timestamp_upper;
  bind[*index].buffer_type = MYSQL_TYPE_LONGLONG;
  transaction->loaded_columns_mask.attachment |= MASK_ATTACHMENT_TIMESTAMP_UPPER;
  (*index)++;

  bind[*index].buffer = (char*)transaction->attachment.nonce;
  bind[*index].buffer_type = MYSQL_TYPE_BLOB;
  bind[*index].buffer_length = FLEX_TRIT_SIZE_81;
  transaction->loaded_columns_mask.attachment |= MASK_ATTACHMENT_NONCE;
  (*index)++;

  bind[*index].buffer = (char*)transaction->attachment.tag;
  bind[*index].buffer_type = MYSQL_TYPE_BLOB;
  bind[*index].buffer_length = FLEX_TRIT_SIZE_81;
  transaction->loaded_columns_mask.attachment |= MASK_ATTACHMENT_TAG;
  (*index)++;
}

static void storage_transaction_load_bind_consensus(MYSQL_BIND* const bind, iota_transaction_t* const transaction,
                                                    size_t* const index) {
  bind[*index].buffer = (char*)transaction->consensus.hash;
  bind[*index].buffer_type = MYSQL_TYPE_BLOB;
  bind[*index].buffer_length = FLEX_TRIT_SIZE_243;
  transaction->loaded_columns_mask.consensus |= MASK_CONSENSUS_HASH;
  (*index)++;
}

static void storage_transaction_load_bind_data(MYSQL_BIND* const bind, iota_transaction_t* const transaction,
                                               size_t* const index) {
  bind[*index].buffer = (char*)transaction->data.signature_or_message;
  bind[*index].buffer_type = MYSQL_TYPE_BLOB;
  bind[*index].buffer_length = FLEX_TRIT_SIZE_6561;
  transaction->loaded_columns_mask.data |= MASK_DATA_SIG_OR_MSG;
  (*index)++;
}

static void storage_transaction_load_bind_metadata(MYSQL_BIND* const bind, iota_transaction_t* const transaction,
                                                   size_t* const index) {
  bind[*index].buffer = (char*)&transaction->metadata.snapshot_index;
  bind[*index].buffer_type = MYSQL_TYPE_LONGLONG;
  transaction->loaded_columns_mask.metadata |= MASK_METADATA_SNAPSHOT_INDEX;
  (*index)++;

  bind[*index].buffer = (char*)&transaction->metadata.solid;
  bind[*index].buffer_type = MYSQL_TYPE_TINY;
  transaction->loaded_columns_mask.metadata |= MASK_METADATA_SOLID;
  (*index)++;

  bind[*index].buffer = (char*)&transaction->metadata.validity;
  bind[*index].buffer_type = MYSQL_TYPE_TINY;
  transaction->loaded_columns_mask.metadata |= MASK_METADATA_VALIDITY;
  (*index)++;

  bind[*index].buffer = (char*)&transaction->metadata.arrival_timestamp;
  bind[*index].buffer_type = MYSQL_TYPE_LONGLONG;
  transaction->loaded_columns_mask.metadata |= MASK_METADATA_ARRIVAL_TIMESTAMP;
  (*index)++;
}

static retcode_t storage_transaction_load_generic(MYSQL_STMT* const mariadb_statement, MYSQL_BIND* const bind_out,
                                                  storage_load_model_t const model, flex_trit_t const* const hash,
                                                  iota_stor_pack_t* const pack) {
  size_t index = 0;
  iota_transaction_t* transaction = *((iota_transaction_t**)pack->models);
  MYSQL_BIND bind_in[1];

  memset(bind_in, 0, sizeof(bind_in));

  column_compress_bind(bind_in, 0, hash, MYSQL_TYPE_BLOB, FLEX_TRIT_SIZE_243);

  if (mysql_stmt_bind_param(mariadb_statement, bind_in) != 0) {
    log_statement_error(mariadb_statement);
    return RC_STORAGE_FAILED_BINDING;
  }

  if (mysql_stmt_execute(mariadb_statement) != 0) {
    log_statement_error(mariadb_statement);
    return RC_STORAGE_FAILED_EXECUTE;
  }

  transaction_reset(transaction);

  if (model == MODEL_TRANSACTION) {
    storage_transaction_load_bind_essence(bind_out, transaction, &index);
    storage_transaction_load_bind_attachment(bind_out, transaction, &index);
    storage_transaction_load_bind_consensus(bind_out, transaction, &index);
    storage_transaction_load_bind_data(bind_out, transaction, &index);
  } else if (model == MODEL_TRANSACTION_ESSENCE_METADATA) {
    storage_transaction_load_bind_essence(bind_out, transaction, &index);
    storage_transaction_load_bind_metadata(bind_out, transaction, &index);
  } else if (model == MODEL_TRANSACTION_ESSENCE_ATTACHMENT_METADATA) {
    storage_transaction_load_bind_essence(bind_out, transaction, &index);
    storage_transaction_load_bind_attachment(bind_out, transaction, &index);
    storage_transaction_load_bind_metadata(bind_out, transaction, &index);
  } else if (model == MODEL_TRANSACTION_ESSENCE_CONSENSUS) {
    storage_transaction_load_bind_essence(bind_out, transaction, &index);
    storage_transaction_load_bind_consensus(bind_out, transaction, &index);
  } else if (model == MODEL_TRANSACTION_METADATA) {
    storage_transaction_load_bind_metadata(bind_out, transaction, &index);
  } else {
    return RC_STORAGE_FAILED_NOT_IMPLEMENTED;
  }

  if (mysql_stmt_bind_result(mariadb_statement, bind_out) != 0) {
    log_statement_error(mariadb_statement);
    return RC_STORAGE_FAILED_BINDING;
  }

  if (mysql_stmt_store_result(mariadb_statement) != 0) {
    log_statement_error(mariadb_statement);
    return RC_STORAGE_FAILED_STORE_RESULT;
  }

  pack->num_loaded = mysql_stmt_num_rows(mariadb_statement);
  pack->insufficient_capacity = pack->num_loaded > pack->capacity;

  if (pack->num_loaded == 0 || pack->insufficient_capacity == true) {
    return RC_OK;
  }

  if (mysql_stmt_fetch(mariadb_statement) != 0) {
    log_statement_error(mariadb_statement);
    return RC_STORAGE_FAILED_STORE_RESULT;
  }

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

retcode_t storage_transaction_load(storage_connection_t const* const connection,
                                   storage_transaction_field_t const field, flex_trit_t const* const key,
                                   iota_stor_pack_t* const pack) {
  mariadb_tangle_connection_t const* mariadb_connection = (mariadb_tangle_connection_t*)connection->actual;
  MYSQL_STMT* mariadb_statement = mariadb_connection->statements.transaction_select_by_hash;
  MYSQL_BIND bind_out[16];
  UNUSED(field);

  memset(bind_out, 0, sizeof(bind_out));

  return storage_transaction_load_generic(mariadb_statement, bind_out, MODEL_TRANSACTION, key, pack);
}

retcode_t storage_transaction_load_essence_and_metadata(storage_connection_t const* const connection,
                                                        flex_trit_t const* const hash, iota_stor_pack_t* const pack) {
  mariadb_tangle_connection_t const* mariadb_connection = (mariadb_tangle_connection_t*)connection->actual;
  MYSQL_STMT* mariadb_statement = mariadb_connection->statements.transaction_select_essence_and_metadata;
  MYSQL_BIND bind_out[11];

  memset(bind_out, 0, sizeof(bind_out));

  return storage_transaction_load_generic(mariadb_statement, bind_out, MODEL_TRANSACTION_ESSENCE_METADATA, hash, pack);
}

retcode_t storage_transaction_load_essence_attachment_and_metadata(storage_connection_t const* const connection,
                                                                   flex_trit_t const* const hash,
                                                                   iota_stor_pack_t* const pack) {
  mariadb_tangle_connection_t const* mariadb_connection = (mariadb_tangle_connection_t*)connection->actual;
  MYSQL_STMT* mariadb_statement = mariadb_connection->statements.transaction_select_essence_attachment_and_metadata;
  MYSQL_BIND bind_out[18];

  memset(bind_out, 0, sizeof(bind_out));

  return storage_transaction_load_generic(mariadb_statement, bind_out, MODEL_TRANSACTION_ESSENCE_ATTACHMENT_METADATA,
                                          hash, pack);
}

retcode_t storage_transaction_load_essence_and_consensus(storage_connection_t const* const connection,
                                                         flex_trit_t const* const hash, iota_stor_pack_t* const pack) {
  mariadb_tangle_connection_t const* mariadb_connection = (mariadb_tangle_connection_t*)connection->actual;
  MYSQL_STMT* mariadb_statement = mariadb_connection->statements.transaction_select_essence_and_consensus;
  MYSQL_BIND bind_out[8];

  memset(bind_out, 0, sizeof(bind_out));

  return storage_transaction_load_generic(mariadb_statement, bind_out, MODEL_TRANSACTION_ESSENCE_CONSENSUS, hash, pack);
}

retcode_t storage_transaction_load_metadata(storage_connection_t const* const connection, flex_trit_t const* const hash,
                                            iota_stor_pack_t* const pack) {
  mariadb_tangle_connection_t const* mariadb_connection = (mariadb_tangle_connection_t*)connection->actual;
  MYSQL_STMT* mariadb_statement = mariadb_connection->statements.transaction_select_metadata;
  MYSQL_BIND bind_out[4];

  memset(bind_out, 0, sizeof(bind_out));

  return storage_transaction_load_generic(mariadb_statement, bind_out, MODEL_TRANSACTION_METADATA, hash, pack);
}

retcode_t storage_transaction_exist(storage_connection_t const* const connection,
                                    storage_transaction_field_t const field, flex_trit_t const* const key,
                                    bool* const exist) {
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

retcode_t storage_transaction_load_hashes(storage_connection_t const* const connection,
                                          storage_transaction_field_t const field, flex_trit_t const* const key,
                                          iota_stor_pack_t* const pack) {
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
