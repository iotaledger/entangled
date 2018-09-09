/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __GOSSIP_COMPONENTS_PROCESSOR_H__
#define __GOSSIP_COMPONENTS_PROCESSOR_H__

#include <stdbool.h>

#include "common/errors.h"
#include "utils/handles/thread.h"

// Forward declarations
typedef struct concurrent_queue_iota_packet_t_s processor_queue_t;
typedef struct iota_packet_s iota_packet_t;
typedef struct node_s node_t;

typedef struct processor_state_s {
  thread_handle_t thread;
  bool running;
  processor_queue_t *queue;
  node_t *node;
  size_t req_hash_size;
} processor_state_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initializes a processor
 *
 * @param state The processor state
 * @param node A node
 * @param testnet Testnet if true, Mainnet otherwise
 *
 * @return a status code
 */
retcode_t processor_init(processor_state_t *const state, node_t *const node,
                         bool testnet);

/**
 * Starts a processor
 *
 * @param state The processor state
 *
 * @return a status code
 */
retcode_t processor_start(processor_state_t *const state);

/**
 * Adds a packet to a processor queue
 *
 * @param state The processor state
 * @param packet The packet
 *
 * @return a status code
 */
retcode_t processor_on_next(processor_state_t *const state,
                            iota_packet_t const packet);

/**
 * Stops a processor
 *
 * @param state The processor state
 *
 * @return a status code
 */
retcode_t processor_stop(processor_state_t *const state);

/**
 * Destroys a processor
 *
 * @param state The processor state
 *
 * @return a status code
 */
retcode_t processor_destroy(processor_state_t *const state);

#ifdef __cplusplus
}
#endif

#endif  //__GOSSIP_COMPONENTS_PROCESSOR_H__
