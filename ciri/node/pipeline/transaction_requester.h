/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __NODE_PIPELINE_TRANSACTION_REQUESTER_H__
#define __NODE_PIPELINE_TRANSACTION_REQUESTER_H__

#include <stdbool.h>

#include "common/errors.h"
#include "utils/containers/hash/hash243_set.h"
#include "utils/containers/hash/hash243_stack.h"
#include "utils/handles/cond.h"
#include "utils/handles/rw_lock.h"
#include "utils/handles/thread.h"

// Forward declarations
typedef struct tangle_s tangle_t;
typedef struct node_s node_t;

typedef struct transaction_requester_s {
  thread_handle_t thread;
  bool running;
  hash243_set_t transactions;
  node_t *node;
  rw_lock_handle_t lock;
  cond_handle_t cond;
} transaction_requester_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initializes a transaction requester
 *
 * @param transaction_requester The transaction requester
 * @param node A node
 *
 * @return a status code
 */
retcode_t requester_init(transaction_requester_t *const transaction_requester, node_t *const node);

/**
 * Destroys a transaction requester
 *
 * @param transaction_requester The transaction requester
 *
 * @return a status code
 */
retcode_t requester_destroy(transaction_requester_t *const transaction_requester);

/**
 * Gets all the requested transactions and milestones hashes from a transaction requester
 *
 * @param transaction_requester The transaction requester
 * @param hashes The stack of transactions to be filled
 *
 * @return a status code
 */
retcode_t requester_get_requested_transactions(transaction_requester_t *const transaction_requester,
                                               hash243_stack_t *const hashes);

/**
 * Gets the number of transactions to request from a transaction requester
 *
 * @param transaction_requester The transaction requester
 *
 * @return The number of transactions to request
 */
size_t requester_size(transaction_requester_t *const transaction_requester);

/**
 * Tells whether a transaction requester is full or not
 *
 * @param transaction_requester The transaction requester
 *
 * @return true if full, false otherwise
 */
bool requester_is_full(transaction_requester_t *const transaction_requester);

/**
 * Cancels a request for a transaction from a transaction requester
 *
 * @param transaction_requester The transaction requester
 * @param hash The transaction request to cancel
 *
 * @return a status code
 */
retcode_t requester_clear_request(transaction_requester_t *const transaction_requester, flex_trit_t const *const hash);

/**
 * Checks if a transaction was requested
 *
 * @param transaction_requester The transaction requester
 * @param hash The transaction to check for request
 * @param is_requested Will be set to true if requested, false otherwise
 *
 * @return a status code
 */
retcode_t requester_is_requested(transaction_requester_t *const transaction_requester, flex_trit_t const *const hash,
                                 bool *const is_requested);

/**
 * Adds a transaction to be requested by a transaction requester
 *
 * @param transaction_requester The transaction requester
 * @param tangle A tangle
 * @param hash The transaction to request
 *
 * @return a status code
 */
retcode_t request_transaction(transaction_requester_t *const transaction_requester, tangle_t *const tangle,
                              flex_trit_t const *const hash);

/**
 * Gets a transaction to request from a transaction requester
 *
 * @param transaction_requester The transaction requester
 * @param tangle A tangle
 * @param hash The transaction to be requested
 *
 * @return a status code
 */
retcode_t get_transaction_to_request(transaction_requester_t *const transaction_requester, tangle_t const *const tangle,
                                     flex_trit_t *const hash);

/**
 * Tells whether the requester queue is empty or not
 *
 * @param requester The requester
 *
 * @return true if empty, false otherwise
 */
static inline bool requester_is_empty(transaction_requester_t *const requester) {
  return requester->transactions == NULL;
}

#ifdef __cplusplus
}
#endif

#endif  //__NODE_PIPELINE_TRANSACTION_REQUESTER_H__
