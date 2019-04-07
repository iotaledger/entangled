/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef CCLIENT_REQUEST_FIND_TRANSACTIONS_H
#define CCLIENT_REQUEST_FIND_TRANSACTIONS_H

#include "cclient/types/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct find_transactions_req_s {
  /**
   * List of bundle hashes.
   * Transactions belonging to bundles will be returned
   */
  hash243_queue_t bundles;

  /**
   * List of addresses.
   * Transactions with any of these addresses as input/output will be returned
   */
  hash243_queue_t addresses;

  /**
   * List of transaction tags.
   * Transactions with any of these tags will be returned
   */
  hash81_queue_t tags;

  /**
   * List of approvees of a transaction.
   * Transactions which directly approve any of approvees will be returned
   */
  hash243_queue_t approvees;
} find_transactions_req_t;

find_transactions_req_t* find_transactions_req_new();
void find_transactions_req_free(find_transactions_req_t** req);

static inline retcode_t find_transactions_req_bundle_add(find_transactions_req_t* const req,
                                                         flex_trit_t const* const hash) {
  return hash243_queue_push(&req->bundles, hash);
}
static inline flex_trit_t* find_transactions_req_bundle_get(find_transactions_req_t* const req, size_t index) {
  return hash243_queue_at(&req->bundles, index);
}

static inline retcode_t find_transactions_req_address_add(find_transactions_req_t* const req,
                                                          flex_trit_t const* const hash) {
  return hash243_queue_push(&req->addresses, hash);
}
static inline flex_trit_t* find_transactions_req_address_get(find_transactions_req_t* const req, size_t index) {
  return hash243_queue_at(&req->addresses, index);
}

static inline retcode_t find_transactions_req_tag_add(find_transactions_req_t* const req,
                                                      flex_trit_t const* const hash) {
  return hash81_queue_push(&req->tags, hash);
}
static inline flex_trit_t* find_transactions_req_tag_get(find_transactions_req_t* const req, size_t index) {
  return hash81_queue_at(&req->tags, index);
}

static inline retcode_t find_transactions_req_approvee_add(find_transactions_req_t* const req,
                                                           flex_trit_t const* const hash) {
  return hash243_queue_push(&req->approvees, hash);
}
static inline flex_trit_t* find_transactions_req_approvee_get(find_transactions_req_t* const req, size_t index) {
  return hash243_queue_at(&req->approvees, index);
}

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_REQUEST_FIND_TRANSACTIONS_H
