/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <inttypes.h>
#include <stdio.h>
#include <string.h>

#include "common/model/milestone.h"
#include "common/model/transaction.h"
#include "common/storage/sql/defs.h"
#include "common/storage/sql/statements.h"
#include "common/trinary/trit_array.h"
#include "utils/logger_helper.h"

#define SQL_STATEMENTS_ID "sql_statements"

/*
 * Generic statements
 */

static retcode_t iota_statement_generic_select(const char *table_name,
                                               const char *select_col,
                                               const char *index_col,
                                               char statement[],
                                               size_t statement_cap) {
  int res;

  if (index_col == NULL || strcmp(index_col, "") == 0) {
    res = snprintf(statement, statement_cap, "SELECT %s FROM %s", select_col,
                   table_name);
  } else {
    res = snprintf(statement, statement_cap, "SELECT %s FROM %s WHERE %s = ?",
                   select_col, table_name, index_col);
  }

  if (res < 0 || res == statement_cap) {
    log_error(SQL_STATEMENTS_ID,
              "Failed in creating statement, statement: %s\n", statement);
    return RC_SQL_FAILED_WRITE_STATEMENT;
  }
  return RC_OK;
}

static retcode_t iota_statement_generic_exist(const char *table_name,
                                              const char *index_col,
                                              char statement[],
                                              size_t statement_cap) {
  int res;

  if (index_col == NULL || strcmp(index_col, "") == 0) {
    res = snprintf(statement, statement_cap,
                   "SELECT '1' WHERE EXISTS(SELECT 1 FROM %s)", table_name);
  } else {
    res = snprintf(statement, statement_cap,
                   "SELECT '1' WHERE EXISTS(SELECT 1 FROM %s WHERE %s = ?)",
                   table_name, index_col);
  }

  if (res < 0 || res == statement_cap) {
    log_error(SQL_STATEMENTS_ID,
              "Failed in creating statement, statement: %s\n", statement);
    return RC_SQL_FAILED_WRITE_STATEMENT;
  }
  return RC_OK;
}

/*
 * Transaction statements
 */

retcode_t iota_statement_transaction_insert(const iota_transaction_t tx,
                                            char statement[],
                                            size_t statement_cap) {
  int res = snprintf(
      statement, statement_cap,
      "INSERT INTO %s (%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s) "
      "VALUES (?,?,%" PRIu64 ",?,%" PRIu64 ",%" PRIu64 ",%" PRIu64
      ",?,?,?,?,%" PRIu64 ",%" PRIu64 ",%" PRIu64 ",?,?)",
      TRANSACTION_TABLE_NAME, TRANSACTION_COL_SIG_OR_MSG,
      TRANSACTION_COL_ADDRESS, TRANSACTION_COL_VALUE,
      TRANSACTION_COL_OBSOLETE_TAG, TRANSACTION_COL_TIMESTAMP,
      TRANSACTION_COL_CURRENT_INDEX, TRANSACTION_COL_LAST_INDEX,
      TRANSACTION_COL_BUNDLE, TRANSACTION_COL_TRUNK, TRANSACTION_COL_BRANCH,
      TRANSACTION_COL_TAG, TRANSACTION_COL_ATTACHMENT_TIMESTAMP,
      TRANSACTION_COL_ATTACHMENT_TIMESTAMP_UPPER,
      TRANSACTION_COL_ATTACHMENT_TIMESTAMP_LOWER, TRANSACTION_COL_NONCE,
      TRANSACTION_COL_HASH, tx->value, tx->timestamp, tx->current_index,
      tx->last_index, tx->attachment_timestamp, tx->attachment_timestamp_upper,
      tx->attachment_timestamp_lower);

  if (res < 0 || res == statement_cap) {
    log_error(SQL_STATEMENTS_ID,
              "Failed in creating statement, statement: %s\n", statement);
    return RC_SQL_FAILED_WRITE_STATEMENT;
  }
  return RC_OK;
}

retcode_t iota_statement_transaction_select(const char *index_col,
                                            char statement[],
                                            size_t statement_cap) {
  return iota_statement_generic_select(TRANSACTION_TABLE_NAME, "*", index_col,
                                       statement, statement_cap);
}

retcode_t iota_statement_transaction_exist(const char *index_col,
                                           char statement[],
                                           size_t statement_cap) {
  return iota_statement_generic_exist(TRANSACTION_TABLE_NAME, index_col,
                                      statement, statement_cap);
}

retcode_t iota_statement_transaction_update(const char *index_col,
                                            const trit_array_p key,
                                            const iota_transaction_t tx,
                                            char statement[],
                                            size_t statement_cap) {
  return RC_OK;
}

retcode_t iota_statement_transaction_select_hashes(const char *index_col,
                                                   char statement[],
                                                   size_t statement_cap) {
  return iota_statement_generic_select(TRANSACTION_TABLE_NAME,
                                       TRANSACTION_COL_HASH, index_col,
                                       statement, statement_cap);
}

retcode_t iota_statement_transaction_select_hashes_approvers(
    const trit_array_p approvee_hash, char statement[], size_t statement_cap) {
  int res;

  char key_str[approvee_hash->num_bytes + 1];
  memcpy(key_str, approvee_hash->trits, approvee_hash->num_bytes);
  key_str[approvee_hash->num_bytes] = 0;
  res =
      snprintf(statement, statement_cap, "SELECT %s FROM %s WHERE %s=? OR %s=?",
               TRANSACTION_COL_HASH, TRANSACTION_TABLE_NAME,
               TRANSACTION_COL_BRANCH, TRANSACTION_COL_TRUNK);

  if (res < 0 || res == statement_cap) {
    log_error(SQL_STATEMENTS_ID,
              "Failed in creating statement, statement: %s\n", statement);
    return RC_SQL_FAILED_WRITE_STATEMENT;
  }

  return RC_OK;
}

/*
 * Milestone statements
 */

retcode_t iota_statement_milestone_insert(const iota_milestone_t *milestone,
                                          char statement[],
                                          size_t statement_cap) {
  int res = snprintf(statement, statement_cap,
                     "INSERT INTO %s (%s,%s) VALUES (%" PRIu64 ", ?)",
                     MILESTONE_TABLE_NAME, MILESTONE_COL_INDEX,
                     MILESTONE_COL_HASH, milestone->index);

  if (res < 0 || res == statement_cap) {
    log_error(SQL_STATEMENTS_ID,
              "Failed in creating statement, statement: %s\n", statement);
    return RC_SQL_FAILED_WRITE_STATEMENT;
  }
  return RC_OK;
}

retcode_t iota_statement_milestone_select(const char *index_col,
                                          char statement[],
                                          size_t statement_cap) {
  return iota_statement_generic_select(MILESTONE_TABLE_NAME, "*", index_col,
                                       statement, statement_cap);
}

retcode_t iota_statement_milestone_exist(const char *index_col,
                                         char statement[],
                                         size_t statement_cap) {
  return iota_statement_generic_exist(MILESTONE_TABLE_NAME, index_col,
                                      statement, statement_cap);
}
