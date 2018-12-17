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
 * Declarations/Definitions
 */

typedef struct bind_execute_hash_params_s {
  sqlite3_stmt* sqlite_statement;
} bind_execute_hash_params_t;

static retcode_t bind_execute_hash_do_func(
    bind_execute_hash_params_t* const params, flex_trit_t const* const hash);

/*
 * BEGIN / END transaction
 */

static retcode_t begin_transaction(sqlite3* const db) {
  if ((sqlite3_exec(db, "BEGIN TRANSACTION;", NULL, NULL, NULL)) != SQLITE_OK) {
    return RC_SQLITE3_FAILED_BEGIN;
  }
  return RC_OK;
}

static retcode_t end_transaction(sqlite3* const db) {
  if (sqlite3_exec(db, "END TRANSACTION;", NULL, NULL, NULL) != SQLITE_OK) {
    return RC_SQLITE3_FAILED_END;
  }
  return RC_OK;
}

static retcode_t rollback_transaction(sqlite3* const db) {
  if (sqlite3_exec(db, "ROLLBACK TRANSACTION;", NULL, NULL, NULL) !=
      SQLITE_OK) {
    return RC_SQLITE3_FAILED_ROLLBACK;
  }
  return RC_OK;
}

/*
 * Binding functions
 */

static retcode_t binding_error() {
  log_error(SQLITE3_LOGGER_ID, "Binding failed\n");
  return RC_SQLITE3_FAILED_BINDING;
}

static retcode_t column_compress_bind(sqlite3_stmt* const statement,
                                      size_t const index,
                                      flex_trit_t const* const flex_trits,
                                      size_t const num_bytes) {
  ssize_t i = num_bytes - 1;
  int rc = 0;

  for (; i >= 0 && flex_trits[i] == FLEX_TRIT_NULL_VALUE; --i)
    ;
  if ((rc = sqlite3_bind_blob(statement, index, flex_trits, i + 1, NULL)) !=
      SQLITE_OK) {
    return RC_SQLITE3_FAILED_BINDING;
  }
  return RC_OK;
}

static void column_decompress_load(sqlite3_stmt* const statement,
                                   size_t const index,
                                   flex_trit_t* const flex_trits,
                                   size_t const num_bytes) {
  char const* buffer = NULL;
  size_t column_size = 0;

  if ((buffer = sqlite3_column_blob(statement, index))) {
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

static void select_milestones_populate_from_row(
    sqlite3_stmt* const statement, iota_milestone_t* const milestone);

static void select_transactions_populate_from_row(sqlite3_stmt* const statement,
                                                  iota_transaction_t const tx);

static void
select_transactions_populate_from_row_essence_attachment_and_metadata(
    sqlite3_stmt* const statement, iota_transaction_t const tx);

static void select_transactions_populate_from_row_essence_and_consensus(
    sqlite3_stmt* const statement, iota_transaction_t const tx);

static void select_transactions_populate_from_row_metadata(
    sqlite3_stmt* const statement, iota_transaction_t const tx);

static retcode_t prepare_statement(sqlite3* const db,
                                   sqlite3_stmt** const sqlite_statement,
                                   char const* const statement) {
  char const* err_msg = NULL;
  int rc = 0;

  if ((rc = sqlite3_prepare_v2(db, statement, -1, sqlite_statement,
                               &err_msg)) != SQLITE_OK) {
    log_error(SQLITE3_LOGGER_ID,
              "Preparing statement failed with sqlite3 code: %" PRIu64 "\n",
              rc);
    return RC_SQLITE3_FAILED_PREPARED_STATEMENT;
  }

  return RC_OK;
}

static retcode_t finalize_statement(sqlite3_stmt* const sqlite_statement) {
  int rc = 0;

  if (sqlite_statement == NULL) {
    return RC_OK;
  }

  if ((rc = sqlite3_finalize(sqlite_statement)) != SQLITE_OK) {
    log_error(SQLITE3_LOGGER_ID,
              "Failed in finalizing, sqlite3 code is: %" PRIu64 "\n", rc);
    return RC_SQLITE3_FAILED_FINALIZE;
  }

  return RC_OK;
}

enum load_model {
  MODEL_TRANSACTION,
  MODEL_HASH,
  MODEL_MILESTONE,
  MODEL_TRANSACTION_ESSENCE_ATTACHMENT_METADATA,
  MODEL_TRANSACTION_MODEL_ESSENCE_CONSENSUS,
  MODEL_TRANSACTION_MODEL_METADATA,
};

static retcode_t execute_statement_load_gen(
    sqlite3_stmt* const sqlite_statement, iota_stor_pack_t* const pack,
    uint32_t const max_records, enum load_model const model) {
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
    } else if (model == MODEL_TRANSACTION_ESSENCE_ATTACHMENT_METADATA) {
      select_transactions_populate_from_row_essence_attachment_and_metadata(
          sqlite_statement, pack->models[pack->num_loaded++]);
    } else if (model == MODEL_TRANSACTION_MODEL_ESSENCE_CONSENSUS) {
      select_transactions_populate_from_row_essence_and_consensus(
          sqlite_statement, pack->models[pack->num_loaded++]);
    } else if (model == MODEL_TRANSACTION_MODEL_METADATA) {
      select_transactions_populate_from_row_metadata(
          sqlite_statement, pack->models[pack->num_loaded++]);
    } else {
      return RC_SQLITE3_FAILED_NOT_IMPLEMENTED;
    }
  }

  return RC_OK;
}

