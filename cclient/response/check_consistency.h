/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef CCLIENT_RESPONSE_CHECK_CONSISTENCY_H
#define CCLIENT_RESPONSE_CHECK_CONSISTENCY_H

#include "cclient/types/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct check_consistency_res_s {
  bool state;
  char_buffer_t* info;  // This field will only be set if `state` is False.
} check_consistency_res_t;

check_consistency_res_t* check_consistency_res_new();
void check_consistency_res_free(check_consistency_res_t* res);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_RESPONSE_CHECK_CONSISTENCY_H
