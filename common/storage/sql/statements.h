/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __COMMON_STORAGE_SQL_STATEMENTS_H__
#define __COMMON_STORAGE_SQL_STATEMENTS_H__

#include "common/errors.h"

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

retcode_t iota_transaction_insert_statement(const iota_transaction_t tx,
                                            char statement[],
                                            size_t statement_cap);

retcode_t iota_transaction_select_statement(const char* index_col,
                                            char statement[],
                                            size_t statement_cap);

retcode_t iota_transaction_exist_statement(const char* index_col,
                                           char statement[],
                                           size_t statement_cap);

retcode_t iota_transaction_update_statement(const char* index_col,
                                            const trit_array_p key,
                                            const iota_transaction_t tx,
                                            char statement[],
                                            size_t statement_cap);

retcode_t iota_transaction_select_hashes_statement(const char* index_col,
                                                   char statement[],
                                                   size_t statement_cap);

retcode_t iota_transaction_select_hashes_approvers_statement(
    const trit_array_p approvee_hash, char statement[], size_t statement_cap);

/*
 * Milestone statements
 */

retcode_t iota_milestone_insert_statement(const iota_milestone_t* milestone,
                                          char statement[],
                                          size_t statement_cap);

retcode_t iota_milestone_select_statement(const char* index_col,
                                          char statement[],
                                          size_t statement_cap);

retcode_t iota_milestone_exist_statement(const char* index_col,
                                         char statement[],
                                         size_t statement_cap);

#ifdef __cplusplus
}
#endif

#endif  // __COMMON_STORAGE_SQL_STATEMENTS_H__
