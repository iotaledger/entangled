/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __COMMON_NETWORK_COMPONENTS_PROCESSOR_H__
#define __COMMON_NETWORK_COMPONENTS_PROCESSOR_H__

#include "common/network/queues/concurrent_queue_trit_array.h"

typedef concurrent_queue_of_trit_array_p processor_queue_t;

typedef struct {
  processor_queue_t *queue;
  bool running;
} processor_state_t;

#ifdef __cplusplus
extern "C" {
#endif

bool processor_init(processor_state_t *const state);
bool processor_on_next(processor_state_t *const state, trit_array_p const hash);

#ifdef __cplusplus
}
#endif

#endif  //__COMMON_NETWORK_COMPONENTS_PROCESSOR_H__
