/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __COMMON_STORAGE_SQL_DEFS_H__
#define __COMMON_STORAGE_SQL_DEFS_H__

/*
 * Common definitions
 */

#define MAX_CREATE_INDEX_STATEMENT_SIZE 128

/*
 * Transaction definitions
 */

#define TRANSACTION_TABLE_NAME "iota_transaction"

#define TRANSACTION_COL_SIG_OR_MSG "signature_or_message"
#define TRANSACTION_COL_ADDRESS "address"
#define TRANSACTION_COL_VALUE "value"
#define TRANSACTION_COL_OBSOLETE_TAG "obsolete_tag"
#define TRANSACTION_COL_TIMESTAMP "timestamp"
#define TRANSACTION_COL_CURRENT_INDEX "current_index"
#define TRANSACTION_COL_LAST_INDEX "last_index"
#define TRANSACTION_COL_BUNDLE "bundle"
#define TRANSACTION_COL_TRUNK "trunk"
#define TRANSACTION_COL_BRANCH "branch"
#define TRANSACTION_COL_TAG "tag"
#define TRANSACTION_COL_ATTACHMENT_TIMESTAMP "attachment_timestamp"
#define TRANSACTION_COL_ATTACHMENT_TIMESTAMP_UPPER "attachment_timestamp_upper"
#define TRANSACTION_COL_ATTACHMENT_TIMESTAMP_LOWER "attachment_timestamp_lower"
#define TRANSACTION_COL_NONCE "nonce"
#define TRANSACTION_COL_HASH "hash"

#define TRANSACTION_NUM_COLS 16

#define TRANSACTION_ADDRESS_INDEX "address_index"
#define TRANSACTION_BUNDLE_INDEX "bundle_index"
#define TRANSACTION_TRUNK_INDEX "trunk_index"
#define TRANSACTION_BRANCH_INDEX "branch_index"
#define TRANSACTION_TAG_INDEX "tag_index"
#define TRANSACTION_HASH_INDEX "hash_index"

#define TRANSACTION_MAX_STORE_STATEMENT_SIZE 512
#define TRANSACTION_MAX_EXIST_STATEMENT_SIZE 128
#define TRANSACTION_MAX_SELECT_STATEMENT_SIZE 128

/*
 * Milestone definitions
 */

#define MILESTONE_TABLE_NAME "iota_milestone"

#define MILESTONE_COL_INDEX "id"
#define MILESTONE_COL_HASH "hash"

#define MILESTONE_NUM_COLS 2

#define MILESTONE_HASH_INDEX "hash_index"

#define MILESTONE_MAX_STORE_STATEMENT_SIZE 64
#define MILESTONE_MAX_EXIST_STATEMENT_SIZE 128
#define MILESTONE_MAX_SELECT_STATEMENT_SIZE 128

#endif  // __COMMON_STORAGE_SQL_DEFS_H__
