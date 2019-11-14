/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __COMMON_STORAGE_DEFS_H__
#define __COMMON_STORAGE_DEFS_H__

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
#define TRANSACTION_COL_SNAPSHOT_INDEX "snapshot_index"
#define TRANSACTION_COL_SOLID "solid"
#define TRANSACTION_COL_VALIDITY "validity"
#define TRANSACTION_COL_ARRIVAL_TIME "arrival_timestamp"

#define TRANSACTION_NUM_COLS 20

/*
 * Milestone definitions
 */

#define MILESTONE_TABLE_NAME "iota_milestone"

#define MILESTONE_COL_INDEX "id"
#define MILESTONE_COL_HASH "hash"
#define MILESTONE_COL_DELTA "delta"

#define MILESTONE_NUM_COLS 3

/*
 * Spent address definitions
 */

#define SPENT_ADDRESS_TABLE_NAME "iota_spent_address"

#define SPENT_ADDRESS_COL_HASH "hash"

#define SPENT_ADDRESS_NUM_COLS 1

#endif  // __COMMON_STORAGE_DEFS_H__
