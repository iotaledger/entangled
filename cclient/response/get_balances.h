/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef CCLIENT_RESPONSE_GET_BALANCES_H
#define CCLIENT_RESPONSE_GET_BALANCES_H

#include "types/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  UT_array* balances;
  int milestoneIndex;
  hash243_queue_t milestone;
} get_balances_res_t;

get_balances_res_t* get_balances_res_new();
void get_balances_res_free(get_balances_res_t** res);
char* get_balances_res_balances_at(get_balances_res_t const* const in,
                                   int const index);
size_t get_balances_res_total_balance(get_balances_res_t const* const res);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_RESPONSE_GET_BALANCES_H
