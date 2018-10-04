/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef CCLIENT_RESPONSE_GET_INCLUSION_STATE_H
#define CCLIENT_RESPONSE_GET_INCLUSION_STATE_H

#include "types/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  UT_array* states;
} get_inclusion_state_res_t;

get_inclusion_state_res_t* get_inclusion_state_res_new();
void get_inclusion_state_res_free(get_inclusion_state_res_t** res);
bool get_inclusion_state_res_bool_at(get_inclusion_state_res_t* in, int index);
int get_inclusion_state_res_bool_num(get_inclusion_state_res_t* in);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_RESPONSE_GET_INCLUSION_STATE_H