static retcode_t execute_statement_load_solid_state(
    sqlite3_stmt* const sqlite_statement, bool* const is_solid) {
  int rc = sqlite3_step(sqlite_statement);
  if (rc == SQLITE_ROW) {
    *is_solid = sqlite3_column_int(sqlite_statement, 0);
  } else {
    log_error(SQLITE3_LOGGER_ID, "Step failed with sqlite3 code: %" PRIu64 "\n",
              rc);
    return RC_SQLITE3_FAILED_STEP;
  }

  return RC_OK;
}

static retcode_t execute_statement_load_hashes(
    sqlite3_stmt* const sqlite_statement, iota_stor_pack_t* const pack) {
  return execute_statement_load_gen(sqlite_statement, pack, pack->capacity,
                                    MODEL_HASH);
}

static retcode_t execute_statement_store_update(
    sqlite3_stmt* const sqlite_statement) {
  int rc = sqlite3_step(sqlite_statement);
  if (rc != SQLITE_OK && rc != SQLITE_DONE) {
    log_error(SQLITE3_LOGGER_ID, "Step failed with sqlite3 code: %" PRIu64 "\n",
              rc);
    return RC_SQLITE3_FAILED_STEP;
  }

  return RC_OK;
}

static retcode_t execute_statement_exist(sqlite3_stmt* const sqlite_statement,
                                         bool* const exist) {
  int rc = sqlite3_step(sqlite_statement);
  *exist = false;

  if (rc == SQLITE_ROW) {
    *exist = true;
  } else if (rc != SQLITE_OK && rc != SQLITE_DONE) {
    log_error(SQLITE3_LOGGER_ID, "Step failed with sqlite3 code: %" PRIu64 "\n",
              rc);
    return RC_SQLITE3_FAILED_STEP;
  }

  return RC_OK;
}

enum value_type {
  BOOLEAN,
  INT64,
};

retcode_t update_transactions(connection_t const* const conn,
                              hash243_set_t const hashes,
                              void const* const value,
                              char const* const statement,
                              enum value_type const type) {
  retcode_t ret = RC_OK;
  retcode_t ret_rollback;
  sqlite3_stmt* sqlite_statement = NULL;
  bool should_rollback_if_failed = false;

  if ((ret = begin_transaction((sqlite3*)conn->db)) != RC_OK) {
    return ret;
  }

  if ((ret = prepare_statement((sqlite3*)conn->db, &sqlite_statement,
                               statement)) != RC_OK) {
    goto done;
  }

  should_rollback_if_failed = true;
  if (type == BOOLEAN) {
    int value_int = *((bool*)value);
    if (sqlite3_bind_int(sqlite_statement, 1, value_int) != SQLITE_OK) {
      ret = binding_error();
      goto done;
    }
  } else if (type == INT64) {
    if (sqlite3_bind_int64(sqlite_statement, 1, *((int64_t*)value)) !=
        SQLITE_OK) {
      ret = binding_error();
      goto done;
    }
  }

  bind_execute_hash_params_t params = {.sqlite_statement = sqlite_statement};
  if ((ret = hash243_set_for_each(
           &hashes, (hash243_on_container_func)bind_execute_hash_do_func,
           &params)) != RC_OK) {
    goto done;
  }

done:

  finalize_statement(sqlite_statement);
  if (ret != RC_OK && should_rollback_if_failed) {
    if ((ret_rollback = rollback_transaction((sqlite3*)conn->db)) != RC_OK) {
      return ret_rollback;
    }
    return ret;
  }
  if ((ret = end_transaction((sqlite3*)conn->db)) != RC_OK) {
    return ret;
  }

  return ret;
}

/*
 * Functors
 */

static retcode_t bind_execute_hash_do_func(
    bind_execute_hash_params_t* const params, flex_trit_t const* const hash) {
  int reset_ret = sqlite3_reset(params->sqlite_statement);

  if (reset_ret != SQLITE_DONE && reset_ret != SQLITE_OK) {
    return binding_error();
  }

  if (column_compress_bind(params->sqlite_statement, 2, hash,
                           FLEX_TRIT_SIZE_243) != RC_OK) {
    return binding_error();
  }

  return execute_statement_store_update(params->sqlite_statement);
}

/*
 * Transaction operations
 */

static retcode_t execute_statement_load_transactions(
    sqlite3_stmt* const sqlite_statement, iota_stor_pack_t* const pack) {
  return execute_statement_load_gen(sqlite_statement, pack, pack->capacity,
                                    MODEL_TRANSACTION);
}

static retcode_t
execute_statement_load_transaction_for_essence_attachment_and_metadata(
    sqlite3_stmt* const sqlite_statement, iota_stor_pack_t* const pack) {
  return execute_statement_load_gen(
      sqlite_statement, pack, pack->capacity,
      MODEL_TRANSACTION_ESSENCE_ATTACHMENT_METADATA);
}

static retcode_t execute_statement_load_transaction_essence_and_consensus(
    sqlite3_stmt* const sqlite_statement, iota_stor_pack_t* const pack) {
  return execute_statement_load_gen(sqlite_statement, pack, pack->capacity,
                                    MODEL_TRANSACTION_MODEL_ESSENCE_CONSENSUS);
}

