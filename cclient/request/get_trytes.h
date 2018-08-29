/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef CCLIENT_REQUEST_GET_TRYTES_H
#define CCLIENT_REQUEST_GET_TRYTES_H

#ifdef __cplusplus
extern "C" {
#endif

#include "types/types.h"

/* List of tx hashes for which request should get trytes */
typedef UT_array get_trytes_req_t;

get_trytes_req_t* get_trytes_req_new();
void get_trytes_req_add(get_trytes_req_t* hashes, const char* hash);
void get_trytes_req_free(get_trytes_req_t* ut);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_REQUEST_GET_TRYTES_H
