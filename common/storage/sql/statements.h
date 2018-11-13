/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __COMMON_STORAGE_SQL_STATEMENTS_H__
#define __COMMON_STORAGE_SQL_STATEMENTS_H__

#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Transaction statements
 */

extern char* iota_statement_transaction_insert;
extern char* iota_statement_transaction_select_by_hash;
extern char* iota_statement_transaction_select_hashes_by_address;
extern char* iota_statement_transaction_select_hashes_of_approvers;
extern char*
    iota_statement_transaction_select_hashes_of_transactions_to_request;
extern char* iota_statement_transaction_select_hashes_of_tips;
extern char* iota_statement_transaction_update_snapshot_index;
extern char* iota_statement_transaction_update_solid_state;
extern char* iota_statement_transaction_exist;
extern char* iota_statement_transaction_exist_by_hash;
extern char* iota_statement_transaction_approvers_count;

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

/*
 * State delta statements
 */

extern char* iota_statement_state_delta_store;
extern char* iota_statement_state_delta_load;

#ifdef __cplusplus
}
#endif

#endif  // __COMMON_STORAGE_SQL_STATEMENTS_H__
