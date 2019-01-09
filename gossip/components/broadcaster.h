/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __GOSSIP_COMPONENTS_BROADCASTER_H__
#define __GOSSIP_COMPONENTS_BROADCASTER_H__

#include <stdbool.h>

#include "common/errors.h"
#include "utils/containers/lock_free/lf_mpmc_queue_flex_trit_t.h"
#include "utils/handles/thread.h"

// Forward declarations
typedef struct node_s node_t;

typedef struct broadcaster_s {
  thread_handle_t thread;
  bool running;
  node_t *node;
  lf_mpmc_queue_flex_trit_t_t queue;
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
retcode_t broadcaster_init(broadcaster_t *const broadcaster,
                           node_t *const node);

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
retcode_t broadcaster_on_next(broadcaster_t *const broadcaster,
                              flex_trit_t const *const flex_trits);

/**
 * Stops a broadcaster
 *
 * @param broadcaster The broadcaster
 *
 * @return a status code
 */
retcode_t broadcaster_stop(broadcaster_t *const broadcaster);

/**
 * Gets the size of the broadcaster queue
 *
 * @param broadcaster The broadcaster
 *
 * @return a status code
 */
static inline size_t broadcaster_size(broadcaster_t const *const broadcaster) {
  if (broadcaster == NULL) {
    return 0;
  }

  return lf_mpmc_queue_flex_trit_t_count(&broadcaster->queue);
}

#ifdef __cplusplus
}
#endif

#endif  //__GOSSIP_COMPONENTS_BROADCASTER_H__
