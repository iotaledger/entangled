/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef CCLIENT_RESPONSE_GET_TIPS_H
#define CCLIENT_RESPONSE_GET_TIPS_H

#include "types/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct get_tips_res_s {
  flex_hash_array_t* tips;
} get_tips_res_t;

get_tips_res_t* get_tips_res_new();
void get_tips_res_free(get_tips_res_t* const res);
get_tips_res_t* get_tips_res_add_tip(get_tips_res_t* const res,
                                     tryte_t const* const tip);
trit_array_p get_tips_res_tip_at(get_tips_res_t* const res, size_t const index);
size_t get_tips_res_tip_num(get_tips_res_t* const res);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_RESPONSE_GET_TIPS_H