static retcode_t execute_statement_load_transaction_metadata(
    sqlite3_stmt* const sqlite_statement, iota_stor_pack_t* const pack) {
  return execute_statement_load_gen(sqlite_statement, pack, pack->capacity,
                                    MODEL_TRANSACTION_MODEL_METADATA);
}

static void select_transactions_populate_from_row(sqlite3_stmt* const statement,
                                                  iota_transaction_t const tx) {
  column_decompress_load(statement, 0, tx->data.signature_or_message,
                         FLEX_TRIT_SIZE_6561);
  column_decompress_load(statement, 1, tx->essence.address, FLEX_TRIT_SIZE_243);
  transaction_set_value(tx, sqlite3_column_int64(statement, 2));
  column_decompress_load(statement, 3, tx->essence.obsolete_tag,
                         FLEX_TRIT_SIZE_81);
  transaction_set_timestamp(tx, sqlite3_column_int64(statement, 4));
  transaction_set_current_index(tx, sqlite3_column_int64(statement, 5));
  transaction_set_last_index(tx, sqlite3_column_int64(statement, 6));
  column_decompress_load(statement, 7, tx->essence.bundle, FLEX_TRIT_SIZE_243);
  column_decompress_load(statement, 8, tx->attachment.trunk,
                         FLEX_TRIT_SIZE_243);
  column_decompress_load(statement, 9, tx->attachment.branch,
                         FLEX_TRIT_SIZE_243);
  column_decompress_load(statement, 10, tx->attachment.tag, FLEX_TRIT_SIZE_81);
  transaction_set_attachment_timestamp(tx, sqlite3_column_int64(statement, 11));
  transaction_set_attachment_timestamp_upper(
      tx, sqlite3_column_int64(statement, 12));
  transaction_set_attachment_timestamp_lower(
      tx, sqlite3_column_int64(statement, 13));
  column_decompress_load(statement, 14, tx->attachment.nonce,
                         FLEX_TRIT_SIZE_81);
  column_decompress_load(statement, 15, tx->consensus.hash, FLEX_TRIT_SIZE_243);
  tx->loaded_columns_mask = (MASK_ALL_COLUMNS & (~MASK_METADATA));
}

void select_transactions_populate_from_row_essence_attachment_and_metadata(
    sqlite3_stmt* const statement, iota_transaction_t const tx) {
  column_decompress_load(statement, 0, tx->essence.address, FLEX_TRIT_SIZE_243);
  transaction_set_value(tx, sqlite3_column_int64(statement, 1));
  column_decompress_load(statement, 2, tx->essence.obsolete_tag,
                         FLEX_TRIT_SIZE_81);
  transaction_set_timestamp(tx, sqlite3_column_int64(statement, 3));
  transaction_set_current_index(tx, sqlite3_column_int64(statement, 4));
  transaction_set_last_index(tx, sqlite3_column_int64(statement, 5));
  column_decompress_load(statement, 6, tx->essence.bundle, FLEX_TRIT_SIZE_243);
  column_decompress_load(statement, 7, tx->attachment.trunk,
                         FLEX_TRIT_SIZE_243);
  column_decompress_load(statement, 8, tx->attachment.branch,
                         FLEX_TRIT_SIZE_243);
  transaction_set_attachment_timestamp(tx, sqlite3_column_int64(statement, 9));
  transaction_set_attachment_timestamp_upper(
      tx, sqlite3_column_int64(statement, 10));
  transaction_set_attachment_timestamp_lower(
      tx, sqlite3_column_int64(statement, 11));
  column_decompress_load(statement, 12, tx->attachment.nonce,
                         FLEX_TRIT_SIZE_81);
  column_decompress_load(statement, 13, tx->attachment.tag, FLEX_TRIT_SIZE_81);
  transaction_set_snapshot_index(tx, sqlite3_column_int64(statement, 14));
  transaction_set_solid(tx, sqlite3_column_int(statement, 15));
  tx->loaded_columns_mask = MASK_ESSENCE | MASK_ATTACHMENT | MASK_METADATA;
}

void select_transactions_populate_from_row_essence_and_consensus(
    sqlite3_stmt* const statement, iota_transaction_t const tx) {
  column_decompress_load(statement, 0, tx->essence.address, FLEX_TRIT_SIZE_243);
  transaction_set_value(tx, sqlite3_column_int64(statement, 1));
  column_decompress_load(statement, 2, tx->essence.obsolete_tag,
                         FLEX_TRIT_SIZE_81);
  transaction_set_timestamp(tx, sqlite3_column_int64(statement, 3));
  transaction_set_current_index(tx, sqlite3_column_int64(statement, 4));
  transaction_set_last_index(tx, sqlite3_column_int64(statement, 5));
  column_decompress_load(statement, 6, tx->essence.bundle, FLEX_TRIT_SIZE_243);
  column_decompress_load(statement, 7, tx->consensus.hash, FLEX_TRIT_SIZE_243);

  tx->loaded_columns_mask = MASK_ESSENCE | MASK_CONSENSUS;
}

void select_transactions_populate_from_row_metadata(
    sqlite3_stmt* const statement, iota_transaction_t const tx) {
  transaction_set_snapshot_index(tx, sqlite3_column_int64(statement, 0));
  transaction_set_solid(tx, sqlite3_column_int(statement, 1));
  tx->loaded_columns_mask = MASK_METADATA;
}

