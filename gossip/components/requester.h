/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __GOSSIP_COMPONENTS_REQUESTER_H__
#define __GOSSIP_COMPONENTS_REQUESTER_H__

#include <stdbool.h>
#include <stdlib.h>

// Forward declarations
typedef struct concurrent_list_trit_array_p_s requester_list_t;
typedef struct _trit_array *trit_array_p;
typedef struct node_s node_t;

typedef struct requester_state_s {
  requester_list_t *list;
  node_t *node;
} requester_state_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initializes a requester
 *
 * @param state The requester state
 *
 * @return a status code
 */
bool requester_init(requester_state_t *const state, node_t *const node);

/**
 * Gets the number of transactions to request
 *
 * @param state The requester state
 *
 * @return The number of transactions to request
 */
size_t requester_size(requester_state_t *const state);

/**
 * Cancels a request for a transaction
 *
 * @param state The requester state
 * @param hash The transaction request to cancel
 *
 * @return a status code
 */
bool requester_clear_request(requester_state_t *const state,
                             trit_array_p const hash);

/**
 * Tells whether the requester is full or not
 *
 * @param state The requester state
 *
 * @return true if full, false otherwise
 */
bool requester_is_full(requester_state_t *const state);

/**
 * Adds a transaction to be requested
 *
 * @param state The requester state
 * @param hash The transaction to request
 *
 * @return a status code
 */
bool request_transaction(requester_state_t *const state,
                         trit_array_p const hash);

/**
 * Gets a transaction to request
 *
 * @param state The requester state
 * @param hash The transaction to be requested
 *
 * @return a status code
 */
bool get_transaction_to_request(requester_state_t *const state,
                                trit_array_p *hash);

/**
 * Destroys a requester
 *
 * @param state The requester state
 *
 * @return a status code
 */
bool requester_destroy(requester_state_t *const state);

#ifdef __cplusplus
}
#endif

#endif  //__GOSSIP_COMPONENTS_REQUESTER_H__
