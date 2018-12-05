/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __GOSSIP_COMPONENTS_TRANSACTION_REQUESTER_H__
#define __GOSSIP_COMPONENTS_TRANSACTION_REQUESTER_H__

#include <stdbool.h>

#include "common/errors.h"
#include "gossip/conf.h"
#include "utils/containers/hash/hash243_set.h"
#include "utils/handles/rw_lock.h"

// Forward declarations
typedef struct tangle_s tangle_t;

typedef struct transaction_requester_s {
  iota_gossip_conf_t *conf;
  hash243_set_t milestones;
  hash243_set_t transactions;
  tangle_t *tangle;
  rw_lock_handle_t lock;
} transaction_requester_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initializes a transaction requester
 *
 * @param transaction_requester The transaction requester
 * @param tangle A tangle
 * @param conf Gossip configuration
 *
 * @return a status code
 */
retcode_t requester_init(transaction_requester_t *const transaction_requester,
                         iota_gossip_conf_t *const conf,
                         tangle_t *const tangle);

/**
 * Destroys a transaction requester
 *
 * @param transaction_requester The transaction requester
 *
 * @return a status code
 */
retcode_t requester_destroy(
    transaction_requester_t *const transaction_requester);

/**
 * Get all the requested transactions and milestones from a transaction
 * requester
 *
 * @param transaction_requester The transaction requester
 * @param transactions The set of transactions to be filled
 *
 * @return a status code
 */
retcode_t requester_get_requested_transactions(
    transaction_requester_t *const transaction_requester,
    hash243_set_t *const transactions);

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
retcode_t requester_clear_request(
    transaction_requester_t *const transaction_requester,
    flex_trit_t const *const hash);

/**
 * Adds a transaction to be requested by a transaction requester
 *
 * @param transaction_requester The transaction requester
 * @param hash The transaction to request
 * @param is_milestone Whether the hash is a milestone or not
 *
 * @return a status code
 */
retcode_t request_transaction(
    transaction_requester_t *const transaction_requester,
    flex_trit_t *const hash, bool const is_milestone);

/**
 * Gets a transaction to request from a transaction requester
 *
 * @param transaction_requester The transaction requester
 * @param hash The transaction to be requested
 * @param milestone Whether to request a milestone or not
 *
 * @return a status code
 */
retcode_t get_transaction_to_request(
    transaction_requester_t *const transaction_requester,
    flex_trit_t *const hash, bool const milestone);

#ifdef __cplusplus
}
#endif

#endif  //__GOSSIP_COMPONENTS_TRANSACTION_REQUESTER_H__