retcode_t iota_stor_transaction_count(connection_t const* const conn,
                                      size_t* const count) {
  retcode_t ret = RC_OK;
  sqlite3_stmt* sqlite_statement = NULL;
  int rc = 0;

  if ((ret = prepare_statement((sqlite3*)conn->db, &sqlite_statement,
                               iota_statement_transaction_count)) != RC_OK) {
    goto done;
  }

  rc = sqlite3_step(sqlite_statement);

  if (rc == SQLITE_ROW) {
    *count = sqlite3_column_int64(sqlite_statement, 0);
  } else if (rc != SQLITE_OK && rc != SQLITE_DONE) {
    log_error(SQLITE3_LOGGER_ID, "Step failed with sqlite3 code: %" PRIu64 "\n",
              rc);
    ret = RC_SQLITE3_FAILED_STEP;
    goto done;
  }

done:
  finalize_statement(sqlite_statement);
  return ret;
}

retcode_t iota_stor_transaction_store(connection_t const* const conn,
                                      iota_transaction_t const tx) {
  retcode_t ret = RC_OK;
  sqlite3_stmt* sqlite_statement = NULL;

  if ((ret = prepare_statement((sqlite3*)conn->db, &sqlite_statement,
                               iota_statement_transaction_insert)) != RC_OK) {
    goto done;
  }

  if (column_compress_bind(sqlite_statement, 1, tx->data.signature_or_message,
                           FLEX_TRIT_SIZE_6561) != RC_OK ||
      column_compress_bind(sqlite_statement, 2, tx->essence.address,
                           FLEX_TRIT_SIZE_243) != RC_OK ||
      sqlite3_bind_int64(sqlite_statement, 3, tx->essence.value) != SQLITE_OK ||
      column_compress_bind(sqlite_statement, 4, tx->essence.obsolete_tag,
                           FLEX_TRIT_SIZE_81) != RC_OK ||
      sqlite3_bind_int64(sqlite_statement, 5, tx->essence.timestamp) !=
          SQLITE_OK ||
      sqlite3_bind_int64(sqlite_statement, 6, tx->essence.current_index) !=
          SQLITE_OK ||
      sqlite3_bind_int64(sqlite_statement, 7, tx->essence.last_index) !=
          SQLITE_OK ||
      column_compress_bind(sqlite_statement, 8, tx->essence.bundle,
                           FLEX_TRIT_SIZE_243) != RC_OK ||
      column_compress_bind(sqlite_statement, 9, tx->attachment.trunk,
                           FLEX_TRIT_SIZE_243) != RC_OK ||
      column_compress_bind(sqlite_statement, 10, tx->attachment.branch,
                           FLEX_TRIT_SIZE_243) != RC_OK ||
      column_compress_bind(sqlite_statement, 11, tx->attachment.tag,
                           FLEX_TRIT_SIZE_81) != RC_OK ||
      sqlite3_bind_int64(sqlite_statement, 12,
                         tx->attachment.attachment_timestamp) != SQLITE_OK ||
      sqlite3_bind_int64(sqlite_statement, 13,
                         tx->attachment.attachment_timestamp_upper) !=
          SQLITE_OK ||
      sqlite3_bind_int64(sqlite_statement, 14,
                         tx->attachment.attachment_timestamp_lower) !=
          SQLITE_OK ||
      column_compress_bind(sqlite_statement, 15, tx->attachment.nonce,
                           FLEX_TRIT_SIZE_81) != RC_OK ||
      column_compress_bind(sqlite_statement, 16, tx->consensus.hash,
                           FLEX_TRIT_SIZE_243) != RC_OK) {
    ret = binding_error();
    goto done;
  }

  if ((ret = execute_statement_store_update(sqlite_statement))) {
    goto done;
  }

done:
  finalize_statement(sqlite_statement);
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
      return RC_SQLITE3_FAILED_NOT_IMPLEMENTED;
  }

  if ((ret = prepare_statement((sqlite3*)conn->db, &sqlite_statement,
                               statement)) != RC_OK) {
    goto done;
  }

  if (column_compress_bind(sqlite_statement, 1, key->trits, key->num_bytes) !=
      RC_OK) {
    ret = binding_error();
    goto done;
  }

  if ((ret = execute_statement_load_transactions(sqlite_statement, pack)) !=
      RC_OK) {
    goto done;
  }

done:
  finalize_statement(sqlite_statement);
  return ret;
}

retcode_t iota_stor_transaction_load_essence_attachment_and_metadata(
    connection_t const* const conn, flex_trit_t const* const hash,
    iota_stor_pack_t* const pack) {
  retcode_t ret = RC_OK;
  char* statement = NULL;
  sqlite3_stmt* sqlite_statement = NULL;

  statement = iota_statement_transaction_select_essence_attachment_and_metadata;

  if ((ret = prepare_statement((sqlite3*)conn->db, &sqlite_statement,
                               statement)) != RC_OK) {
    goto done;
  }

  if (column_compress_bind(sqlite_statement, 1, hash, FLEX_TRIT_SIZE_243) !=
      RC_OK) {
    ret = binding_error();
    goto done;
  }

  if ((ret =
           execute_statement_load_transaction_for_essence_attachment_and_metadata(
               sqlite_statement, pack)) != RC_OK) {
    goto done;
  }

done:
  finalize_statement(sqlite_statement);
  return ret;
}

