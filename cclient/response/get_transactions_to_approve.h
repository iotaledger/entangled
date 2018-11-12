/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef CCLIENT_RESPONSE_GET_TRANSACTIONS_TO_APPROVE_H
#define CCLIENT_RESPONSE_GET_TRANSACTIONS_TO_APPROVE_H

#include "types/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  flex_trit_t branch[FLEX_TRIT_SIZE_243];
  flex_trit_t trunk[FLEX_TRIT_SIZE_243];
} get_transactions_to_approve_res_t;

get_transactions_to_approve_res_t* get_transactions_to_approve_res_new();
void get_transactions_to_approve_res_free(
    get_transactions_to_approve_res_t** res);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_RESPONSE_GET_TRANSACTIONS_TO_APPROVE_H
