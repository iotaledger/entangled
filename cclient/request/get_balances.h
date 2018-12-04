/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef CCLIENT_REQUEST_GET_BALANCES_H
#define CCLIENT_REQUEST_GET_BALANCES_H

#include "types/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  /**
   * Transactions with any of these tags will be returned
   */
  size_t threshold;
  /**
   * List of addresses you want to get the confirmed balance for.
   */
  hash243_queue_t addresses;
  /**
   * List of hashes, if present calculate the balance of addresses
   * from the PoV of these transactions, can be used to chain bundles.
   */
  hash243_queue_t tips;
} get_balances_req_t;

get_balances_req_t* get_balances_req_new();
void get_balances_req_free(get_balances_req_t** req);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_REQUEST_GET_BALANCES_H