retcode_t iota_stor_transaction_load_essence_and_consensus(
    connection_t const* const conn, flex_trit_t const* const hash,
    iota_stor_pack_t* const pack) {
  retcode_t ret = RC_OK;
  char* statement = NULL;
  sqlite3_stmt* sqlite_statement = NULL;

  statement = iota_statement_transaction_select_essence_and_consensus;

  if ((ret = prepare_statement((sqlite3*)conn->db, &sqlite_statement,
                               statement)) != RC_OK) {
    goto done;
  }

  if (column_compress_bind(sqlite_statement, 1, hash, FLEX_TRIT_SIZE_243) !=
      RC_OK) {
    ret = binding_error();
    goto done;
  }

  if ((ret = execute_statement_load_transaction_essence_and_consensus(
           sqlite_statement, pack)) != RC_OK) {
    goto done;
  }

done:
  finalize_statement(sqlite_statement);
  return ret;
}

retcode_t iota_stor_transaction_load_metadata(connection_t const* const conn,
                                              flex_trit_t const* const hash,
                                              iota_stor_pack_t* const pack) {
  retcode_t ret = RC_OK;
  char* statement = NULL;
  sqlite3_stmt* sqlite_statement = NULL;

  statement = iota_statement_transaction_select_metadata;

  if ((ret = prepare_statement((sqlite3*)conn->db, &sqlite_statement,
                               statement)) != RC_OK) {
    goto done;
  }

  if (column_compress_bind(sqlite_statement, 1, hash, FLEX_TRIT_SIZE_243) !=
      RC_OK) {
    ret = binding_error();
    goto done;
  }

  if ((ret = execute_statement_load_transaction_metadata(sqlite_statement,
                                                         pack)) != RC_OK) {
    goto done;
  }

done:
  finalize_statement(sqlite_statement);
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
      return RC_SQLITE3_FAILED_NOT_IMPLEMENTED;
      break;
  }

  if ((ret = prepare_statement((sqlite3*)conn->db, &sqlite_statement,
                               statement)) != RC_OK) {
    goto done;
  }

  if (column_compress_bind(sqlite_statement, 1, key->trits, key->num_bytes) !=
      RC_OK) {
    ret = binding_error();
    goto done;
  }

  if ((ret = execute_statement_load_hashes(sqlite_statement, pack)) != RC_OK) {
    goto done;
  }

done:
  finalize_statement(sqlite_statement);
  return ret;
}

retcode_t iota_stor_transaction_load_hashes_of_approvers(
    connection_t const* const conn, flex_trit_t const* const approvee_hash,
    iota_stor_pack_t* const pack) {
  retcode_t ret = RC_OK;
  sqlite3_stmt* sqlite_statement = NULL;

  if ((ret = prepare_statement(
           (sqlite3*)conn->db, &sqlite_statement,
           iota_statement_transaction_select_hashes_of_approvers)) != RC_OK) {
    goto done;
  }

  if (column_compress_bind(sqlite_statement, 1, approvee_hash,
                           FLEX_TRIT_SIZE_243) != RC_OK ||
      column_compress_bind(sqlite_statement, 2, approvee_hash,
                           FLEX_TRIT_SIZE_243) != RC_OK) {
    ret = binding_error();
    goto done;
  }

  if ((ret = execute_statement_load_hashes(sqlite_statement, pack)) != RC_OK) {
    goto done;
  }

done:
  finalize_statement(sqlite_statement);
  return ret;
}

retcode_t iota_stor_transaction_load_hashes_of_requests(
    connection_t const* const conn, iota_stor_pack_t* const pack,
    size_t const limit) {
  retcode_t ret = RC_OK;
  sqlite3_stmt* sqlite_statement = NULL;

  if ((ret = prepare_statement(
           (sqlite3*)conn->db, &sqlite_statement,
           iota_statement_transaction_select_hashes_of_transactions_to_request)) !=
      RC_OK) {
    goto done;
  }

  if (sqlite3_bind_int(sqlite_statement, 1, limit) != SQLITE_OK) {
    ret = binding_error();
    goto done;
  }

  if ((ret = execute_statement_load_hashes(sqlite_statement, pack)) != RC_OK) {
    goto done;
  }

done:
  finalize_statement(sqlite_statement);
  return ret;
}

retcode_t iota_stor_transaction_load_hashes_of_tips(
    connection_t const* const conn, iota_stor_pack_t* const pack,
    size_t const limit) {
  retcode_t ret = RC_OK;
  sqlite3_stmt* sqlite_statement = NULL;

  if ((ret = prepare_statement(
           (sqlite3*)conn->db, &sqlite_statement,
           iota_statement_transaction_select_hashes_of_tips)) != RC_OK) {
    goto done;
  }

  if (sqlite3_bind_int(sqlite_statement, 1, limit) != SQLITE_OK) {
    ret = binding_error();
    goto done;
  }

  if ((ret = execute_statement_load_hashes(sqlite_statement, pack)) != RC_OK) {
    goto done;
  }

done:
  finalize_statement(sqlite_statement);
  return ret;
}

