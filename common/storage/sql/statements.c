/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common/storage/defs.h"

/*
 * Generic statement builders
 */

char *iota_statement_in_clause_build(size_t const count) {
  char *in_clause = (char *)calloc(2 * count + 1, 1);
  size_t offset = 0;

  if (count != 0) {
    for (size_t i = 0; i < count; i++) {
      offset += sprintf(in_clause + offset, "?,");
    }
    in_clause[offset - 1] = '\0';
  }
  return in_clause;
}

/*
 * Transaction statements
 */

char *iota_statement_transaction_insert =
    "INSERT INTO " TRANSACTION_TABLE_NAME "(" TRANSACTION_COL_SIG_OR_MSG "," TRANSACTION_COL_ADDRESS
    "," TRANSACTION_COL_VALUE "," TRANSACTION_COL_OBSOLETE_TAG "," TRANSACTION_COL_TIMESTAMP
    "," TRANSACTION_COL_CURRENT_INDEX "," TRANSACTION_COL_LAST_INDEX "," TRANSACTION_COL_BUNDLE
    "," TRANSACTION_COL_TRUNK "," TRANSACTION_COL_BRANCH "," TRANSACTION_COL_TAG
    "," TRANSACTION_COL_ATTACHMENT_TIMESTAMP "," TRANSACTION_COL_ATTACHMENT_TIMESTAMP_UPPER
    "," TRANSACTION_COL_ATTACHMENT_TIMESTAMP_LOWER "," TRANSACTION_COL_NONCE "," TRANSACTION_COL_HASH
    "," TRANSACTION_COL_ARRIVAL_TIME ")VALUES(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)";

char *iota_statement_transaction_select_by_hash =
    "SELECT " TRANSACTION_COL_SIG_OR_MSG "," TRANSACTION_COL_ADDRESS "," TRANSACTION_COL_VALUE
    "," TRANSACTION_COL_OBSOLETE_TAG "," TRANSACTION_COL_TIMESTAMP "," TRANSACTION_COL_CURRENT_INDEX
    "," TRANSACTION_COL_LAST_INDEX "," TRANSACTION_COL_BUNDLE "," TRANSACTION_COL_TRUNK "," TRANSACTION_COL_BRANCH
    "," TRANSACTION_COL_TAG "," TRANSACTION_COL_ATTACHMENT_TIMESTAMP "," TRANSACTION_COL_ATTACHMENT_TIMESTAMP_UPPER
    "," TRANSACTION_COL_ATTACHMENT_TIMESTAMP_LOWER "," TRANSACTION_COL_NONCE "," TRANSACTION_COL_HASH
    " FROM " TRANSACTION_TABLE_NAME " WHERE " TRANSACTION_COL_HASH "=?";

char *iota_statement_transaction_select_hashes_by_address =
    "SELECT " TRANSACTION_COL_HASH " FROM " TRANSACTION_TABLE_NAME " WHERE " TRANSACTION_COL_ADDRESS "=?";

char *iota_statement_transaction_select_hashes_of_approvers =
    "SELECT " TRANSACTION_COL_HASH " FROM " TRANSACTION_TABLE_NAME " WHERE " TRANSACTION_COL_BRANCH
    "=? OR " TRANSACTION_COL_TRUNK "=?";

char *iota_statement_transaction_select_hashes_of_approvers_before_date =
    "SELECT " TRANSACTION_COL_HASH " FROM " TRANSACTION_TABLE_NAME " WHERE (" TRANSACTION_COL_BRANCH
    "=? OR " TRANSACTION_COL_TRUNK "=?) AND " TRANSACTION_COL_ARRIVAL_TIME "<?";

char *iota_statement_transaction_select_hashes_of_transactions_to_request =
    "SELECT " TRANSACTION_COL_TRUNK " FROM " TRANSACTION_TABLE_NAME
    " a WHERE NOT(EXISTS(SELECT 1 FROM " TRANSACTION_TABLE_NAME " b WHERE b." TRANSACTION_COL_HASH
    " = a." TRANSACTION_COL_TRUNK ")) UNION SELECT " TRANSACTION_COL_BRANCH " FROM " TRANSACTION_TABLE_NAME
    " a WHERE NOT(EXISTS(SELECT 1 FROM " TRANSACTION_TABLE_NAME " b WHERE b." TRANSACTION_COL_HASH
    " = a." TRANSACTION_COL_BRANCH ")) LIMIT ?";

