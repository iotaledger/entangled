/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef CCLIENT_RESPONSE_GET_TRYTES_H
#define CCLIENT_RESPONSE_GET_TRYTES_H

#include "cclient/types/types.h"

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
static inline retcode_t get_trytes_res_trytes_add(get_trytes_res_t* const res, flex_trit_t const* const hash) {
  return hash8019_queue_push(&res->trytes, hash);
}
static inline flex_trit_t* get_trytes_res_trytes_get(get_trytes_res_t* const res, size_t index) {
  return hash8019_queue_at(&res->trytes, index);
}

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_RESPONSE_GET_TRYTES_H
