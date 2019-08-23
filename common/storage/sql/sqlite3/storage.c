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
#include "common/storage/sql/sqlite3/connection.h"
#include "common/storage/sql/sqlite3/wrappers.h"
#include "common/storage/sql/statements.h"
#include "common/storage/storage.h"
#include "utils/logger_helper.h"
#include "utils/macros.h"
#include "utils/time.h"

#define SQLITE3_LOGGER_ID "sqlite3"

static logger_id_t logger_id;

static void error_log_callback(void* const arg, int const err_code, char const* const message) {
  UNUSED(arg);

  log_error(logger_id, "Failed with error code %d: %s\n", err_code, message);
}

retcode_t storage_init() {
  logger_id = logger_helper_enable(SQLITE3_LOGGER_ID, LOGGER_DEBUG, true);

  if (sqlite3_config(SQLITE_CONFIG_LOG, error_log_callback, NULL) != SQLITE_OK) {
    return RC_STORAGE_FAILED_CONFIG;
  }

  // TODO - implement connections pool so no two threads
  // will access db through same connection simultaneously
  if (sqlite3_config(SQLITE_CONFIG_MULTITHREAD) != SQLITE_OK) {
    return RC_STORAGE_FAILED_CONFIG;
  }

  if (sqlite3_config(SQLITE_CONFIG_MEMSTATUS, 0) != SQLITE_OK) {
    return RC_STORAGE_FAILED_CONFIG;
  }

  if (sqlite3_initialize() != SQLITE_OK) {
    return RC_STORAGE_FAILED_INIT;
  }

  return RC_OK;
}

retcode_t storage_destroy() {
  logger_helper_release(logger_id);

  if (sqlite3_shutdown() != SQLITE_OK) {
    return RC_STORAGE_FAILED_SHUTDOWN;
  }

  return RC_OK;
}

/*
 * Declarations/Definitions
 */

typedef struct bind_execute_hash_params_s {
  sqlite3_stmt* sqlite_statement;
  size_t hash_index;
} bind_execute_hash_params_t;

static retcode_t bind_execute_hash_do_func(bind_execute_hash_params_t* const params, flex_trit_t const* const hash);

/*
 * Generic function
 */

static void select_milestones_populate_from_row(sqlite3_stmt* const statement, iota_milestone_t* const milestone);

static void select_transactions_populate_essence(sqlite3_stmt* const statement, iota_transaction_t* const tx,
                                                 size_t* const index);
static void select_transactions_populate_attachment(sqlite3_stmt* const statement, iota_transaction_t* const tx,
                                                    size_t* const index);
static void select_transactions_populate_consensus(sqlite3_stmt* const statement, iota_transaction_t* const tx,
                                                   size_t* const index);
static void select_transactions_populate_data(sqlite3_stmt* const statement, iota_transaction_t* const tx,
                                              size_t* const index);
static void select_transactions_populate_metadata(sqlite3_stmt* const statement, iota_transaction_t* const tx,
                                                  size_t* const index);

static retcode_t execute_statement_load_gen(sqlite3_stmt* const sqlite_statement, iota_stor_pack_t* const pack,
                                            uint32_t const max_records, storage_load_model_t const model) {
  size_t index = 0;

  pack->insufficient_capacity = false;
  while (sqlite3_step(sqlite_statement) == SQLITE_ROW) {  // While query has result-rows.
    index = 0;

    if (pack->num_loaded == pack->capacity || pack->num_loaded == max_records) {
      pack->insufficient_capacity = (pack->num_loaded == pack->capacity);
      break;
    }
    if (model == MODEL_HASH) {
      column_decompress_load(sqlite_statement, 0, ((flex_trit_t*)pack->models[pack->num_loaded]), FLEX_TRIT_SIZE_243);
      pack->num_loaded++;
    } else if (model == MODEL_MILESTONE) {
      select_milestones_populate_from_row(sqlite_statement, pack->models[pack->num_loaded]);
      pack->num_loaded++;
    } else if (model == MODEL_TRANSACTION) {
      select_transactions_populate_essence(sqlite_statement, pack->models[pack->num_loaded], &index);
      select_transactions_populate_attachment(sqlite_statement, pack->models[pack->num_loaded], &index);
      select_transactions_populate_consensus(sqlite_statement, pack->models[pack->num_loaded], &index);
      select_transactions_populate_data(sqlite_statement, pack->models[pack->num_loaded], &index);
      pack->num_loaded++;
    } else if (model == MODEL_TRANSACTION_ESSENCE_METADATA) {
      select_transactions_populate_essence(sqlite_statement, pack->models[pack->num_loaded], &index);
      select_transactions_populate_metadata(sqlite_statement, pack->models[pack->num_loaded], &index);
      pack->num_loaded++;
    } else if (model == MODEL_TRANSACTION_ESSENCE_ATTACHMENT_METADATA) {
      select_transactions_populate_essence(sqlite_statement, pack->models[pack->num_loaded], &index);
      select_transactions_populate_attachment(sqlite_statement, pack->models[pack->num_loaded], &index);
      select_transactions_populate_metadata(sqlite_statement, pack->models[pack->num_loaded], &index);
      pack->num_loaded++;
    } else if (model == MODEL_TRANSACTION_ESSENCE_CONSENSUS) {
      select_transactions_populate_essence(sqlite_statement, pack->models[pack->num_loaded], &index);
      select_transactions_populate_consensus(sqlite_statement, pack->models[pack->num_loaded], &index);
      pack->num_loaded++;
    } else if (model == MODEL_TRANSACTION_METADATA) {
      select_transactions_populate_metadata(sqlite_statement, pack->models[pack->num_loaded], &index);
      pack->num_loaded++;
    } else {
      return RC_STORAGE_FAILED_NOT_IMPLEMENTED;
    }
  }

  return RC_OK;
}

static retcode_t execute_statement_load_hashes(sqlite3_stmt* const sqlite_statement, iota_stor_pack_t* const pack) {
  return execute_statement_load_gen(sqlite_statement, pack, pack->capacity, MODEL_HASH);
}

static retcode_t execute_statement(sqlite3_stmt* const sqlite_statement) {
  int rc = sqlite3_step(sqlite_statement);
  if (rc != SQLITE_OK && rc != SQLITE_DONE) {
    return RC_STORAGE_FAILED_STEP;
  }

  return RC_OK;
}

