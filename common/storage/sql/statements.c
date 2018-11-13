/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <inttypes.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "common/storage/defs.h"

/*
 * Transaction statements
 */

char *iota_statement_transaction_insert =
    "INSERT INTO " TRANSACTION_TABLE_NAME "(" TRANSACTION_COL_SIG_OR_MSG
    "," TRANSACTION_COL_ADDRESS "," TRANSACTION_COL_VALUE
    "," TRANSACTION_COL_OBSOLETE_TAG "," TRANSACTION_COL_TIMESTAMP
    "," TRANSACTION_COL_CURRENT_INDEX "," TRANSACTION_COL_LAST_INDEX
    "," TRANSACTION_COL_BUNDLE "," TRANSACTION_COL_TRUNK
    "," TRANSACTION_COL_BRANCH "," TRANSACTION_COL_TAG
    "," TRANSACTION_COL_ATTACHMENT_TIMESTAMP
    "," TRANSACTION_COL_ATTACHMENT_TIMESTAMP_UPPER
    "," TRANSACTION_COL_ATTACHMENT_TIMESTAMP_LOWER "," TRANSACTION_COL_NONCE
    "," TRANSACTION_COL_HASH "," TRANSACTION_COL_SNAPSHOT_INDEX
    "," TRANSACTION_COL_SOLID ")VALUES(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)";

char *iota_statement_transaction_select_by_hash =
    "SELECT * FROM " TRANSACTION_TABLE_NAME " WHERE " TRANSACTION_COL_HASH "=?";

char *iota_statement_transaction_select_hashes_by_address =
    "SELECT " TRANSACTION_COL_HASH " FROM " TRANSACTION_TABLE_NAME
    " WHERE " TRANSACTION_COL_ADDRESS "=?";

char *iota_statement_transaction_select_hashes_of_approvers =
    "SELECT " TRANSACTION_COL_HASH " FROM " TRANSACTION_TABLE_NAME
    " WHERE " TRANSACTION_COL_BRANCH "=? OR " TRANSACTION_COL_TRUNK "=?";

char *iota_statement_transaction_select_hashes_of_transactions_to_request =
    "SELECT " TRANSACTION_COL_TRUNK " FROM " TRANSACTION_TABLE_NAME
    " a WHERE NOT(EXISTS(SELECT 1 FROM " TRANSACTION_TABLE_NAME
    " b WHERE b." TRANSACTION_COL_HASH " = a." TRANSACTION_COL_TRUNK
    ")) UNION SELECT " TRANSACTION_COL_BRANCH " FROM " TRANSACTION_TABLE_NAME
    " a WHERE NOT(EXISTS(SELECT 1 FROM " TRANSACTION_TABLE_NAME
    " b WHERE b." TRANSACTION_COL_HASH " = a." TRANSACTION_COL_BRANCH
    ")) LIMIT ?";

char *iota_statement_transaction_select_hashes_of_tips =
    "SELECT " TRANSACTION_COL_HASH " FROM " TRANSACTION_TABLE_NAME
    " a WHERE NOT(EXISTS(SELECT 1 FROM " TRANSACTION_TABLE_NAME
    " b WHERE b." TRANSACTION_COL_TRUNK " = a." TRANSACTION_COL_HASH
    " OR b." TRANSACTION_COL_BRANCH " = a." TRANSACTION_COL_HASH ")) LIMIT ?";

char *iota_statement_transaction_update_snapshot_index =
    "UPDATE " TRANSACTION_TABLE_NAME " SET " TRANSACTION_COL_SNAPSHOT_INDEX
    "=? WHERE " TRANSACTION_COL_HASH "=?";

char *iota_statement_transaction_update_solid_state =
    "UPDATE " TRANSACTION_TABLE_NAME " SET " TRANSACTION_COL_SOLID
    "=? WHERE " TRANSACTION_COL_HASH "=?";

char *iota_statement_transaction_exist =
    "SELECT 1 WHERE EXISTS(SELECT 1 "
    "FROM " TRANSACTION_TABLE_NAME ")";

char *iota_statement_transaction_exist_by_hash =
    "SELECT 1 WHERE EXISTS(SELECT 1 "
    "FROM " TRANSACTION_TABLE_NAME " WHERE " TRANSACTION_COL_HASH "=?)";

char *iota_statement_transaction_approvers_count =
    "SELECT COUNT(*) FROM " TRANSACTION_TABLE_NAME " WHERE branch=? OR trunk=?";

/*
 * Milestone statements
 */

char *iota_statement_milestone_insert =
    "INSERT INTO " MILESTONE_TABLE_NAME "(" MILESTONE_COL_INDEX
    "," MILESTONE_COL_HASH ")VALUES(?,?)";

char *iota_statement_milestone_select_by_hash =
    "SELECT * FROM " MILESTONE_TABLE_NAME " WHERE " MILESTONE_COL_HASH "=?";

char *iota_statement_milestone_select_first =
    "SELECT * FROM " MILESTONE_TABLE_NAME " ORDER BY " MILESTONE_COL_INDEX
    " ASC LIMIT 1";

char *iota_statement_milestone_select_last =
    "SELECT * FROM " MILESTONE_TABLE_NAME " ORDER BY " MILESTONE_COL_INDEX
    " DESC LIMIT 1";

char *iota_statement_milestone_select_next =
    "SELECT * FROM " MILESTONE_TABLE_NAME " WHERE " MILESTONE_COL_INDEX
    ">? ORDER BY " MILESTONE_COL_INDEX " ASC LIMIT 1";

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
    "UPDATE " MILESTONE_TABLE_NAME " SET " MILESTONE_COL_DELTA
    "=? WHERE " MILESTONE_COL_INDEX "=?";

char *iota_statement_state_delta_load =
    "SELECT " MILESTONE_COL_DELTA " FROM " MILESTONE_TABLE_NAME
    " WHERE " MILESTONE_COL_INDEX "=?";