retcode_t iota_stor_transaction_load_hashes_of_milestone_candidates(
    connection_t const* const conn, iota_stor_pack_t* const pack,
    flex_trit_t const* const coordinator) {
  retcode_t ret = RC_OK;
  sqlite3_stmt* sqlite_statement = NULL;

  if ((ret = prepare_statement(
           (sqlite3*)conn->db, &sqlite_statement,
           iota_statement_transaction_select_hashes_of_milestone_candidates)) !=
      RC_OK) {
    goto done;
  }

  if (column_compress_bind(sqlite_statement, 1, coordinator,
                           FLEX_TRIT_SIZE_243) != RC_OK) {
    ret = binding_error();
    goto done;
  }

  if ((ret = execute_statement_load_hashes(sqlite_statement, pack)) != RC_OK) {
    goto done;
  }

done:
  finalize_statement(sqlite_statement);
  return ret;
}

retcode_t iota_stor_transaction_update_solid_state(
    connection_t const* const conn, flex_trit_t const* const hash,
    bool const is_solid) {
  retcode_t ret = RC_OK;
  sqlite3_stmt* sqlite_statement = NULL;

  if ((ret = prepare_statement(
           (sqlite3*)conn->db, &sqlite_statement,
           iota_statement_transaction_update_solid_state)) != RC_OK) {
    goto done;
  }

  if (sqlite3_bind_int(sqlite_statement, 1, (int)is_solid) != SQLITE_OK ||
      column_compress_bind(sqlite_statement, 2, hash, FLEX_TRIT_SIZE_243) !=
          RC_OK) {
    ret = binding_error();
    goto done;
  }

  if ((ret = execute_statement_store_update(sqlite_statement)) != RC_OK) {
    goto done;
  }

done:
  finalize_statement(sqlite_statement);
  return ret;
}

retcode_t iota_stor_transactions_update_solid_state(
    connection_t const* const conn, hash243_set_t const hashes,
    bool const is_solid) {
  return update_transactions(conn, hashes, &is_solid,
                             iota_statement_transaction_update_solid_state,
                             BOOLEAN);
}

retcode_t iota_stor_transactions_update_snapshot_index(
    connection_t const* const conn, hash243_set_t const hashes,
    uint64_t const snapshot_index) {
  return update_transactions(conn, hashes, &snapshot_index,
                             iota_statement_transaction_update_snapshot_index,
                             INT64);
}

retcode_t iota_stor_transaction_update_snapshot_index(
    connection_t const* const conn, flex_trit_t const* const hash,
    uint64_t const snapshot_index) {
  retcode_t ret = RC_OK;
  sqlite3_stmt* sqlite_statement = NULL;

  if ((ret = prepare_statement(
           (sqlite3*)conn->db, &sqlite_statement,
           iota_statement_transaction_update_snapshot_index)) != RC_OK) {
    goto done;
  }

  if (sqlite3_bind_int64(sqlite_statement, 1, snapshot_index) != SQLITE_OK ||
      column_compress_bind(sqlite_statement, 2, hash, FLEX_TRIT_SIZE_243) !=
          RC_OK) {
    ret = binding_error();
    goto done;
  }

  if ((ret = execute_statement_store_update(sqlite_statement)) != RC_OK) {
    goto done;
  }

done:
  finalize_statement(sqlite_statement);
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
      return RC_SQLITE3_FAILED_NOT_IMPLEMENTED;
      break;
  }

  if ((ret = prepare_statement((sqlite3*)conn->db, &sqlite_statement,
                               statement)) != RC_OK) {
    goto done;
  }

  if (field != TRANSACTION_FIELD_NONE && key) {
    if (column_compress_bind(sqlite_statement, 1, (void*)key->trits,
                             key->num_bytes) != RC_OK) {
      ret = binding_error();
      goto done;
    }
  }

  if ((ret = execute_statement_exist(sqlite_statement, exist)) != RC_OK) {
    goto done;
  }

done:
  finalize_statement(sqlite_statement);
  return ret;
}

retcode_t iota_stor_transaction_approvers_count(connection_t const* const conn,
                                                flex_trit_t const* const hash,
                                                size_t* const count) {
  retcode_t ret = RC_OK;
  sqlite3_stmt* sqlite_statement = NULL;
  int rc = 0;

  if ((ret = prepare_statement((sqlite3*)conn->db, &sqlite_statement,
                               iota_statement_transaction_approvers_count)) !=
      RC_OK) {
    goto done;
  }

  if (column_compress_bind(sqlite_statement, 1, hash, FLEX_TRIT_SIZE_243) !=
          RC_OK ||
      column_compress_bind(sqlite_statement, 2, hash, FLEX_TRIT_SIZE_243) !=
          RC_OK) {
    ret = binding_error();
    goto done;
  }

  rc = sqlite3_step(sqlite_statement);
  if (rc == SQLITE_ROW) {
    *count = sqlite3_column_int64(sqlite_statement, 0);
  } else if (rc != SQLITE_OK && rc != SQLITE_DONE) {
    log_error(SQLITE3_LOGGER_ID, "Step failed with sqlite3 code: %" PRIu64 "\n",
              rc);
    ret = RC_SQLITE3_FAILED_STEP;
    goto done;
  }

done:
  finalize_statement(sqlite_statement);
  return ret;
}