static retcode_t execute_statement_exist(sqlite3_stmt* const sqlite_statement, bool* const exist) {
  int rc = sqlite3_step(sqlite_statement);
  *exist = false;

  if (rc == SQLITE_ROW) {
    *exist = true;
  } else if (rc != SQLITE_OK && rc != SQLITE_DONE) {
    return RC_STORAGE_FAILED_STEP;
  }

  return RC_OK;
}

static retcode_t update_transactions(storage_connection_t const* const connection, hash243_set_t const hashes,
                                     void const* const value, sqlite3_stmt* const sqlite_statement,
                                     storage_value_type_t const type) {
  sqlite3_tangle_connection_t const* sqlite3_connection = (sqlite3_tangle_connection_t*)connection->actual;
  retcode_t ret = RC_OK;
  retcode_t ret_rollback;
  bool should_rollback_if_failed = false;

  if ((ret = begin_transaction(sqlite3_connection->db)) != RC_OK) {
    return ret;
  }

  should_rollback_if_failed = true;
  if (type == BOOLEAN) {
    int value_int = *((bool*)value);
    if (sqlite3_bind_int(sqlite_statement, 1, value_int) != SQLITE_OK) {
      ret = RC_STORAGE_FAILED_BINDING;
      goto done;
    }
  } else if (type == INT64) {
    if (sqlite3_bind_int64(sqlite_statement, 1, *((int64_t*)value)) != SQLITE_OK) {
      ret = RC_STORAGE_FAILED_BINDING;
      goto done;
    }
  }

  {
    bind_execute_hash_params_t params = {.sqlite_statement = sqlite_statement, .hash_index = 2};

    if ((ret = hash243_set_for_each(hashes, (hash243_on_container_func)bind_execute_hash_do_func, &params)) != RC_OK) {
      goto done;
    }
  }

done:
  sqlite3_reset(sqlite_statement);
  if (ret != RC_OK && should_rollback_if_failed) {
    if ((ret_rollback = rollback_transaction(sqlite3_connection->db)) != RC_OK) {
      return ret_rollback;
    }
    return ret;
  }
  if ((ret = end_transaction(sqlite3_connection->db)) != RC_OK) {
    return ret;
  }

  return ret;
}

/*
 * Functors
 */

static retcode_t bind_execute_hash_do_func(bind_execute_hash_params_t* const params, flex_trit_t const* const hash) {
  int reset_ret = sqlite3_reset(params->sqlite_statement);

  if (reset_ret != SQLITE_DONE && reset_ret != SQLITE_OK) {
    return RC_STORAGE_FAILED_BINDING;
  }

  if (column_compress_bind(params->sqlite_statement, params->hash_index, hash, FLEX_TRIT_SIZE_243) != RC_OK) {
    return RC_STORAGE_FAILED_BINDING;
  }

  return execute_statement(params->sqlite_statement);
}

/*
 * Transaction operations
 */

static retcode_t execute_statement_load_transactions(sqlite3_stmt* const sqlite_statement,
                                                     iota_stor_pack_t* const pack) {
  return execute_statement_load_gen(sqlite_statement, pack, pack->capacity, MODEL_TRANSACTION);
}

static retcode_t execute_statement_load_transaction_for_essence_metadata(sqlite3_stmt* const sqlite_statement,
                                                                         iota_stor_pack_t* const pack) {
  return execute_statement_load_gen(sqlite_statement, pack, pack->capacity, MODEL_TRANSACTION_ESSENCE_METADATA);
}

static retcode_t execute_statement_load_transaction_for_essence_attachment_metadata(
    sqlite3_stmt* const sqlite_statement, iota_stor_pack_t* const pack) {
  return execute_statement_load_gen(sqlite_statement, pack, pack->capacity,
                                    MODEL_TRANSACTION_ESSENCE_ATTACHMENT_METADATA);
}

static retcode_t execute_statement_load_transaction_essence_consensus(sqlite3_stmt* const sqlite_statement,
                                                                      iota_stor_pack_t* const pack) {
  return execute_statement_load_gen(sqlite_statement, pack, pack->capacity, MODEL_TRANSACTION_ESSENCE_CONSENSUS);
}

static retcode_t execute_statement_load_transaction_metadata(sqlite3_stmt* const sqlite_statement,
                                                             iota_stor_pack_t* const pack) {
  return execute_statement_load_gen(sqlite_statement, pack, pack->capacity, MODEL_TRANSACTION_METADATA);
}

static void select_transactions_populate_essence(sqlite3_stmt* const statement, iota_transaction_t* const tx,
                                                 size_t* const index) {
  column_decompress_load(statement, (*index)++, tx->essence.address, FLEX_TRIT_SIZE_243);
  tx->loaded_columns_mask.essence |= MASK_ESSENCE_ADDRESS;
  transaction_set_value(tx, sqlite3_column_int64(statement, (*index)++));
  column_decompress_load(statement, (*index)++, tx->essence.obsolete_tag, FLEX_TRIT_SIZE_81);
  tx->loaded_columns_mask.essence |= MASK_ESSENCE_OBSOLETE_TAG;
  transaction_set_timestamp(tx, sqlite3_column_int64(statement, (*index)++));
  transaction_set_current_index(tx, sqlite3_column_int64(statement, (*index)++));
  transaction_set_last_index(tx, sqlite3_column_int64(statement, (*index)++));
  column_decompress_load(statement, (*index)++, tx->essence.bundle, FLEX_TRIT_SIZE_243);
  tx->loaded_columns_mask.essence |= MASK_ESSENCE_BUNDLE;
}

static void select_transactions_populate_attachment(sqlite3_stmt* const statement, iota_transaction_t* const tx,
                                                    size_t* const index) {
  column_decompress_load(statement, (*index)++, tx->attachment.trunk, FLEX_TRIT_SIZE_243);
  tx->loaded_columns_mask.attachment |= MASK_ATTACHMENT_TRUNK;
  column_decompress_load(statement, (*index)++, tx->attachment.branch, FLEX_TRIT_SIZE_243);
  tx->loaded_columns_mask.attachment |= MASK_ATTACHMENT_BRANCH;
  transaction_set_attachment_timestamp(tx, sqlite3_column_int64(statement, (*index)++));
  transaction_set_attachment_timestamp_lower(tx, sqlite3_column_int64(statement, (*index)++));
  transaction_set_attachment_timestamp_upper(tx, sqlite3_column_int64(statement, (*index)++));
  column_decompress_load(statement, (*index)++, tx->attachment.nonce, FLEX_TRIT_SIZE_81);
  tx->loaded_columns_mask.attachment |= MASK_ATTACHMENT_NONCE;
  column_decompress_load(statement, (*index)++, tx->attachment.tag, FLEX_TRIT_SIZE_81);
  tx->loaded_columns_mask.attachment |= MASK_ATTACHMENT_TAG;
}

