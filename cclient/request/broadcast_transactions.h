/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef CCLIENT_REQUEST_BROADCAST_TRANSACTIONS_H
#define CCLIENT_REQUEST_BROADCAST_TRANSACTIONS_H

#include "types/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct broadcast_transactions_req_s {
  // List of raw transactions data to be broadcasted
  hash8019_stack_t trytes;
} broadcast_transactions_req_t;

broadcast_transactions_req_t* broadcast_transactions_req_new();
void broadcast_transactions_req_free(broadcast_transactions_req_t** const req);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_REQUEST_BROADCAST_TRANSACTIONS_H
