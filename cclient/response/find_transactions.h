/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef CCLIENT_RESPONSE_FIND_TRANSACTIONS_H
#define CCLIENT_RESPONSE_FIND_TRANSACTIONS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "types/types.h"

typedef flex_hash_array_t find_transactions_res_t;

find_transactions_res_t* find_transactions_res_new();
void find_transactions_res_free(find_transactions_res_t* res);
trit_array_p find_transactions_res_hash_at(find_transactions_res_t* in,
                                           int index);
int find_transactions_res_hash_num(find_transactions_res_t* in);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_RESPONSE_FIND_TRANSACTIONS_H
