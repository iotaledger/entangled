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

typedef struct receiver_s {
  thread_handle_t thread;
  bool running;
  node_t *node;
  uint16_t port;
} receiver_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initializes a receiver
 *
 * @param receiver The receiver
 * @param node A node
 * @param port A port to listen to
 *
 * @return a status code
 */
retcode_t receiver_init(receiver_t *const receiver, node_t *const node, uint16_t port);

/**
 * Starts a receiver
 *
 * @param receiver The receiver
 *
 * @return a status code
 */
retcode_t receiver_start(receiver_t *const receiver);

/**
 * Stops a receiver
 *
 * @param receiver The receiver
 *
 * @return a status code
 */
retcode_t receiver_stop(receiver_t *const receiver);

/**
 * Destroys a receiver
 *
 * @param receiver The receiver
 *
 * @return a status code
 */
retcode_t receiver_destroy(receiver_t *const receiver);

#ifdef __cplusplus
}
#endif

#endif  //__CIRI_NODE_PIPELINE_RECEIVER_H__
