/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __COMMON_STORAGE_SQL_STATEMENTS_H__
#define __COMMON_STORAGE_SQL_STATEMENTS_H__

#include "common/errors.h"
#include "common/trinary/trit_array.h"

// Forward declarations
typedef struct _trit_array* trit_array_p;
typedef struct _iota_transaction* iota_transaction_t;
typedef struct iota_milestone_s iota_milestone_t;

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Transaction statements
 */

extern retcode_t iota_statement_transaction_insert(const iota_transaction_t tx,
                                                   char statement[],
                                                   size_t statement_cap);

extern retcode_t iota_statement_transaction_select(const char* index_col,
                                                   char statement[],
                                                   size_t statement_cap);

extern retcode_t iota_statement_transaction_exist(const char* index_col,
                                                  char statement[],
                                                  size_t statement_cap);

extern retcode_t iota_statement_transaction_update_snapshot_index(
    uint64_t snapshot_index, char statement[], size_t statement_cap);

extern retcode_t iota_statement_transaction_select_hashes(const char* index_col,
                                                          char statement[],
                                                          size_t statement_cap);

extern retcode_t iota_statement_transaction_select_hashes_approvers(
    const flex_trit_t* approvee_hash, char statement[], size_t statement_cap);

/*
 * Milestone statements
 */

extern char* iota_statement_milestone_insert;
extern char* iota_statement_milestone_select_by_hash;
extern char* iota_statement_milestone_select_first;
extern char* iota_statement_milestone_select_last;
extern char* iota_statement_milestone_select_next;
extern char* iota_statement_milestone_exist;
extern char* iota_statement_milestone_exist_by_hash;

#ifdef __cplusplus
}
#endif

#endif  // __COMMON_STORAGE_SQL_STATEMENTS_H__