static void select_transactions_populate_consensus(sqlite3_stmt* const statement, iota_transaction_t* const tx,
                                                   size_t* const index) {
  column_decompress_load(statement, (*index)++, tx->consensus.hash, FLEX_TRIT_SIZE_243);
  tx->loaded_columns_mask.consensus |= MASK_CONSENSUS_HASH;
}

static void select_transactions_populate_data(sqlite3_stmt* const statement, iota_transaction_t* const tx,
                                              size_t* const index) {
  column_decompress_load(statement, (*index)++, tx->data.signature_or_message, FLEX_TRIT_SIZE_6561);
  tx->loaded_columns_mask.data |= MASK_DATA_SIG_OR_MSG;
}

static void select_transactions_populate_metadata(sqlite3_stmt* const statement, iota_transaction_t* const tx,
                                                  size_t* const index) {
  transaction_set_snapshot_index(tx, sqlite3_column_int64(statement, (*index)++));
  transaction_set_solid(tx, sqlite3_column_int(statement, (*index)++));
  transaction_set_validity(tx, sqlite3_column_int(statement, (*index)++));
  transaction_set_arrival_timestamp(tx, sqlite3_column_int64(statement, (*index)++));
}

retcode_t storage_transaction_count(storage_connection_t const* const connection, size_t* const count) {
  sqlite3_tangle_connection_t const* sqlite3_connection = (sqlite3_tangle_connection_t*)connection->actual;
  retcode_t ret = RC_OK;
  sqlite3_stmt* sqlite_statement = sqlite3_connection->statements.transaction_count;
  int rc = sqlite3_step(sqlite_statement);

  if (rc == SQLITE_ROW) {
    *count = sqlite3_column_int64(sqlite_statement, 0);
  } else if (rc != SQLITE_OK && rc != SQLITE_DONE) {
    ret = RC_STORAGE_FAILED_STEP;
    goto done;
  }

done:
  sqlite3_reset(sqlite_statement);
  return ret;
}

retcode_t storage_transaction_store(storage_connection_t const* const connection, iota_transaction_t const* const tx) {
  sqlite3_tangle_connection_t const* sqlite3_connection = (sqlite3_tangle_connection_t*)connection->actual;
  retcode_t ret = RC_OK;
  sqlite3_stmt* sqlite_statement = sqlite3_connection->statements.transaction_insert;

  if (column_compress_bind(sqlite_statement, 1, tx->data.signature_or_message, FLEX_TRIT_SIZE_6561) != RC_OK ||
      column_compress_bind(sqlite_statement, 2, tx->essence.address, FLEX_TRIT_SIZE_243) != RC_OK ||
      sqlite3_bind_int64(sqlite_statement, 3, tx->essence.value) != SQLITE_OK ||
      column_compress_bind(sqlite_statement, 4, tx->essence.obsolete_tag, FLEX_TRIT_SIZE_81) != RC_OK ||
      sqlite3_bind_int64(sqlite_statement, 5, tx->essence.timestamp) != SQLITE_OK ||
      sqlite3_bind_int64(sqlite_statement, 6, tx->essence.current_index) != SQLITE_OK ||
      sqlite3_bind_int64(sqlite_statement, 7, tx->essence.last_index) != SQLITE_OK ||
      column_compress_bind(sqlite_statement, 8, tx->essence.bundle, FLEX_TRIT_SIZE_243) != RC_OK ||
      column_compress_bind(sqlite_statement, 9, tx->attachment.trunk, FLEX_TRIT_SIZE_243) != RC_OK ||
      column_compress_bind(sqlite_statement, 10, tx->attachment.branch, FLEX_TRIT_SIZE_243) != RC_OK ||
      column_compress_bind(sqlite_statement, 11, tx->attachment.tag, FLEX_TRIT_SIZE_81) != RC_OK ||
      sqlite3_bind_int64(sqlite_statement, 12, tx->attachment.attachment_timestamp) != SQLITE_OK ||
      sqlite3_bind_int64(sqlite_statement, 13, tx->attachment.attachment_timestamp_upper) != SQLITE_OK ||
      sqlite3_bind_int64(sqlite_statement, 14, tx->attachment.attachment_timestamp_lower) != SQLITE_OK ||
      column_compress_bind(sqlite_statement, 15, tx->attachment.nonce, FLEX_TRIT_SIZE_81) != RC_OK ||
      column_compress_bind(sqlite_statement, 16, tx->consensus.hash, FLEX_TRIT_SIZE_243) != RC_OK ||
      sqlite3_bind_int64(sqlite_statement, 17, current_timestamp_ms()) != SQLITE_OK) {
    ret = RC_STORAGE_FAILED_BINDING;
    goto done;
  }

  if ((ret = execute_statement(sqlite_statement)) != RC_OK) {
    goto done;
  }

done:
  sqlite3_reset(sqlite_statement);
  return ret;
}

retcode_t storage_transaction_load(storage_connection_t const* const connection,
                                   storage_transaction_field_t const field, flex_trit_t const* const key,
                                   iota_stor_pack_t* const pack) {
  sqlite3_tangle_connection_t const* sqlite3_connection = (sqlite3_tangle_connection_t*)connection->actual;
  retcode_t ret = RC_OK;
  sqlite3_stmt* sqlite_statement = NULL;
  size_t num_key_bytes;

  switch (field) {
    case TRANSACTION_FIELD_HASH:
      sqlite_statement = sqlite3_connection->statements.transaction_select_by_hash;
      num_key_bytes = FLEX_TRIT_SIZE_243;
      break;
    default:
      return RC_STORAGE_FAILED_NOT_IMPLEMENTED;
  }

  if (column_compress_bind(sqlite_statement, 1, key, num_key_bytes) != RC_OK) {
    ret = RC_STORAGE_FAILED_BINDING;
    goto done;
  }

  if ((ret = execute_statement_load_transactions(sqlite_statement, pack)) != RC_OK) {
    goto done;
  }

done:
  sqlite3_reset(sqlite_statement);
  return ret;
}

