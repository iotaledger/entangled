/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __COMMON_NETWORK_COMPONENTS_RECEIVER_H__
#define __COMMON_NETWORK_COMPONENTS_RECEIVER_H__

typedef struct {
} receiver_state_t;

void *receiver_routine(receiver_state_t *const state);

#endif  //__COMMON_NETWORK_COMPONENTS_RECEIVER_H__
