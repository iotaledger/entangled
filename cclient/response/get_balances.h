/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef CCLIENT_RESPONSE_GET_BALANCES_H
#define CCLIENT_RESPONSE_GET_BALANCES_H

#ifdef __cplusplus
extern "C" {
#endif

#include "types/types.h"

typedef struct {
  int_array* balances;
  int milestoneIndex;
  UT_array* milestone;
} get_balances_res_t;

get_balances_res_t* get_balances_res_new();
void get_balances_res_free(get_balances_res_t* res);
int get_balances_res_balances_at(get_balances_res_t* in, int index);
char* get_balances_res_milestone_at(get_balances_res_t* in, int index);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_RESPONSE_GET_BALANCES_H
