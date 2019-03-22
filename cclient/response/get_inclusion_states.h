/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef CCLIENT_RESPONSE_GET_INCLUSION_STATES_H
#define CCLIENT_RESPONSE_GET_INCLUSION_STATES_H

#include "cclient/types/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  UT_array* states;
} get_inclusion_states_res_t;

get_inclusion_states_res_t* get_inclusion_states_res_new();
retcode_t get_inclusion_states_res_states_set(get_inclusion_states_res_t* res, int st);
void get_inclusion_states_res_free(get_inclusion_states_res_t** res);
bool get_inclusion_states_res_states_at(get_inclusion_states_res_t* in, int index);
int get_inclusion_states_res_states_count(get_inclusion_states_res_t* in);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_RESPONSE_GET_INCLUSION_STATES_H
