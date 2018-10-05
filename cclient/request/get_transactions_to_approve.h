/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef CCLIENT_REQUEST_GET_TRANSACTIONS_TO_APPROVE_H
#define CCLIENT_REQUEST_GET_TRANSACTIONS_TO_APPROVE_H

#include "types/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  uint32_t depth;
  char_buffer_t* reference;
} get_transactions_to_approve_req_t;

get_transactions_to_approve_req_t* get_transactions_to_approve_req_new();
void get_transactions_to_approve_req_free(
    get_transactions_to_approve_req_t** req);
void get_transactions_to_approve_req_set_reference(
    get_transactions_to_approve_req_t* req, char* reference);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_REQUEST_FIND_TRANSACTIONS_H
