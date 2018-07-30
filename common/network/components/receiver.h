/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __COMMON_NETWORK_COMPONENTS_RECEIVER_H__
#define __COMMON_NETWORK_COMPONENTS_RECEIVER_H__

#include <stdbool.h>

#include "common/network/iota_packet.h"
#include "common/thread_handle.h"

typedef struct node_s node_t;

typedef struct {
  thread_handle_t thread;
  bool running;
  void *opaque_network;
  uint16_t tcp_port;
  uint16_t udp_port;
  node_t *node;
} receiver_state_t;

#ifdef __cplusplus
extern "C" {
#endif

bool receiver_packet_handler(receiver_state_t *const state,
                             iota_packet_t *const packet);
bool receiver_init(receiver_state_t *const state, node_t *const node, uint16_t tcp_port,
                    uint16_t udp_port);
bool receiver_start(receiver_state_t *const state);
bool receiver_stop(receiver_state_t *const state);

#ifdef __cplusplus
}
#endif

#endif  //__COMMON_NETWORK_COMPONENTS_RECEIVER_H__
