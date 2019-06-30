/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __CIRI_NODE_PIPELINE_BROADCASTER_H__
#define __CIRI_NODE_PIPELINE_BROADCASTER_H__

#include <stdbool.h>

#include "ciri/node/iota_packet.h"
#include "common/errors.h"
#include "utils/handles/cond.h"
#include "utils/handles/rw_lock.h"
#include "utils/handles/thread.h"

// Forward declarations
typedef struct node_s node_t;

typedef struct broadcaster_s {
  cond_handle_t cond;
  rw_lock_handle_t lock;
  bool running;
  thread_handle_t thread;
  node_t *node;
  iota_packet_queue_t queue;
} broadcaster_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initializes a broadcaster
 *
 * @param broadcaster The broadcaster
 * @param node A node
 *
 * @return a status code
 */
retcode_t broadcaster_init(broadcaster_t *const broadcaster, node_t *const node);

/**
 * Starts a broadcaster
 *
 * @param broadcaster The broadcaster
 *
 * @return a status code
 */
retcode_t broadcaster_start(broadcaster_t *const broadcaster);

/**
 * Stops a broadcaster
 *
 * @param broadcaster The broadcaster
 *
 * @return a status code
 */
retcode_t broadcaster_stop(broadcaster_t *const broadcaster);

/**
 * Destroys a broadcaster
 *
 * @param broadcaster The broadcaster
 *
 * @return a status code
 */
retcode_t broadcaster_destroy(broadcaster_t *const broadcaster);

/**
 * Adds a packet to the broadcaster queue
 *
 * @param broadcaster The broadcaster
 * @param packet The packet
 *
 * @return a status code
 */
retcode_t broadcaster_on_next(broadcaster_t *const broadcaster, iota_packet_t const *const packet);

/**
 * Gets the size of the broadcaster queue
 *
 * @param broadcaster The broadcaster
 *
 * @return a status code
 */
size_t broadcaster_size(broadcaster_t *const broadcaster);

#ifdef __cplusplus
}
#endif

#endif  //__CIRI_NODE_PIPELINE_BROADCASTER_H__
