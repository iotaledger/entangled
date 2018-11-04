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
#include "consensus/transaction_validator/transaction_validator.h"
#include "gossip/iota_packet.h"
#include "utils/handles/cond.h"
#include "utils/handles/lock.h"
#include "utils/handles/rw_lock.h"
#include "utils/handles/thread.h"

// Forward declarations
typedef struct node_s node_t;
typedef struct tangle_s tangle_t;

typedef struct processor_s {
  thread_handle_t thread;
  bool running;
  iota_packet_queue_t queue;
  rw_lock_handle_t lock;
  cond_handle_t cond;
  node_t *node;
  tangle_t *tangle;
  transaction_validator_t *transaction_validator;
} processor_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initializes a processor
 *
 * @param processor The processor state
 * @param node A node
 * @param tangle A tangle
 *
 * @return a status code
 */
retcode_t processor_init(processor_t *const processor, node_t *const node,
                         tangle_t *const tangle,
                         transaction_validator_t *const transaction_validator);

/**
 * Starts a processor
 *
 * @param processor The processor state
 *
 * @return a status code
 */
retcode_t processor_start(processor_t *const processor);

/**
 * Stops a processor
 *
 * @param processor The processor state
 *
 * @return a status code
 */
retcode_t processor_stop(processor_t *const processor);

/**
 * Destroys a processor
 *
 * @param processor The processor state
 *
 * @return a status code
 */
retcode_t processor_destroy(processor_t *const processor);

/**
 * Adds a packet to a processor queue
 *
 * @param processor The processor state
 * @param packet The packet
 *
 * @return a status code
 */
retcode_t processor_on_next(processor_t *const processor,
                            iota_packet_t const packet);

/**
 * Gets the size of the processor queue
 *
 * @param processor The processor
 *
 * @return a status code
 */
size_t processor_size(processor_t *const processor);

#ifdef __cplusplus
}
#endif

#endif  //__GOSSIP_COMPONENTS_PROCESSOR_H__
