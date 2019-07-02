/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __NODE_PIPELINE_RECEIVER_H__
#define __NODE_PIPELINE_RECEIVER_H__

#include <stdbool.h>

#include "common/errors.h"

// Forward declarations
typedef struct node_s node_t;

typedef struct receiver_s {
  bool running;
  node_t *node;
} receiver_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initializes a receiver
 *
 * @param receiver The receiver
 * @param node A node
 * @param tcp_port A TCP port to listen
 *
 * @return a status code
 */
retcode_t receiver_init(receiver_t *const receiver, node_t *const node, uint16_t tcp_port);

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

#endif  //__NODE_PIPELINE_RECEIVER_H__
