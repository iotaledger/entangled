/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef CCLIENT_REQUEST_BROADCAST_TRANSACTIONS_H
#define CCLIENT_REQUEST_BROADCAST_TRANSACTIONS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "types/types.h"

// List of raw data of transactions to be rebroadcast.
typedef UT_array broadcast_transactions_req_t;

broadcast_transactions_req_t* broadcast_transactions_req_new();
void broadcast_transactions_req_add(broadcast_transactions_req_t* transactions,
                                    const char* trytes);
void broadcast_transactions_req_free(
    broadcast_transactions_req_t* transactions);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_REQUEST_BROADCAST_TRANSACTIONS_H