static char* iota_stor_transaction_find_in_clause(size_t const count) {
  char* in_clause = calloc(2 * count, 1);
  size_t offset = 0;

  if (count != 0) {
    for (size_t i = 0; i < count; i++) {
      offset += sprintf(in_clause + offset, "?,");
    }
    in_clause[offset - 1] = '\0';
  }
  return in_clause;
}

static char* iota_stor_transaction_find_build_query(
    size_t const bundles_count, size_t const addresses_count,
    size_t const tags_count, size_t const approvees_count) {
  // Base size of the query + enough space for '?' (bindings)
  size_t statement_size = 328 + 2 * bundles_count + 2 * addresses_count +
                          2 * tags_count + 4 * approvees_count;
  char* statement = malloc(statement_size);

  char* bundles_in_clause = iota_stor_transaction_find_in_clause(bundles_count);
  char* addresses_in_clause =
      iota_stor_transaction_find_in_clause(addresses_count);
  char* tags_in_clause = iota_stor_transaction_find_in_clause(tags_count);
  char* approvees_in_clause =
      iota_stor_transaction_find_in_clause(approvees_count);

  snprintf(statement, statement_size, iota_statement_transaction_find,
           bundles_in_clause, addresses_in_clause, tags_in_clause,
           approvees_in_clause, approvees_in_clause);

  free(bundles_in_clause);
  free(addresses_in_clause);
  free(tags_in_clause);
  free(approvees_in_clause);

  return statement;
}

retcode_t iota_stor_transaction_find(connection_t const* const conn,
                                     hash243_queue_t const bundles,
                                     hash243_queue_t const addresses,
                                     hash81_queue_t const tags,
                                     hash243_queue_t const approvees,
                                     iota_stor_pack_t* const pack) {
  retcode_t ret = RC_OK;
  sqlite3_stmt* sqlite_statement = NULL;
  size_t bundles_count = hash243_queue_count(bundles);
  size_t addresses_count = hash243_queue_count(addresses);
  size_t tags_count = hash81_queue_count(tags);
  size_t approvees_count = hash243_queue_count(approvees);
  hash243_queue_entry_t* iter243 = NULL;
  hash81_queue_entry_t* iter81 = NULL;
  size_t column = 1;

  char* statement = iota_stor_transaction_find_build_query(
      bundles_count, addresses_count, tags_count, approvees_count);

  if ((ret = prepare_statement((sqlite3*)conn->db, &sqlite_statement,
                               statement)) != RC_OK) {
    goto done;
  }

  if (sqlite3_bind_int(sqlite_statement, column++, !bundles_count) !=
      SQLITE_OK) {
    ret = binding_error();
    goto done;
  }

  CDL_FOREACH(bundles, iter243) {
    if (column_compress_bind(sqlite_statement, column++, iter243->hash,
                             FLEX_TRIT_SIZE_243) != RC_OK) {
      ret = binding_error();
      goto done;
    }
  }

  if (sqlite3_bind_int(sqlite_statement, column++, !addresses_count) !=
      SQLITE_OK) {
    ret = binding_error();
    goto done;
  }

  CDL_FOREACH(addresses, iter243) {
    if (column_compress_bind(sqlite_statement, column++, iter243->hash,
                             FLEX_TRIT_SIZE_243) != RC_OK) {
      ret = binding_error();
      goto done;
    }
  }

  if (sqlite3_bind_int(sqlite_statement, column++, !tags_count) != SQLITE_OK) {
    ret = binding_error();
    goto done;
  }

  CDL_FOREACH(tags, iter81) {
    if (column_compress_bind(sqlite_statement, column++, iter81->hash,
                             FLEX_TRIT_SIZE_81) != RC_OK) {
      ret = binding_error();
      goto done;
    }
  }

  if (sqlite3_bind_int(sqlite_statement, column++, !approvees_count) !=
      SQLITE_OK) {
    ret = binding_error();
    goto done;
  }

  CDL_FOREACH(approvees, iter243) {
    if (column_compress_bind(sqlite_statement, column, iter243->hash,
                             FLEX_TRIT_SIZE_243) != RC_OK) {
      ret = binding_error();
      goto done;
    }
    if (column_compress_bind(sqlite_statement, column + approvees_count,
                             iter243->hash, FLEX_TRIT_SIZE_243) != RC_OK) {
      ret = binding_error();
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
                               iota_statement_milestone_insert)) != RC_OK) {
    goto done;
  }

  if (sqlite3_bind_int64(sqlite_statement, 1, milestone->index) != SQLITE_OK ||
      column_compress_bind(sqlite_statement, 2, (flex_trit_t*)milestone->hash,
                           FLEX_TRIT_SIZE_243) != RC_OK) {
    ret = binding_error();
    goto done;
  }

  if ((ret = execute_statement_store_update(sqlite_statement)) != RC_OK) {
    goto done;
  }

done:
  finalize_statement(sqlite_statement);
  return ret;
}

retcode_t iota_stor_milestone_load(connection_t const* const conn,
                                   flex_trit_t const* const hash,
                                   iota_stor_pack_t* const pack) {
  retcode_t ret = RC_OK;
  sqlite3_stmt* sqlite_statement = NULL;

  if ((ret = prepare_statement((sqlite3*)conn->db, &sqlite_statement,
                               iota_statement_milestone_select_by_hash)) !=
      RC_OK) {
    goto done;
  }

  if (column_compress_bind(sqlite_statement, 1, hash, FLEX_TRIT_SIZE_243) !=
      RC_OK) {
    ret = binding_error();
    goto done;
  }

  if ((ret = execute_statement_load_milestones(sqlite_statement, pack,
                                               pack->capacity)) != RC_OK) {
    goto done;
  }

done:
  finalize_statement(sqlite_statement);
  return ret;
}

