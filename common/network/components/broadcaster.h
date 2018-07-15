/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __COMMON_NETWORK_COMPONENTS_BROADCASTER_H__
#define __COMMON_NETWORK_COMPONENTS_BROADCASTER_H__

#include "common/network/queues/concurrent_queue_trit_array.h"

typedef concurrent_queue_of_trit_array_p broadcaster_queue_t;

typedef struct {
  broadcaster_queue_t *queue;
  bool running;
} broadcaster_state_t;

#ifdef __cplusplus
extern "C" {
#endif

bool broadcaster_init(broadcaster_state_t *const state);
bool broadcaster_on_next(broadcaster_state_t *const state,
                         trit_array_p const hash);
bool broadcaster_stop(broadcaster_state_t *const state);

#ifdef __cplusplus
}
#endif

#endif  //__COMMON_NETWORK_COMPONENTS_BROADCASTER_H__
