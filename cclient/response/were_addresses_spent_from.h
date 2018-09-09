/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef CCLIENT_RESPONSE_WERE_ADDRESSES_SPENT_FROM_H
#define CCLIENT_RESPONSE_WERE_ADDRESSES_SPENT_FROM_H

#ifdef __cplusplus
extern "C" {
#endif

#include "types/types.h"

typedef UT_array were_addresses_spent_from_res_t;

were_addresses_spent_from_res_t* were_addresses_spent_from_res_new();
void were_addresses_spent_from_res_free(were_addresses_spent_from_res_t* res);
bool were_addresses_spent_from_res_at(were_addresses_spent_from_res_t* in,
                                      int index);
int were_addresses_spent_from_res_num(were_addresses_spent_from_res_t* in);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_RESPONSE_WERE_ADDRESSES_SPENT_FROM_H
