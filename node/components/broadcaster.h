/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __NODE_COMPONENTS_BROADCASTER_H__
#define __NODE_COMPONENTS_BROADCASTER_H__

#include <stdbool.h>

#include "common/errors.h"
#include "utils/containers/hash/hash8019_queue.h"
#include "utils/handles/cond.h"
#include "utils/handles/rw_lock.h"
#include "utils/handles/thread.h"

// Forward declarations
typedef struct node_s node_t;

typedef struct broadcaster_s {
  thread_handle_t thread;
  bool running;
  node_t *node;
  hash8019_queue_t queue;
  rw_lock_handle_t lock;
  cond_handle_t cond;
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
 * Destroys a broadcaster
 *
 * @param broadcaster The broadcaster
 *
 * @return a status code
 */
retcode_t broadcaster_destroy(broadcaster_t *const broadcaster);

/**
 * Starts a broadcaster
 *
 * @param broadcaster The broadcaster
 *
 * @return a status code
 */
retcode_t broadcaster_start(broadcaster_t *const broadcaster);

/**
 * Adds transaction flex trits to the broadcaster queue
 *
 * @param broadcaster The broadcaster
 * @param flex_trits Transaction flex trits
 *
 * @return a status code
 */
retcode_t broadcaster_on_next(broadcaster_t *const broadcaster, flex_trit_t const *const flex_trits);

/**
 * Gets the size of the broadcaster queue
 *
 * @param broadcaster The broadcaster
 *
 * @return a status code
 */
size_t broadcaster_size(broadcaster_t *const broadcaster);

/**
 * Stops a broadcaster
 *
 * @param broadcaster The broadcaster
 *
 * @return a status code
 */
retcode_t broadcaster_stop(broadcaster_t *const broadcaster);

#ifdef __cplusplus
}
#endif

#endif  //__NODE_COMPONENTS_BROADCASTER_H__
