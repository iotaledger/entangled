/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef CCLIENT_RESPONSE_FIND_TRANSACTIONS_H
#define CCLIENT_RESPONSE_FIND_TRANSACTIONS_H

#include "cclient/types/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct find_transactions_res {
  hash243_queue_t hashes;
} find_transactions_res_t;

find_transactions_res_t* find_transactions_res_new();
void find_transactions_res_free(find_transactions_res_t** res);
static inline retcode_t find_transactions_res_hashes_add(find_transactions_res_t* const res,
                                                         flex_trit_t const* const hash) {
  return hash243_queue_push(&res->hashes, hash);
}
static inline flex_trit_t* find_transactions_res_hashes_get(find_transactions_res_t* const res, size_t index) {
  return hash243_queue_at(&res->hashes, index);
}

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_RESPONSE_FIND_TRANSACTIONS_H
