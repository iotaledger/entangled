/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __CIRI_NODE_PIPELINE_BROADCASTER_H__
#define __CIRI_NODE_PIPELINE_BROADCASTER_H__

#include <stdbool.h>

#include "ciri/node/protocol/gossip.h"
#include "common/errors.h"
#include "utils/handles/cond.h"
#include "utils/handles/rw_lock.h"
#include "utils/handles/thread.h"

#ifdef __cplusplus
extern "C" {
#endif

// Forward declarations
typedef struct node_s node_t;

/**
 * @brief The broadcaster stage takes care of broadcasting newly received transactions to all neighbors except the
 * neighbor from which the transaction originated from
 */
typedef struct broadcaster_stage_s {
  // Metadata
  cond_handle_t cond;     /*!< Condition variable to wait/signal the broadcaster */
  rw_lock_handle_t lock;  /*!< Lock for the broadcaster queue */
  bool running;           /*!< State of the broadcaster */
  thread_handle_t thread; /*!< Handle for the broadcaster thread */
  // Data
  node_t *node;                  /*!< The parent node */
  protocol_gossip_queue_t queue; /*!< A queue of packets to be broadcasted */
} broadcaster_stage_t;

/**
 * @brief Initializes a broadcaster stage
 *
 * @param[out]  broadcaster The broadcaster stage
 * @param[in]   node        A node
 *
 * @return a status code
 */
retcode_t broadcaster_stage_init(broadcaster_stage_t *const broadcaster, node_t *const node);

/**
 * @brief Starts a broadcaster stage
 *
 * @param[out]  broadcaster The broadcaster stage
 *
 * @return a status code
 */
retcode_t broadcaster_stage_start(broadcaster_stage_t *const broadcaster);

/**
 * @brief Stops a broadcaster stage
 *
 * @param[out]  broadcaster The broadcaster stage
 *
 * @return a status code
 */
retcode_t broadcaster_stage_stop(broadcaster_stage_t *const broadcaster);

/**
 * @brief Destroys a broadcaster stage
 *
 * @param[out]  broadcaster The broadcaster stage
 *
 * @return a status code
 */
retcode_t broadcaster_stage_destroy(broadcaster_stage_t *const broadcaster);

/**
 * @brief Adds a packet to be broadcasted
 *
 * @param[out]  broadcaster The broadcaster stage
 * @param[in]   packet      The packet
 *
 * @return a status code
 */
retcode_t broadcaster_stage_add(broadcaster_stage_t *const broadcaster, protocol_gossip_t const *const packet);

/**
 * @brief Gets the size of the broadcaster stage queue
 *
 * @param[in] broadcaster The broadcaster stage
 *
 * @return a status code
 */
size_t broadcaster_stage_size(broadcaster_stage_t *const broadcaster);

#ifdef __cplusplus
}
#endif

#endif  //__CIRI_NODE_PIPELINE_BROADCASTER_H__
