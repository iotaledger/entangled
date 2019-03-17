/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef CCLIENT_REQUEST_GET_BALANCES_H
#define CCLIENT_REQUEST_GET_BALANCES_H

#include "cclient/types/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  /**
   * The confirmation threshold between 0 and 100(inclusive).
   *  Should be set to 100 for getting balance by counting only confirmed
   * transactions.
   */
  uint8_t threshold;
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

static inline retcode_t get_balances_req_address_add(get_balances_req_t* const req, flex_trit_t const* const hash) {
  return hash243_queue_push(&req->addresses, hash);
}
static inline flex_trit_t* get_balances_req_address_get(get_balances_req_t* const req, size_t index) {
  return hash243_queue_at(&req->addresses, index);
}

static inline retcode_t get_balances_req_tip_add(get_balances_req_t* const req, flex_trit_t const* const hash) {
  return hash243_queue_push(&req->tips, hash);
}
static inline flex_trit_t* get_balances_req_tip_get(get_balances_req_t* const req, size_t index) {
  return hash243_queue_at(&req->tips, index);
}

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_REQUEST_GET_BALANCES_H
