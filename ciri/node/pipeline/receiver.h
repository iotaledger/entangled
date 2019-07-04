/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __CIRI_NODE_PIPELINE_RECEIVER_H__
#define __CIRI_NODE_PIPELINE_RECEIVER_H__

#include <stdbool.h>

#include "common/errors.h"
#include "utils/handles/thread.h"

// Forward declarations
typedef struct node_s node_t;

typedef struct receiver_stage_s {
  thread_handle_t thread;
  bool running;
  node_t *node;
  uint16_t port;
} receiver_stage_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initializes a receiver stage
 *
 * @param receiver The receiver stage
 * @param node A node
 * @param port A port to listen to
 *
 * @return a status code
 */
retcode_t receiver_stage_init(receiver_stage_t *const receiver, node_t *const node, uint16_t port);

/**
 * Starts a receiver stage
 *
 * @param receiver The receiver stage
 *
 * @return a status code
 */
retcode_t receiver_stage_start(receiver_stage_t *const receiver);

/**
 * Stops a receiver stage
 *
 * @param receiver The receiver stage
 *
 * @return a status code
 */
retcode_t receiver_stage_stop(receiver_stage_t *const receiver);

/**
 * Destroys a receiver stage
 *
 * @param receiver The receiver stage
 *
 * @return a status code
 */
retcode_t receiver_stage_destroy(receiver_stage_t *const receiver);

#ifdef __cplusplus
}
#endif

#endif  //__CIRI_NODE_PIPELINE_RECEIVER_H__