char *iota_statement_transaction_select_hashes_of_tips =
    "SELECT " TRANSACTION_COL_HASH " FROM " TRANSACTION_TABLE_NAME
    " a WHERE NOT(EXISTS(SELECT 1 FROM " TRANSACTION_TABLE_NAME " b WHERE b." TRANSACTION_COL_TRUNK
    " = a." TRANSACTION_COL_HASH " OR b." TRANSACTION_COL_BRANCH " = a." TRANSACTION_COL_HASH ")) LIMIT ?";

char *iota_statement_transaction_select_hashes_of_milestone_candidates =
    "SELECT " TRANSACTION_COL_HASH " FROM " TRANSACTION_TABLE_NAME " WHERE " TRANSACTION_COL_ADDRESS
    " LIKE ? EXCEPT SELECT " MILESTONE_COL_HASH " FROM " MILESTONE_TABLE_NAME;

char *iota_statement_transaction_update_snapshot_index =
    "UPDATE " TRANSACTION_TABLE_NAME " SET " TRANSACTION_COL_SNAPSHOT_INDEX "=? WHERE " TRANSACTION_COL_HASH "=?";

char *iota_statement_transaction_update_solid_state =
    "UPDATE " TRANSACTION_TABLE_NAME " SET " TRANSACTION_COL_SOLID "=? WHERE " TRANSACTION_COL_HASH "=?";

char *iota_statement_transaction_exist =
    "SELECT 1 WHERE EXISTS(SELECT 1 "
    "FROM " TRANSACTION_TABLE_NAME ")";

char *iota_statement_transaction_exist_by_hash =
    "SELECT 1 WHERE EXISTS(SELECT 1 "
    "FROM " TRANSACTION_TABLE_NAME " WHERE " TRANSACTION_COL_HASH "=?)";

char *iota_statement_transaction_approvers_count =
    "SELECT COUNT(*) FROM " TRANSACTION_TABLE_NAME " WHERE branch=? OR trunk=?";

char *iota_statement_transaction_count = "SELECT COUNT(*) FROM " TRANSACTION_TABLE_NAME;

char *iota_statement_transaction_find =
    "SELECT a." TRANSACTION_COL_HASH " FROM " TRANSACTION_TABLE_NAME " a JOIN " TRANSACTION_TABLE_NAME
    " b ON a." TRANSACTION_COL_HASH "=b." TRANSACTION_COL_HASH " AND (? OR b." TRANSACTION_COL_BUNDLE
    " IN(%s)) JOIN " TRANSACTION_TABLE_NAME " c ON b." TRANSACTION_COL_HASH "=c." TRANSACTION_COL_HASH
    " AND (? OR c." TRANSACTION_COL_ADDRESS " IN(%s)) JOIN " TRANSACTION_TABLE_NAME " d ON c." TRANSACTION_COL_HASH
    "=d." TRANSACTION_COL_HASH " AND (? OR d." TRANSACTION_COL_TAG " IN(%s)) JOIN " TRANSACTION_TABLE_NAME
    " e ON d." TRANSACTION_COL_HASH "=e." TRANSACTION_COL_HASH " AND (? OR (e." TRANSACTION_COL_BRANCH
    " IN(%s) OR e." TRANSACTION_COL_TRUNK " IN (%s)))";
// Used to avoid strlen in a heavily used API function
// Update accordingly if `iota_statement_transaction_find` is modified
static size_t iota_statement_transaction_find_size = 328;

/*
 * Partial Transaction statements
 */

char *iota_statement_transaction_select_essence_and_metadata =
    "SELECT " TRANSACTION_COL_ADDRESS "," TRANSACTION_COL_VALUE "," TRANSACTION_COL_OBSOLETE_TAG
    "," TRANSACTION_COL_TIMESTAMP "," TRANSACTION_COL_CURRENT_INDEX "," TRANSACTION_COL_LAST_INDEX
    "," TRANSACTION_COL_BUNDLE "," TRANSACTION_COL_SNAPSHOT_INDEX "," TRANSACTION_COL_SOLID
    " FROM " TRANSACTION_TABLE_NAME " WHERE " TRANSACTION_COL_HASH "=?";

char *iota_statement_transaction_select_essence_attachment_and_metadata =
    "SELECT " TRANSACTION_COL_ADDRESS "," TRANSACTION_COL_VALUE "," TRANSACTION_COL_OBSOLETE_TAG
    "," TRANSACTION_COL_TIMESTAMP "," TRANSACTION_COL_CURRENT_INDEX "," TRANSACTION_COL_LAST_INDEX
    "," TRANSACTION_COL_BUNDLE "," TRANSACTION_COL_TRUNK "," TRANSACTION_COL_BRANCH
    "," TRANSACTION_COL_ATTACHMENT_TIMESTAMP "," TRANSACTION_COL_ATTACHMENT_TIMESTAMP_UPPER
    "," TRANSACTION_COL_ATTACHMENT_TIMESTAMP_LOWER "," TRANSACTION_COL_NONCE "," TRANSACTION_COL_TAG
    "," TRANSACTION_COL_SNAPSHOT_INDEX "," TRANSACTION_COL_SOLID "," TRANSACTION_COL_ARRIVAL_TIME
    " FROM " TRANSACTION_TABLE_NAME " WHERE " TRANSACTION_COL_HASH "=?";

