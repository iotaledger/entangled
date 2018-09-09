/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef CCLIENT_REQUEST_GET_BALANCES_H
#define CCLIENT_REQUEST_GET_BALANCES_H

#ifdef __cplusplus
extern "C" {
#endif

#include "types/types.h"

typedef struct {
  /**
   * List of addresses you want to get the confirmed balance for.
   */
  UT_array* addresses;
  /**
   * Transactions with any of these tags will be returned
   */
  size_t threshold;
  /**
   * List of hashes, if present calculate the balance of addresses
   * from the PoV of these transactions, can be used to chain bundles.
   */
  UT_array* tips;
} get_balances_req_t;

get_balances_req_t* get_balances_req_new();
void get_balances_req_free(get_balances_req_t* req);
void get_balances_req_add_address(get_balances_req_t* req, char* addr);
void get_balances_req_add_tips(get_balances_req_t* req, char* tips);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_REQUEST_GET_BALANCES_H
