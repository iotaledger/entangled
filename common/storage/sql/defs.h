/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __COMMON_STORAGE_SQL_DEFS_H__
#define __COMMON_STORAGE_SQL_DEFS_H__

#define TRANSACTION_TABLE_NAME "iota_transaction"
#define COL_SIG_OR_MSG "signature_or_message"
#define COL_ADDRESS "address"
#define COL_VALUE "value"
#define COL_OBSOLETE_TAG "obsolete_tag"
#define COL_TIMESTAMP "timestamp"
#define COL_CURRENT_INDEX "current_index"
#define COL_LAST_INDEX "last_index"
#define COL_BUNDLE "bundle"
#define COL_TRUNK "trunk"
#define COL_BRANCH "branch"
#define COL_TAG "tag"
#define COL_ATTACHMENT_TIMESTAMP "attachment_timestamp"
#define COL_ATTACHMENT_TIMESTAMP_UPPER "attachment_timestamp_upper"
#define COL_ATTACHMENT_TIMESTAMP_LOWER "attachment_timestamp_lower"
#define COL_NONCE "nonce"

#define NUM_COLS 15

#define MAX_CREATE_INDEX_STATEMENT_SIZE 128
#define MAX_STORE_STATEMENT_SIZE 4096
#define MAX_EXIST_STATEMENT_SIZE 256
#define MAX_SELECT_STATEMENT_SIZE 256

#endif  // __COMMON_STORAGE_SQL_DEFS_H__
