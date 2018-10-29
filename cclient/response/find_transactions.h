/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef CCLIENT_RESPONSE_FIND_TRANSACTIONS_H
#define CCLIENT_RESPONSE_FIND_TRANSACTIONS_H

#include "types/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct find_transactions_res {
  hash243_queue_t hashes;
} find_transactions_res_t;

find_transactions_res_t* find_transactions_res_new();
void find_transactions_res_free(find_transactions_res_t** res);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_RESPONSE_FIND_TRANSACTIONS_H
