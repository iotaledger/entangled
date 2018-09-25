/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef CCLIENT_RESPONSE_GET_TRYTES_H
#define CCLIENT_RESPONSE_GET_TRYTES_H

#ifdef __cplusplus
extern "C" {
#endif

#include "types/types.h"

/*
The raw transaction data (trytes) of a specific transaction. These trytes can
then be easily converted into the actual transaction object.
*/
typedef flex_hash_array_t get_trytes_res_t;

get_trytes_res_t* get_trytes_res_new();
trit_array_p get_trytes_res_at(get_trytes_res_t* trytes_array, int index);
int get_trytes_res_num(get_trytes_res_t* trytes_array);
void get_trytes_res_free(get_trytes_res_t* trytes_array);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_RESPONSE_GET_TRYTES_H