retcode_t storage_transaction_load_essence_metadata(storage_connection_t const* const connection,
                                                    flex_trit_t const* const hash, iota_stor_pack_t* const pack) {
  sqlite3_tangle_connection_t const* sqlite3_connection = (sqlite3_tangle_connection_t*)connection->actual;
  retcode_t ret = RC_OK;
  sqlite3_stmt* sqlite_statement = sqlite3_connection->statements.transaction_select_essence_metadata;

  if (column_compress_bind(sqlite_statement, 1, hash, FLEX_TRIT_SIZE_243) != RC_OK) {
    ret = RC_STORAGE_FAILED_BINDING;
    goto done;
  }

  if ((ret = execute_statement_load_transaction_for_essence_metadata(sqlite_statement, pack)) != RC_OK) {
    goto done;
  }

done:
  sqlite3_reset(sqlite_statement);
  return ret;
}

retcode_t storage_transaction_load_essence_attachment_metadata(storage_connection_t const* const connection,
                                                               flex_trit_t const* const hash,
                                                               iota_stor_pack_t* const pack) {
  sqlite3_tangle_connection_t const* sqlite3_connection = (sqlite3_tangle_connection_t*)connection->actual;
  retcode_t ret = RC_OK;
  sqlite3_stmt* sqlite_statement = sqlite3_connection->statements.transaction_select_essence_attachment_metadata;

  if (column_compress_bind(sqlite_statement, 1, hash, FLEX_TRIT_SIZE_243) != RC_OK) {
    ret = RC_STORAGE_FAILED_BINDING;
    goto done;
  }

  if ((ret = execute_statement_load_transaction_for_essence_attachment_metadata(sqlite_statement, pack)) != RC_OK) {
    goto done;
  }

done:
  sqlite3_reset(sqlite_statement);
  return ret;
}

retcode_t storage_transaction_load_essence_consensus(storage_connection_t const* const connection,
                                                     flex_trit_t const* const hash, iota_stor_pack_t* const pack) {
  sqlite3_tangle_connection_t const* sqlite3_connection = (sqlite3_tangle_connection_t*)connection->actual;
  retcode_t ret = RC_OK;
  sqlite3_stmt* sqlite_statement = sqlite3_connection->statements.transaction_select_essence_consensus;

  if (column_compress_bind(sqlite_statement, 1, hash, FLEX_TRIT_SIZE_243) != RC_OK) {
    ret = RC_STORAGE_FAILED_BINDING;
    goto done;
  }

  if ((ret = execute_statement_load_transaction_essence_consensus(sqlite_statement, pack)) != RC_OK) {
    goto done;
  }

done:
  sqlite3_reset(sqlite_statement);
  return ret;
}

retcode_t storage_transaction_load_metadata(storage_connection_t const* const connection, flex_trit_t const* const hash,
                                            iota_stor_pack_t* const pack) {
  sqlite3_tangle_connection_t const* sqlite3_connection = (sqlite3_tangle_connection_t*)connection->actual;
  retcode_t ret = RC_OK;
  sqlite3_stmt* sqlite_statement = sqlite3_connection->statements.transaction_select_metadata;

  if (column_compress_bind(sqlite_statement, 1, hash, FLEX_TRIT_SIZE_243) != RC_OK) {
    ret = RC_STORAGE_FAILED_BINDING;
    goto done;
  }

  if ((ret = execute_statement_load_transaction_metadata(sqlite_statement, pack)) != RC_OK) {
    goto done;
  }

done:
  sqlite3_reset(sqlite_statement);
  return ret;
}

retcode_t storage_transaction_load_hashes(storage_connection_t const* const connection,
                                          storage_transaction_field_t const field, flex_trit_t const* const key,
                                          iota_stor_pack_t* const pack) {
  sqlite3_tangle_connection_t const* sqlite3_connection = (sqlite3_tangle_connection_t*)connection->actual;
  retcode_t ret = RC_OK;
  size_t num_bytes_key;
  sqlite3_stmt* sqlite_statement = NULL;

  switch (field) {
    case TRANSACTION_FIELD_ADDRESS:
      sqlite_statement = sqlite3_connection->statements.transaction_select_hashes_by_address;
      num_bytes_key = FLEX_TRIT_SIZE_243;
      break;
    default:
      return RC_STORAGE_FAILED_NOT_IMPLEMENTED;
  }

  if (column_compress_bind(sqlite_statement, 1, key, num_bytes_key) != RC_OK) {
    ret = RC_STORAGE_FAILED_BINDING;
    goto done;
  }

  if ((ret = execute_statement_load_hashes(sqlite_statement, pack)) != RC_OK) {
    goto done;
  }

done:
  sqlite3_reset(sqlite_statement);
  return ret;
}

retcode_t storage_transaction_load_hashes_of_approvers(storage_connection_t const* const connection,
                                                       flex_trit_t const* const approvee_hash,
                                                       iota_stor_pack_t* const pack, int64_t before_timestamp) {
  sqlite3_tangle_connection_t const* sqlite3_connection = (sqlite3_tangle_connection_t*)connection->actual;
  retcode_t ret = RC_OK;
  sqlite3_stmt* sqlite_statement =
      before_timestamp != 0 ? sqlite3_connection->statements.transaction_select_hashes_of_approvers_before_date
                            : sqlite3_connection->statements.transaction_select_hashes_of_approvers;

  if (column_compress_bind(sqlite_statement, 1, approvee_hash, FLEX_TRIT_SIZE_243) != RC_OK ||
      column_compress_bind(sqlite_statement, 2, approvee_hash, FLEX_TRIT_SIZE_243) != RC_OK) {
    ret = RC_STORAGE_FAILED_BINDING;
    goto done;
  }

  if (before_timestamp != 0 && sqlite3_bind_int64(sqlite_statement, 3, before_timestamp) != SQLITE_OK) {
    ret = RC_STORAGE_FAILED_BINDING;
    goto done;
  }

  if ((ret = execute_statement_load_hashes(sqlite_statement, pack)) != RC_OK) {
    goto done;
  }

done:
  sqlite3_reset(sqlite_statement);
  return ret;
}

