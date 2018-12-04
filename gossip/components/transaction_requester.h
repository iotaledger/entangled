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

typedef struct requester_state_s {
  iota_gossip_conf_t *conf;
  hash243_set_t milestones;
  hash243_set_t transactions;
  tangle_t *tangle;
  rw_lock_handle_t lock;
} requester_state_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initializes a requester
 *
 * @param state The requester state
 * @param tangle A tangle
 * @param conf Gossip configuration
 *
 * @return a status code
 */
retcode_t requester_init(requester_state_t *const state,
                         iota_gossip_conf_t *const conf,
                         tangle_t *const tangle);

/**
 * Destroys a requester
 *
 * @param state The requester state
 *
 * @return a status code
 */
retcode_t requester_destroy(requester_state_t *const state);

/**
 * Get all the requested transactions and milestones
 *
 * @param state The requester state
 * @param transactions The set of transactions to be filled
 *
 * @return a status code
 */
retcode_t requester_get_requested_transactions(
    requester_state_t *const state, hash243_set_t *const transactions);

/**
 * Gets the number of transactions to request
 *
 * @param state The requester state
 *
 * @return The number of transactions to request
 */
size_t requester_size(requester_state_t *const state);

/**
 * Tells whether the requester is full or not
 *
 * @param state The requester state
 *
 * @return true if full, false otherwise
 */
bool requester_is_full(requester_state_t *const state);

/**
 * Cancels a request for a transaction
 *
 * @param state The requester state
 * @param hash The transaction request to cancel
 *
 * @return a status code
 */
retcode_t requester_clear_request(requester_state_t *const state,
                                  flex_trit_t const *const hash);

/**
 * Adds a transaction to be requested
 *
 * @param state The requester state
 * @param hash The transaction to request
 * @param is_milestone Whether the hash is a milestone or not
 *
 * @return a status code
 */
retcode_t request_transaction(requester_state_t *const state,
                              flex_trit_t *const hash, bool const is_milestone);

/**
 * Gets a transaction to request
 *
 * @param state The requester state
 * @param hash The transaction to be requested
 * @param milestone Whether to request a milestone or not
 *
 * @return a status code
 */
retcode_t get_transaction_to_request(requester_state_t *const state,
                                     flex_trit_t *const hash,
                                     bool const milestone);

#ifdef __cplusplus
}
#endif

#endif  //__GOSSIP_COMPONENTS_TRANSACTION_REQUESTER_H__
