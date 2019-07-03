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

#ifdef __cplusplus
extern "C" {
#endif

// Forward declarations
typedef struct node_s node_t;

/**
 * @brief The broadcaster takes care of broadcasting newly received transactions to all neighbors except the neighbor
 * from which the transaction originated from
 */
typedef struct broadcaster_s {
  // Metadata
  cond_handle_t cond;     /*!< Condition variable to wait/signal the broadcaster */
  rw_lock_handle_t lock;  /*!< Lock for the broadcaster queue */
  bool running;           /*!< State of the broadcaster */
  thread_handle_t thread; /*!< Handle for the broadcaster thread */
  // Data
  node_t *node;              /*!< The parent node */
  iota_packet_queue_t queue; /*!< A queue of packets to be broadcasted */
} broadcaster_t;

/**
 * @brief Initializes a broadcaster
 *
 * @param[out]  broadcaster The broadcaster
 * @param[in]   node        A node
 *
 * @return a status code
 */
retcode_t broadcaster_init(broadcaster_t *const broadcaster, node_t *const node);

/**
 * @brief Starts a broadcaster
 *
 * @param[out]  broadcaster The broadcaster
 *
 * @return a status code
 */
retcode_t broadcaster_start(broadcaster_t *const broadcaster);

/**
 * @brief Stops a broadcaster
 *
 * @param[out]  broadcaster The broadcaster
 *
 * @return a status code
 */
retcode_t broadcaster_stop(broadcaster_t *const broadcaster);

/**
 * @brief Destroys a broadcaster
 *
 * @param[out]  broadcaster The broadcaster
 *
 * @return a status code
 */
retcode_t broadcaster_destroy(broadcaster_t *const broadcaster);

/**
 * @brief Adds a packet to be broadcasted
 *
 * @param[out]  broadcaster The broadcaster
 * @param[in]   packet      The packet
 *
 * @return a status code
 */
retcode_t broadcaster_add(broadcaster_t *const broadcaster, iota_packet_t const *const packet);

/**
 * @brief Gets the size of the broadcaster queue
 *
 * @param[in] broadcaster The broadcaster
 *
 * @return a status code
 */
size_t broadcaster_size(broadcaster_t *const broadcaster);

#ifdef __cplusplus
}
#endif

#endif  //__CIRI_NODE_PIPELINE_BROADCASTER_H__
