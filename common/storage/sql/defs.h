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

#define TRANSACTION_ADDRESS_INDEX "address_index"
#define TRANSACTION_BUNDLE_INDEX "bundle_index"
#define TRANSACTION_TRUNK_INDEX "trunk_index"
#define TRANSACTION_BRANCH_INDEX "branch_index"
#define TRANSACTION_TAG_INDEX "tag_index"
#define TRANSACTION_HASH_INDEX "hash_index"

#define TRANSACTION_MAX_STORE_STATEMENT_SIZE 512
#define TRANSACTION_MAX_EXIST_STATEMENT_SIZE 128
#define TRANSACTION_MAX_SELECT_STATEMENT_SIZE 64

/*
 * Milestone definitions
 */

#define MILESTONE_HASH_INDEX "hash_index"

#define MILESTONE_MAX_STORE_STATEMENT_SIZE 64
#define MILESTONE_MAX_EXIST_STATEMENT_SIZE 128
#define MILESTONE_MAX_SELECT_STATEMENT_SIZE 64

#endif  // __COMMON_STORAGE_SQL_DEFS_H__
