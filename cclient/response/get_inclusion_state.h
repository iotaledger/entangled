/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef CCLIENT_RESPONSE_GET_INCLUSION_STATE_H
#define CCLIENT_RESPONSE_GET_INCLUSION_STATE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "types/types.h"

typedef struct {
  int_array bitmap;
} get_inclusion_state_res_t;

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_RESPONSE_GET_INCLUSION_STATE_H
