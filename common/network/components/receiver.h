/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __COMMON_NETWORK_COMPONENTS_RECEIVER_H__
#define __COMMON_NETWORK_COMPONENTS_RECEIVER_H__

#include <stdbool.h>

typedef struct {
  bool running;
} receiver_state_t;

#ifdef __cplusplus
extern "C" {
#endif

bool receiver_init(receiver_state_t *const state);

#ifdef __cplusplus
}
#endif

#endif  //__COMMON_NETWORK_COMPONENTS_RECEIVER_H__