retcode_t iota_stor_milestone_load_first(connection_t const* const conn,
                                         iota_stor_pack_t* const pack) {
  retcode_t ret = RC_OK;
  sqlite3_stmt* sqlite_statement = NULL;

  if ((ret = prepare_statement((sqlite3*)conn->db, &sqlite_statement,
                               iota_statement_milestone_select_first)) !=
      RC_OK) {
    goto done;
  }

  if ((ret = execute_statement_load_milestones(sqlite_statement, pack, 1)) !=
      RC_OK) {
    goto done;
  }

done:
  finalize_statement(sqlite_statement);
  return ret;
}

retcode_t iota_stor_milestone_load_last(connection_t const* const conn,
                                        iota_stor_pack_t* const pack) {
  retcode_t ret = RC_OK;
  sqlite3_stmt* sqlite_statement = NULL;

  if ((ret = prepare_statement((sqlite3*)conn->db, &sqlite_statement,
                               iota_statement_milestone_select_last)) !=
      RC_OK) {
    goto done;
  }

  if ((ret = execute_statement_load_milestones(sqlite_statement, pack, 1)) !=
      RC_OK) {
    goto done;
  }

done:
  finalize_statement(sqlite_statement);
  return ret;
}

retcode_t iota_stor_milestone_load_next(connection_t const* const conn,
                                        uint64_t const index,
                                        iota_stor_pack_t* const pack) {
  retcode_t ret = RC_OK;
  sqlite3_stmt* sqlite_statement = NULL;

  if ((ret = prepare_statement((sqlite3*)conn->db, &sqlite_statement,
                               iota_statement_milestone_select_next)) !=
      RC_OK) {
    goto done;
  }

  if (sqlite3_bind_int(sqlite_statement, 1, index) != SQLITE_OK) {
    ret = binding_error();
    goto done;
  }

  if ((ret = execute_statement_load_milestones(sqlite_statement, pack, 1)) !=
      RC_OK) {
    goto done;
  }

done:
  finalize_statement(sqlite_statement);
  return ret;
}

retcode_t iota_stor_milestone_exist(connection_t const* const conn,
                                    flex_trit_t const* const hash,
                                    bool* const exist) {
  retcode_t ret = RC_OK;
  char* statement = NULL;
  sqlite3_stmt* sqlite_statement = NULL;

  if (hash) {
    statement = iota_statement_milestone_exist_by_hash;
  } else {
    statement = iota_statement_milestone_exist;
  }

  if ((ret = prepare_statement((sqlite3*)conn->db, &sqlite_statement,
                               statement)) != RC_OK) {
    goto done;
  }

  if (hash) {
    if (column_compress_bind(sqlite_statement, 1, hash, FLEX_TRIT_SIZE_243) !=
        RC_OK) {
      ret = binding_error();
      goto done;
    }
  }

  if ((ret = execute_statement_exist(sqlite_statement, exist)) != RC_OK) {
    goto done;
  }

done:
  finalize_statement(sqlite_statement);
  return ret;
}

/*
 * State delta operations
 */

retcode_t iota_stor_state_delta_store(connection_t const* const conn,
                                      uint64_t const index,
                                      state_delta_t const* const delta) {
  retcode_t ret = RC_OK;
  sqlite3_stmt* sqlite_statement = NULL;
  size_t size = 0;
  byte_t* bytes = NULL;

  if ((ret = prepare_statement((sqlite3*)conn->db, &sqlite_statement,
                               iota_statement_state_delta_store))) {
    goto done;
  }

  size = state_delta_serialized_size(delta);
  if ((bytes = calloc(size, sizeof(byte_t))) == NULL) {
    ret = RC_STORAGE_OOM;
    goto done;
  }

  if ((ret = state_delta_serialize(delta, bytes)) != RC_OK) {
    goto done;
  }

  if (sqlite3_bind_blob(sqlite_statement, 1, bytes, size, NULL) != SQLITE_OK ||
      sqlite3_bind_int(sqlite_statement, 2, index) != SQLITE_OK) {
    ret = binding_error();
    goto done;
  }

  if ((ret = execute_statement_store_update(sqlite_statement))) {
    goto done;
  }

done:
  if (bytes) {
    free(bytes);
  }
  finalize_statement(sqlite_statement);
  return ret;
}

retcode_t iota_stor_state_delta_load(connection_t const* const conn,
                                     uint64_t const index,
                                     state_delta_t* const delta) {
  retcode_t ret = RC_OK;
  sqlite3_stmt* sqlite_statement = NULL;
  byte_t* bytes = NULL;
  size_t size = 0;
  int rc = 0;

  *delta = NULL;

  if ((ret = prepare_statement((sqlite3*)conn->db, &sqlite_statement,
                               iota_statement_state_delta_load))) {
    goto done;
  }

  if (sqlite3_bind_int(sqlite_statement, 1, index) != SQLITE_OK) {
    ret = binding_error();
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
    ret = RC_SQLITE3_FAILED_STEP;
  }

done:
  finalize_statement(sqlite_statement);
  return ret;
}
