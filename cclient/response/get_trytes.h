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
  flex_hash_array_t* trytes;
} get_trytes_res_t;

get_trytes_res_t* get_trytes_res_new();
void get_trytes_res_free(get_trytes_res_t** const res);
trit_array_p get_trytes_res_trytes_at(get_trytes_res_t const* const res,
                                      int index);
get_trytes_res_t* get_trytes_res_add_trytes(get_trytes_res_t* const res,
                                            tryte_t const* const trytes);
int get_trytes_res_trytes_num(get_trytes_res_t const* const res);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_RESPONSE_GET_TRYTES_H