retcode_t storage_transaction_load_hashes_of_milestone_candidates(storage_connection_t const* const connection,
                                                                  iota_stor_pack_t* const pack,
                                                                  flex_trit_t const* const coordinator) {
  sqlite3_tangle_connection_t const* sqlite3_connection = (sqlite3_tangle_connection_t*)connection->actual;
  retcode_t ret = RC_OK;
  sqlite3_stmt* sqlite_statement = sqlite3_connection->statements.transaction_select_hashes_of_milestone_candidates;

  if (column_compress_bind(sqlite_statement, 1, coordinator, FLEX_TRIT_SIZE_243) != RC_OK) {
    ret = RC_STORAGE_FAILED_BINDING;
    goto done;
  }

  if ((ret = execute_statement_load_hashes(sqlite_statement, pack)) != RC_OK) {
    goto done;
  }

done:
  sqlite3_reset(sqlite_statement);
  return ret;
}

retcode_t storage_transaction_update_solid_state(storage_connection_t const* const connection,
                                                 flex_trit_t const* const hash, bool const is_solid) {
  sqlite3_tangle_connection_t const* sqlite3_connection = (sqlite3_tangle_connection_t*)connection->actual;
  retcode_t ret = RC_OK;
  sqlite3_stmt* sqlite_statement = sqlite3_connection->statements.transaction_update_solid_state;

  if (sqlite3_bind_int(sqlite_statement, 1, (int)is_solid) != SQLITE_OK ||
      column_compress_bind(sqlite_statement, 2, hash, FLEX_TRIT_SIZE_243) != RC_OK) {
    ret = RC_STORAGE_FAILED_BINDING;
    goto done;
  }

  if ((ret = execute_statement(sqlite_statement)) != RC_OK) {
    goto done;
  }

done:
  sqlite3_reset(sqlite_statement);
  return ret;
}

retcode_t storage_transactions_update_solid_state(storage_connection_t const* const connection,
                                                  hash243_set_t const hashes, bool const is_solid) {
  sqlite3_tangle_connection_t const* sqlite3_connection = (sqlite3_tangle_connection_t*)connection->actual;
  return update_transactions(connection, hashes, &is_solid,
                             sqlite3_connection->statements.transaction_update_solid_state, BOOLEAN);
}

retcode_t storage_transactions_update_snapshot_index(storage_connection_t const* const connection,
                                                     hash243_set_t const hashes, uint64_t const snapshot_index) {
  sqlite3_tangle_connection_t const* sqlite3_connection = (sqlite3_tangle_connection_t*)connection->actual;
  return update_transactions(connection, hashes, &snapshot_index,
                             sqlite3_connection->statements.transaction_update_snapshot_index, INT64);
}

retcode_t storage_transaction_update_snapshot_index(storage_connection_t const* const connection,
                                                    flex_trit_t const* const hash, uint64_t const snapshot_index) {
  sqlite3_tangle_connection_t const* sqlite3_connection = (sqlite3_tangle_connection_t*)connection->actual;
  retcode_t ret = RC_OK;
  sqlite3_stmt* sqlite_statement = sqlite3_connection->statements.transaction_update_snapshot_index;

  if (sqlite3_bind_int64(sqlite_statement, 1, snapshot_index) != SQLITE_OK ||
      column_compress_bind(sqlite_statement, 2, hash, FLEX_TRIT_SIZE_243) != RC_OK) {
    ret = RC_STORAGE_FAILED_BINDING;
    goto done;
  }

  if ((ret = execute_statement(sqlite_statement)) != RC_OK) {
    goto done;
  }

done:
  sqlite3_reset(sqlite_statement);
  return ret;
}

retcode_t storage_transaction_exist(storage_connection_t const* const connection,
                                    storage_transaction_field_t const field, flex_trit_t const* const key,
                                    bool* const exist) {
  sqlite3_tangle_connection_t const* sqlite3_connection = (sqlite3_tangle_connection_t*)connection->actual;
  retcode_t ret = RC_OK;
  sqlite3_stmt* sqlite_statement = NULL;
  size_t num_bytes_key;

  switch (field) {
    case TRANSACTION_FIELD_NONE:
      sqlite_statement = sqlite3_connection->statements.transaction_exist;
      break;
    case TRANSACTION_FIELD_HASH:
      sqlite_statement = sqlite3_connection->statements.transaction_exist_by_hash;
      num_bytes_key = FLEX_TRIT_SIZE_243;
      break;
    default:
      return RC_STORAGE_FAILED_NOT_IMPLEMENTED;
  }

  if (field != TRANSACTION_FIELD_NONE && key) {
    if (column_compress_bind(sqlite_statement, 1, (void*)key, num_bytes_key) != RC_OK) {
      ret = RC_STORAGE_FAILED_BINDING;
      goto done;
    }
  }

  if ((ret = execute_statement_exist(sqlite_statement, exist)) != RC_OK) {
    goto done;
  }

done:
  sqlite3_reset(sqlite_statement);
  return ret;
}

retcode_t storage_transaction_approvers_count(storage_connection_t const* const connection,
                                              flex_trit_t const* const hash, size_t* const count) {
  sqlite3_tangle_connection_t const* sqlite3_connection = (sqlite3_tangle_connection_t*)connection->actual;
  int rc = 0;
  retcode_t ret = RC_OK;
  sqlite3_stmt* sqlite_statement = sqlite3_connection->statements.transaction_approvers_count;

  if (column_compress_bind(sqlite_statement, 1, hash, FLEX_TRIT_SIZE_243) != RC_OK ||
      column_compress_bind(sqlite_statement, 2, hash, FLEX_TRIT_SIZE_243) != RC_OK) {
    ret = RC_STORAGE_FAILED_BINDING;
    goto done;
  }

  rc = sqlite3_step(sqlite_statement);
  if (rc == SQLITE_ROW) {
    *count = sqlite3_column_int64(sqlite_statement, 0);
  } else if (rc != SQLITE_OK && rc != SQLITE_DONE) {
    ret = RC_STORAGE_FAILED_STEP;
    goto done;
  }

done:
  sqlite3_reset(sqlite_statement);
  return ret;
}

