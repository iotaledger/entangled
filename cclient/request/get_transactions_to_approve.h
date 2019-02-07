/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef CCLIENT_REQUEST_GET_TRANSACTIONS_TO_APPROVE_H
#define CCLIENT_REQUEST_GET_TRANSACTIONS_TO_APPROVE_H

#include "cclient/types/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct get_transactions_to_approve_req_s {
  uint32_t depth;
  flex_trit_t* reference;
} get_transactions_to_approve_req_t;

get_transactions_to_approve_req_t* get_transactions_to_approve_req_new();
void get_transactions_to_approve_req_free(
    get_transactions_to_approve_req_t** const req);
void get_transactions_to_approve_req_set_depth(
    get_transactions_to_approve_req_t* const req, uint32_t const depth);
void get_transactions_to_approve_req_set_reference(
    get_transactions_to_approve_req_t* const req,
    flex_trit_t const* const reference);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_REQUEST_FIND_TRANSACTIONS_H
