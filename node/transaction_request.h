/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __NODE_TRANSACTION_REQUEST_H__
#define __NODE_TRANSACTION_REQUEST_H__

#include "utlist.h"

#include "common/errors.h"
#include "common/trinary/flex_trit.h"

// Forward declarations
typedef struct neighbor_s neighbor_t;

// A transaction request coming from a neighbor
typedef struct transaction_request_s {
  // The requesting neighbor
  neighbor_t* neighbor;
  // The requested hash
  flex_trit_t hash[FLEX_TRIT_SIZE_243];
} transaction_request_t;

/**
 * A queue of transaction requests used to dispatch them in different threads.
 * Not concurrent by default.
 */
typedef struct transaction_request_queue_entry_s {
  transaction_request_t request;
  struct transaction_request_queue_entry_s* next;
  struct transaction_request_queue_entry_s* prev;
} transaction_request_queue_entry_t;

typedef transaction_request_queue_entry_t* transaction_request_queue_t;

/**
 * Gets the size of a transaction request queue
 *
 * @param queue The transaction request queue
 *
 * @return the size of the transaction request queue
 */
size_t transaction_request_queue_count(transaction_request_queue_t const queue);

/**
 * Pushes a transaction request to a transaction request queue
 *
 * @param queue The transaction request queue
 * @param neighbor The requesting neighbor
 * @param hash The requested hash
 *
 * @return a status code
 */
retcode_t transaction_request_queue_push(transaction_request_queue_t* const queue, neighbor_t* const neighbor,
                                         flex_trit_t const* const hash);

/**
 * Pops a transaction request from a transaction request queue
 *
 * @param queue The transaction request queue
 *
 * @return the transaction request entry
 */
transaction_request_queue_entry_t* transaction_request_queue_pop(transaction_request_queue_t* const queue);

/**
 * Frees a transaction request queue
 *
 * @param queue The transaction request queue
 *
 * @return a status code
 */
void transaction_request_queue_free(transaction_request_queue_t* const queue);

#endif  // __NODE_TRANSACTION_REQUEST_H__