retcode_t storage_transaction_find(storage_connection_t const* const connection, hash243_queue_t const bundles,
                                   hash243_queue_t const addresses, hash81_queue_t const tags,
                                   hash243_queue_t const approvees, iota_stor_pack_t* const pack) {
  sqlite3_tangle_connection_t const* sqlite3_connection = (sqlite3_tangle_connection_t*)connection->actual;
  retcode_t ret = RC_OK;
  sqlite3_stmt* sqlite_statement = NULL;
  size_t bundles_count = hash243_queue_count(bundles);
  size_t addresses_count = hash243_queue_count(addresses);
  size_t tags_count = hash81_queue_count(tags);
  size_t approvees_count = hash243_queue_count(approvees);
  hash243_queue_entry_t* iter243 = NULL;
  hash81_queue_entry_t* iter81 = NULL;
  size_t column = 1;

  char* statement =
      storage_statement_transaction_find_build(bundles_count, addresses_count, tags_count, approvees_count);

  if ((ret = prepare_statement(sqlite3_connection->db, &sqlite_statement, statement)) != RC_OK) {
    goto done;
  }

  if (sqlite3_bind_int(sqlite_statement, column++, !bundles_count) != SQLITE_OK) {
    ret = RC_STORAGE_FAILED_BINDING;
    goto done;
  }

  CDL_FOREACH(bundles, iter243) {
    if (column_compress_bind(sqlite_statement, column++, iter243->hash, FLEX_TRIT_SIZE_243) != RC_OK) {
      ret = RC_STORAGE_FAILED_BINDING;
      goto done;
    }
  }

  if (sqlite3_bind_int(sqlite_statement, column++, !addresses_count) != SQLITE_OK) {
    ret = RC_STORAGE_FAILED_BINDING;
    goto done;
  }

  CDL_FOREACH(addresses, iter243) {
    if (column_compress_bind(sqlite_statement, column++, iter243->hash, FLEX_TRIT_SIZE_243) != RC_OK) {
      ret = RC_STORAGE_FAILED_BINDING;
      goto done;
    }
  }

  if (sqlite3_bind_int(sqlite_statement, column++, !tags_count) != SQLITE_OK) {
    ret = RC_STORAGE_FAILED_BINDING;
    goto done;
  }

  CDL_FOREACH(tags, iter81) {
    if (column_compress_bind(sqlite_statement, column++, iter81->hash, FLEX_TRIT_SIZE_81) != RC_OK) {
      ret = RC_STORAGE_FAILED_BINDING;
      goto done;
    }
  }

  if (sqlite3_bind_int(sqlite_statement, column++, !approvees_count) != SQLITE_OK) {
    ret = RC_STORAGE_FAILED_BINDING;
    goto done;
  }

  CDL_FOREACH(approvees, iter243) {
    if (column_compress_bind(sqlite_statement, column, iter243->hash, FLEX_TRIT_SIZE_243) != RC_OK) {
      ret = RC_STORAGE_FAILED_BINDING;
      goto done;
    }
    if (column_compress_bind(sqlite_statement, column + approvees_count, iter243->hash, FLEX_TRIT_SIZE_243) != RC_OK) {
      ret = RC_STORAGE_FAILED_BINDING;
      goto done;
    }
    column++;
  }

  if ((ret = execute_statement_load_hashes(sqlite_statement, pack)) != RC_OK) {
    goto done;
  }

done:
  finalize_statement(sqlite_statement);
  free(statement);
  return ret;
}

retcode_t storage_transaction_metadata_clear(storage_connection_t const* const connection) {
  sqlite3_tangle_connection_t const* sqlite3_connection = (sqlite3_tangle_connection_t*)connection->actual;
  retcode_t ret = RC_OK;
  sqlite3_stmt* sqlite_statement = sqlite3_connection->statements.transaction_metadata_clear;

  if ((ret = execute_statement(sqlite_statement)) != RC_OK) {
    goto done;
  }

done:
  sqlite3_reset(sqlite_statement);
  return ret;
}

retcode_t storage_transactions_delete(storage_connection_t const* const connection, hash243_set_t const hashes) {
  sqlite3_tangle_connection_t const* sqlite3_connection = (sqlite3_tangle_connection_t*)connection->actual;
  retcode_t ret = RC_OK;
  retcode_t ret_rollback;
  bool should_rollback_if_failed = false;
  sqlite3_stmt* sqlite_statement = sqlite3_connection->statements.transaction_delete;

  if ((ret = begin_transaction(sqlite3_connection->db)) != RC_OK) {
    return ret;
  }

  should_rollback_if_failed = true;

  {
    bind_execute_hash_params_t params = {.sqlite_statement = sqlite_statement, .hash_index = 1};

    if ((ret = hash243_set_for_each(hashes, (hash243_on_container_func)bind_execute_hash_do_func, &params)) != RC_OK) {
      goto done;
    }
  }

done:
  sqlite3_reset(sqlite_statement);
  if (ret != RC_OK && should_rollback_if_failed) {
    if ((ret_rollback = rollback_transaction(sqlite3_connection->db)) != RC_OK) {
      return ret_rollback;
    }
    return ret;
  }
  if ((ret = end_transaction(sqlite3_connection->db)) != RC_OK) {
    return ret;
  }

  return ret;
}

/*
 * Bundle operations
 */

retcode_t storage_bundle_update_validity(storage_connection_t const* const connection,
                                         bundle_transactions_t const* const bundle, bundle_status_t const status) {
  sqlite3_tangle_connection_t const* sqlite3_connection = (sqlite3_tangle_connection_t*)connection->actual;
  retcode_t ret = RC_OK;
  retcode_t ret_rollback;
  bool should_rollback_if_failed = true;
  sqlite3_stmt* sqlite_statement = sqlite3_connection->statements.transaction_update_validity;

  if ((ret = begin_transaction(sqlite3_connection->db)) != RC_OK) {
    return ret;
  }

  if (sqlite3_bind_int(sqlite_statement, 1, (int)status) != SQLITE_OK) {
    ret = RC_STORAGE_FAILED_BINDING;
    goto done;
  }

  {
    iota_transaction_t* tx = NULL;
    bind_execute_hash_params_t params = {.sqlite_statement = sqlite_statement, .hash_index = 2};

    BUNDLE_FOREACH(bundle, tx) {
      if ((ret = bind_execute_hash_do_func(&params, transaction_hash(tx))) != RC_OK) {
        goto done;
      }
    }
  }

done:
  sqlite3_reset(sqlite_statement);
  if (ret != RC_OK && should_rollback_if_failed) {
    if ((ret_rollback = rollback_transaction(sqlite3_connection->db)) != RC_OK) {
      return ret_rollback;
    }
    return ret;
  }
  if ((ret = end_transaction(sqlite3_connection->db)) != RC_OK) {
    return ret;
  }

  return ret;
}

/*
 * Milestone operations
 */

