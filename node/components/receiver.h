/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __NODE_COMPONENTS_RECEIVER_H__
#define __NODE_COMPONENTS_RECEIVER_H__

#include <stdbool.h>

#include "common/errors.h"
#include "node/services/receiver.h"

// Forward declarations
typedef struct node_s node_t;

typedef struct receiver_state_s {
  receiver_service_t tcp_service;
  receiver_service_t udp_service;
  bool running;
  node_t *node;
} receiver_state_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initializes a receiver
 *
 * @param state The receiver state
 * @param node A node
 * @param tcp_port A TCP port to listen
 * @param udp_port A UDP port to listen
 *
 * @return a status code
 */
retcode_t receiver_init(receiver_state_t *const state, node_t *const node, uint16_t tcp_port, uint16_t udp_port);

/**
 * Starts a receiver
 *
 * @param state The receiver state
 *
 * @return a status code
 */
retcode_t receiver_start(receiver_state_t *const state);

/**
 * Stops a receiver
 *
 * @param state The receiver state
 *
 * @return a status code
 */
retcode_t receiver_stop(receiver_state_t *const state);

/**
 * Destroys a receiver
 *
 * @param state The receiver state
 *
 * @return a status code
 */
retcode_t receiver_destroy(receiver_state_t *const state);

#ifdef __cplusplus
}
#endif

#endif  //__NODE_COMPONENTS_RECEIVER_H__
