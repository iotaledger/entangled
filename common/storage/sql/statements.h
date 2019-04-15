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

typedef struct iota_statements_s {
  void* transaction_insert;
  void* transaction_select_by_hash;
  void* transaction_select_hashes_by_address;
  void* transaction_select_hashes_of_approvers;
  void* transaction_select_hashes_of_approvers_before_date;
  void* transaction_select_hashes_of_milestone_candidates;
  void* transaction_update_snapshot_index;
  void* transaction_update_solid_state;
  void* transaction_exist;
  void* transaction_exist_by_hash;
  void* transaction_approvers_count;
  void* transaction_count;
  void* transaction_select_essence_and_metadata;
  void* transaction_select_essence_attachment_and_metadata;
  void* transaction_select_essence_and_consensus;
  void* transaction_select_metadata;
  void* milestone_clear;
  void* milestone_insert;
  void* milestone_select_by_hash;
  void* milestone_select_first;
  void* milestone_select_last;
  void* milestone_select_next;
  void* milestone_exist;
  void* milestone_exist_by_hash;
  void* state_delta_store;
  void* state_delta_load;
} iota_statements_t;

/*
 * Generic statement builders
 */

extern char* iota_statement_in_clause_build(size_t const count);

/*
 * Transaction statements
 */

extern char* iota_statement_transaction_insert;
extern char* iota_statement_transaction_select_by_hash;
extern char* iota_statement_transaction_select_hashes_by_address;
extern char* iota_statement_transaction_select_hashes_of_approvers;
extern char* iota_statement_transaction_select_hashes_of_approvers_before_date;
extern char* iota_statement_transaction_select_hashes_of_milestone_candidates;
extern char* iota_statement_transaction_update_snapshot_index;
extern char* iota_statement_transaction_update_solid_state;
extern char* iota_statement_transaction_exist;
extern char* iota_statement_transaction_exist_by_hash;
extern char* iota_statement_transaction_approvers_count;
extern char* iota_statement_transaction_count;
extern char* iota_statement_transaction_find;

/*
 * Partial Transaction statements
 */

extern char* iota_statement_transaction_select_essence_and_metadata;
extern char* iota_statement_transaction_select_essence_attachment_and_metadata;
extern char* iota_statement_transaction_select_essence_and_consensus;
extern char* iota_statement_transaction_select_metadata;

/*
 * Transaction statement builders
 */

extern char* iota_statement_transaction_find_build(size_t const bundles_count, size_t const addresses_count,
                                                   size_t const tags_count, size_t const approvees_count);

/*
 * Milestone statements
 */

extern char* iota_statement_milestone_clear;
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
