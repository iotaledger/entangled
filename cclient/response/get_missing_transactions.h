/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef CCLIENT_RESPONSE_GET_MISSING_TRANSACTIONS_H
#define CCLIENT_RESPONSE_GET_MISSING_TRANSACTIONS_H

#include "cclient/types/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct get_missing_transactions_res_s {
  hash243_stack_t hashes;
} get_missing_transactions_res_t;

get_missing_transactions_res_t* get_missing_transactions_res_new();
size_t get_missing_transactions_res_hash_num(get_missing_transactions_res_t* res);
void get_missing_transactions_res_free(get_missing_transactions_res_t** res);
static inline retcode_t get_missing_transactions_res_hashes_add(get_missing_transactions_res_t* const res,
                                                                flex_trit_t const* const hash) {
  return hash243_stack_push(&res->hashes, hash);
}

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_RESPONSE_GET_MISSING_TRANSACTIONS_H
