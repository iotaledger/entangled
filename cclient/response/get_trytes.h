/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef CCLIENT_RESPONSE_GET_TRYTES_H
#define CCLIENT_RESPONSE_GET_TRYTES_H

#include "types/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct get_trytes_res_s {
  /*
  The raw transaction data (trytes) of a specific transaction. These trytes can
  then be easily converted into the actual transaction object.
  */
  hash8019_queue_t trytes;
} get_trytes_res_t;

get_trytes_res_t* get_trytes_res_new();
void get_trytes_res_free(get_trytes_res_t** const res);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_RESPONSE_GET_TRYTES_H