static retcode_t execute_statement_load_milestones(sqlite3_stmt* const sqlite_statement, iota_stor_pack_t* const pack,
                                                   uint32_t const max_records) {
  return execute_statement_load_gen(sqlite_statement, pack, max_records, MODEL_MILESTONE);
}

static void select_milestones_populate_from_row(sqlite3_stmt* const statement, iota_milestone_t* const milestone) {
  milestone->index = sqlite3_column_int64(statement, 0);
  column_decompress_load(statement, 1, milestone->hash, FLEX_TRIT_SIZE_243);
}

retcode_t storage_milestone_clear(storage_connection_t const* const connection) {
  sqlite3_tangle_connection_t const* sqlite3_connection = (sqlite3_tangle_connection_t*)connection->actual;
  retcode_t ret = RC_OK;
  sqlite3_stmt* sqlite_statement = sqlite3_connection->statements.milestone_clear;

  if ((ret = execute_statement(sqlite_statement)) != RC_OK) {
    goto done;
  }

done:
  sqlite3_reset(sqlite_statement);
  return ret;
}

retcode_t storage_milestone_store(storage_connection_t const* const connection,
                                  iota_milestone_t const* const milestone) {
  sqlite3_tangle_connection_t const* sqlite3_connection = (sqlite3_tangle_connection_t*)connection->actual;
  assert(milestone);
  retcode_t ret = RC_OK;
  sqlite3_stmt* sqlite_statement = sqlite3_connection->statements.milestone_insert;

  if (sqlite3_bind_int64(sqlite_statement, 1, milestone->index) != SQLITE_OK ||
      column_compress_bind(sqlite_statement, 2, (flex_trit_t*)milestone->hash, FLEX_TRIT_SIZE_243) != RC_OK) {
    ret = RC_STORAGE_FAILED_BINDING;
    goto done;
  }

  if ((ret = execute_statement(sqlite_statement)) != RC_OK) {
    goto done;
  }

done:
  sqlite3_reset(sqlite_statement);
  return ret;
}

retcode_t storage_milestone_load(storage_connection_t const* const connection, flex_trit_t const* const hash,
                                 iota_stor_pack_t* const pack) {
  sqlite3_tangle_connection_t const* sqlite3_connection = (sqlite3_tangle_connection_t*)connection->actual;
  retcode_t ret = RC_OK;
  sqlite3_stmt* sqlite_statement = sqlite3_connection->statements.milestone_select_by_hash;

  if (column_compress_bind(sqlite_statement, 1, hash, FLEX_TRIT_SIZE_243) != RC_OK) {
    ret = RC_STORAGE_FAILED_BINDING;
    goto done;
  }

  if ((ret = execute_statement_load_milestones(sqlite_statement, pack, pack->capacity)) != RC_OK) {
    goto done;
  }

done:
  sqlite3_reset(sqlite_statement);
  return ret;
}

retcode_t storage_milestone_load_last(storage_connection_t const* const connection, iota_stor_pack_t* const pack) {
  sqlite3_tangle_connection_t const* sqlite3_connection = (sqlite3_tangle_connection_t*)connection->actual;
  retcode_t ret = RC_OK;
  sqlite3_stmt* sqlite_statement = sqlite3_connection->statements.milestone_select_last;

  if ((ret = execute_statement_load_milestones(sqlite_statement, pack, 1)) != RC_OK) {
    goto done;
  }

done:
  sqlite3_reset(sqlite_statement);
  return ret;
}

retcode_t storage_milestone_load_first(storage_connection_t const* const connection, iota_stor_pack_t* const pack) {
  sqlite3_tangle_connection_t const* sqlite3_connection = (sqlite3_tangle_connection_t*)connection->actual;
  retcode_t ret = RC_OK;
  sqlite3_stmt* sqlite_statement = sqlite3_connection->statements.milestone_select_first;

  if ((ret = execute_statement_load_milestones(sqlite_statement, pack, 1)) != RC_OK) {
    goto done;
  }

done:
  sqlite3_reset(sqlite_statement);
  return ret;
}

retcode_t storage_milestone_load_by_index(storage_connection_t const* const connection, uint64_t const index,
                                          iota_stor_pack_t* const pack) {
  sqlite3_tangle_connection_t const* sqlite3_connection = (sqlite3_tangle_connection_t*)connection->actual;
  retcode_t ret = RC_OK;
  sqlite3_stmt* sqlite_statement = sqlite3_connection->statements.milestone_select_by_index;

  if (sqlite3_bind_int(sqlite_statement, 1, index) != SQLITE_OK) {
    ret = RC_STORAGE_FAILED_BINDING;
    goto done;
  }

  if ((ret = execute_statement_load_milestones(sqlite_statement, pack, 1)) != RC_OK) {
    goto done;
  }

done:
  sqlite3_reset(sqlite_statement);
  return ret;
}

retcode_t storage_milestone_load_next(storage_connection_t const* const connection, uint64_t const index,
                                      iota_stor_pack_t* const pack) {
  sqlite3_tangle_connection_t const* sqlite3_connection = (sqlite3_tangle_connection_t*)connection->actual;
  retcode_t ret = RC_OK;
  sqlite3_stmt* sqlite_statement = sqlite3_connection->statements.milestone_select_next;

  if (sqlite3_bind_int(sqlite_statement, 1, index) != SQLITE_OK) {
    ret = RC_STORAGE_FAILED_BINDING;
    goto done;
  }

  if ((ret = execute_statement_load_milestones(sqlite_statement, pack, 1)) != RC_OK) {
    goto done;
  }

done:
  sqlite3_reset(sqlite_statement);
  return ret;
}

retcode_t storage_milestone_exist(storage_connection_t const* const connection, flex_trit_t const* const hash,
                                  bool* const exist) {
  sqlite3_tangle_connection_t const* sqlite3_connection = (sqlite3_tangle_connection_t*)connection->actual;
  retcode_t ret = RC_OK;
  sqlite3_stmt* sqlite_statement = NULL;

  if (hash) {
    sqlite_statement = sqlite3_connection->statements.milestone_exist_by_hash;
  } else {
    sqlite_statement = sqlite3_connection->statements.milestone_exist;
  }

  if (hash) {
    if (column_compress_bind(sqlite_statement, 1, hash, FLEX_TRIT_SIZE_243) != RC_OK) {
      ret = RC_STORAGE_FAILED_BINDING;
      goto done;
    }
  }

  if ((ret = execute_statement_exist(sqlite_statement, exist)) != RC_OK) {
    goto done;
  }

done:
  sqlite3_reset(sqlite_statement);
  return ret;
}