char *iota_statement_transaction_select_essence_and_consensus =
    "SELECT " TRANSACTION_COL_ADDRESS "," TRANSACTION_COL_VALUE "," TRANSACTION_COL_OBSOLETE_TAG
    "," TRANSACTION_COL_TIMESTAMP "," TRANSACTION_COL_CURRENT_INDEX "," TRANSACTION_COL_LAST_INDEX
    "," TRANSACTION_COL_BUNDLE "," TRANSACTION_COL_HASH " FROM " TRANSACTION_TABLE_NAME " WHERE " TRANSACTION_COL_HASH
    "=?";

char *iota_statement_transaction_select_metadata =
    "SELECT " TRANSACTION_COL_SNAPSHOT_INDEX "," TRANSACTION_COL_SOLID "," TRANSACTION_COL_ARRIVAL_TIME
    " FROM " TRANSACTION_TABLE_NAME " WHERE " TRANSACTION_COL_HASH "=?";

/*
 * Transaction statement builders
 */

char *iota_statement_transaction_find_build(size_t const bundles_count, size_t const addresses_count,
                                            size_t const tags_count, size_t const approvees_count) {
  // Base size of the query + enough space for '?' (bindings)
  size_t statement_size = iota_statement_transaction_find_size + 2 * bundles_count + 2 * addresses_count +
                          2 * tags_count + 4 * approvees_count;
  char *statement = (char *)malloc(statement_size);

  char *bundles_in_clause = iota_statement_in_clause_build(bundles_count);
  char *addresses_in_clause = iota_statement_in_clause_build(addresses_count);
  char *tags_in_clause = iota_statement_in_clause_build(tags_count);
  char *approvees_in_clause = iota_statement_in_clause_build(approvees_count);

  snprintf(statement, statement_size, iota_statement_transaction_find, bundles_in_clause, addresses_in_clause,
           tags_in_clause, approvees_in_clause, approvees_in_clause);

  free(bundles_in_clause);
  free(addresses_in_clause);
  free(tags_in_clause);
  free(approvees_in_clause);

  return statement;
}

/*
 * Milestone statements
 */

char *iota_statement_milestone_insert =
    "INSERT INTO " MILESTONE_TABLE_NAME "(" MILESTONE_COL_INDEX "," MILESTONE_COL_HASH ")VALUES(?,?)";

char *iota_statement_milestone_select_by_hash = "SELECT " MILESTONE_COL_INDEX "," MILESTONE_COL_HASH
                                                " FROM " MILESTONE_TABLE_NAME " WHERE " MILESTONE_COL_HASH "=?";

char *iota_statement_milestone_select_first =
    "SELECT " MILESTONE_COL_INDEX "," MILESTONE_COL_HASH " FROM " MILESTONE_TABLE_NAME " ORDER BY " MILESTONE_COL_INDEX
    " ASC LIMIT 1";

char *iota_statement_milestone_select_last =
    "SELECT " MILESTONE_COL_INDEX "," MILESTONE_COL_HASH " FROM " MILESTONE_TABLE_NAME " ORDER BY " MILESTONE_COL_INDEX
    " DESC LIMIT 1";

char *iota_statement_milestone_select_next = "SELECT " MILESTONE_COL_INDEX "," MILESTONE_COL_HASH
                                             " FROM " MILESTONE_TABLE_NAME " WHERE " MILESTONE_COL_INDEX "=(?+1)";

char *iota_statement_milestone_exist =
    "SELECT 1 WHERE EXISTS(SELECT 1 "
    "FROM " MILESTONE_TABLE_NAME ")";

char *iota_statement_milestone_exist_by_hash =
    "SELECT 1 WHERE EXISTS(SELECT 1 "
    "FROM " MILESTONE_TABLE_NAME " WHERE " MILESTONE_COL_HASH "=?)";

/*
 * State delta statements
 */

char *iota_statement_state_delta_store =
    "UPDATE " MILESTONE_TABLE_NAME " SET " MILESTONE_COL_DELTA "=? WHERE " MILESTONE_COL_INDEX "=?";

char *iota_statement_state_delta_load =
    "SELECT " MILESTONE_COL_DELTA " FROM " MILESTONE_TABLE_NAME " WHERE " MILESTONE_COL_INDEX "=?";
