/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef CCLIENT_REQUEST_FIND_TRANSACTIONS_H
#define CCLIENT_REQUEST_FIND_TRANSACTIONS_H

#include "types/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
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
  /**
   * List of bundle hashes.
   * Transactions belonging to bundles will be returned
   */
  hash243_queue_t bundles;
  // size_t numBundles;

} find_transactions_req_t;

find_transactions_req_t* find_transactions_req_new();
void find_transactions_req_free(find_transactions_req_t** req);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_REQUEST_FIND_TRANSACTIONS_H