retcode_t storage_milestone_delete(storage_connection_t const* const connection, flex_trit_t const* const hash) {
  sqlite3_tangle_connection_t const* sqlite3_connection = (sqlite3_tangle_connection_t*)connection->actual;
  retcode_t ret = RC_OK;
  sqlite3_stmt* sqlite_statement = NULL;

  sqlite_statement = sqlite3_connection->statements.milestone_delete_by_hash;

  if (column_compress_bind(sqlite_statement, 1, hash, FLEX_TRIT_SIZE_243) != RC_OK) {
    ret = RC_STORAGE_FAILED_BINDING;
    goto done;
  }

  if ((ret = execute_statement(sqlite_statement)) != RC_OK) {
    goto done;
  }

done:
  sqlite3_reset(sqlite_statement);
  return ret;
}

/*
 * State delta operations
 */

retcode_t storage_state_delta_store(storage_connection_t const* const connection, uint64_t const index,
                                    state_delta_t const* const delta) {
  sqlite3_tangle_connection_t const* sqlite3_connection = (sqlite3_tangle_connection_t*)connection->actual;
  retcode_t ret = RC_OK;
  size_t size = 0;
  byte_t* bytes = NULL;
  sqlite3_stmt* sqlite_statement = sqlite3_connection->statements.state_delta_store;

  size = state_delta_serialized_size(delta);
  if ((bytes = (byte_t*)calloc(size, sizeof(byte_t))) == NULL) {
    ret = RC_OOM;
    goto done;
  }

  if ((ret = state_delta_serialize(delta, bytes)) != RC_OK) {
    goto done;
  }

  if (sqlite3_bind_blob(sqlite_statement, 1, bytes, size, NULL) != SQLITE_OK ||
      sqlite3_bind_int(sqlite_statement, 2, index) != SQLITE_OK) {
    ret = RC_STORAGE_FAILED_BINDING;
    goto done;
  }

  if ((ret = execute_statement(sqlite_statement))) {
    goto done;
  }

done:
  sqlite3_reset(sqlite_statement);
  if (bytes) {
    free(bytes);
  }
  return ret;
}

retcode_t storage_state_delta_load(storage_connection_t const* const connection, uint64_t const index,
                                   state_delta_t* const delta) {
  sqlite3_tangle_connection_t const* sqlite3_connection = (sqlite3_tangle_connection_t*)connection->actual;
  retcode_t ret = RC_OK;
  byte_t* bytes = NULL;
  size_t size = 0;
  int rc = 0;
  sqlite3_stmt* sqlite_statement = sqlite3_connection->statements.state_delta_load;

  *delta = NULL;

  if (sqlite3_bind_int(sqlite_statement, 1, index) != SQLITE_OK) {
    ret = RC_STORAGE_FAILED_BINDING;
    goto done;
  }

  rc = sqlite3_step(sqlite_statement);
  if (rc == SQLITE_ROW) {
    bytes = (byte_t*)sqlite3_column_blob(sqlite_statement, 0);
    size = sqlite3_column_bytes(sqlite_statement, 0);
    if ((ret = state_delta_deserialize(bytes, size, delta)) != RC_OK) {
      goto done;
    }
  } else if (rc != SQLITE_OK && rc != SQLITE_DONE) {
    ret = RC_STORAGE_FAILED_STEP;
  }

done:
  sqlite3_reset(sqlite_statement);
  return ret;
}

/*
 * Spent address operations
 */

retcode_t storage_spent_address_store(storage_connection_t const* const connection, flex_trit_t const* const address) {
  retcode_t ret = RC_OK;
  sqlite3_spent_addresses_connection_t const* sqlite3_connection =
      (sqlite3_spent_addresses_connection_t*)connection->actual;
  sqlite3_stmt* sqlite_statement = sqlite3_connection->statements.spent_address_insert;

  if (column_compress_bind(sqlite_statement, 1, address, FLEX_TRIT_SIZE_243) != RC_OK) {
    ret = RC_STORAGE_FAILED_BINDING;
    goto done;
  }

  if ((ret = execute_statement(sqlite_statement)) != RC_OK) {
    goto done;
  }

done:
  sqlite3_reset(sqlite_statement);
  return ret;
}

retcode_t storage_spent_addresses_store(storage_connection_t const* const connection, hash243_set_t const addresses) {
  retcode_t ret = RC_OK;
  sqlite3_spent_addresses_connection_t const* sqlite3_connection =
      (sqlite3_spent_addresses_connection_t*)connection->actual;
  sqlite3_stmt* sqlite_statement = sqlite3_connection->statements.spent_address_insert;
  retcode_t ret_rollback;
  bool should_rollback_if_failed = true;

  if ((ret = begin_transaction(sqlite3_connection->db)) != RC_OK) {
    return ret;
  }

  {
    bind_execute_hash_params_t params = {.sqlite_statement = sqlite_statement, .hash_index = 1};

    if ((ret = hash243_set_for_each(addresses, (hash243_on_container_func)bind_execute_hash_do_func, &params)) !=
        RC_OK) {
      goto done;
    }
  }

done:
  sqlite3_reset(sqlite_statement);
  if (ret != RC_OK && should_rollback_if_failed) {
    if ((ret_rollback = rollback_transaction(sqlite3_connection->db)) != RC_OK) {
      return ret_rollback;
    }
    return ret;
  }
  if ((ret = end_transaction(sqlite3_connection->db)) != RC_OK) {
    return ret;
  }

  return ret;
}

retcode_t storage_spent_address_exist(storage_connection_t const* const connection, flex_trit_t const* const address,
                                      bool* const exist) {
  retcode_t ret = RC_OK;
  sqlite3_spent_addresses_connection_t const* sqlite3_connection =
      (sqlite3_spent_addresses_connection_t*)connection->actual;
  sqlite3_stmt* sqlite_statement = sqlite3_connection->statements.spent_address_exist;

  if (column_compress_bind(sqlite_statement, 1, address, FLEX_TRIT_SIZE_243) != RC_OK) {
    ret = RC_STORAGE_FAILED_BINDING;
    goto done;
  }

  if ((ret = execute_statement_exist(sqlite_statement, exist)) != RC_OK) {
    goto done;
  }

done:
  sqlite3_reset(sqlite_statement);
  return ret;
}
