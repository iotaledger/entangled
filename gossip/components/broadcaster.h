/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __GOSSIP_COMPONENTS_BROADCASTER_H__
#define __GOSSIP_COMPONENTS_BROADCASTER_H__

#include <stdbool.h>

#include "utils/handles/thread.h"

// Forward declarations
typedef struct concurrent_queue_iota_packet_t_s broadcaster_queue_t;
typedef struct iota_packet_s iota_packet_t;
typedef struct node_s node_t;

typedef struct broadcaster_state_s {
  thread_handle_t thread;
  bool running;
  broadcaster_queue_t *queue;
  node_t *node;
} broadcaster_state_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initializes a broadcaster
 *
 * @param state The broadcaster state
 * @param node A node
 *
 * @return a status code
 */
bool broadcaster_init(broadcaster_state_t *const state, node_t *const node);

/**
 * Starts a broadcaster
 *
 * @param state The broadcaster state
 *
 * @return a status code
 */
bool broadcaster_start(broadcaster_state_t *const state);

/**
 * Adds a packet to a broadcaster queue
 *
 * @param state The broadcaster state
 * @param packet The packet
 *
 * @return a status code
 */
bool broadcaster_on_next(broadcaster_state_t *const state,
                         iota_packet_t const packet);

/**
 * Stops a broadcaster
 *
 * @param state The broadcaster state
 *
 * @return a status code
 */
bool broadcaster_stop(broadcaster_state_t *const state);

/**
 * Destroys a broadcaster
 *
 * @param state The broadcaster state
 *
 * @return a status code
 */
bool broadcaster_destroy(broadcaster_state_t *const state);

#ifdef __cplusplus
}
#endif

#endif  //__GOSSIP_COMPONENTS_BROADCASTER_H__
