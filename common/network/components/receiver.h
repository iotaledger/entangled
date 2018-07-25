/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __COMMON_NETWORK_COMPONENTS_RECEIVER_H__
#define __COMMON_NETWORK_COMPONENTS_RECEIVER_H__

#include <stdbool.h>

#include "common/thread_handle.h"

typedef struct {
  thread_handle_t thread;
  bool running;
} receiver_state_t;

#ifdef __cplusplus
extern "C" {
#endif

bool receiver_start(receiver_state_t *const state);
bool receiver_stop(receiver_state_t *const state);

#ifdef __cplusplus
}
#endif

#endif  //__COMMON_NETWORK_COMPONENTS_RECEIVER_H__
