/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef CCLIENT_REQUEST_WERE_ADDRESSES_SPENT_FROM_H
#define CCLIENT_REQUEST_WERE_ADDRESSES_SPENT_FROM_H

#ifdef __cplusplus
extern "C" {
#endif

#include "types/types.h"

/* List of tx hashes for which request should get trytes */
typedef UT_array were_addresses_spent_from_req_t;

were_addresses_spent_from_req_t* were_addresses_spent_from_req_new();
void were_addresses_spent_from_req_add(were_addresses_spent_from_req_t* req,
                                       const char* trytes);
void were_addresses_spent_from_req_free(were_addresses_spent_from_req_t* ut);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_REQUEST_WERE_ADDRESSES_SPENT_FROM_H