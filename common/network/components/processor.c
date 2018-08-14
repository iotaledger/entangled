/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "common/network/components/processor.h"
#include "ciri/node.h"
#include "common/network/neighbor.h"
#include "utils/containers/lists/concurrent_list_neighbor.h"
#include "utils/logger_helper.h"

#define TX_BYTES_SIZE 1604
#define TX_TRITS_SIZE 8019

#define PROCESSOR_COMPONENT_LOGGER_ID "processor_component"

static bool pre_processor(processor_state_t *const state,
                          iota_packet_t *const packet) {
  neighbor_t *neighbor = NULL;

  if (state == NULL) {
    return false;
  }
  if (packet == NULL) {
    return false;
  }

  log_debug(PROCESSOR_COMPONENT_LOGGER_ID, "Pre-processing packet\n");
  neighbor = neighbor_find_by_endpoint(state->node->neighbors, &packet->source);
  if (neighbor == NULL) {
    return false;
  }

  // Transaction bytes
  // TODO(thibault): neighbor.incAllTransactions();
  // TODO(thibault): Randomly dropping transaction.
  // TODO(thibault): Compute cache-specific hash (faster)?
  // TODO(thibault): get from cache
  // if !cached
  if (true) {
    trit_t tx_trits[TX_TRITS_SIZE];
    bytes_to_trits((byte_t *)packet->content, TX_BYTES_SIZE, tx_trits,
                   TX_TRITS_SIZE);
    // TODO(thibault): create transaction ?
    // TODO(thibault): transaction validation
    // TODO(thibault): put to cache ? Why here ?
    //           synchronized(recentSeenBytes) {
    //             recentSeenBytes.put(byteHash, receivedTransactionHash);
    // TODO(thibault): if valid - add to receive queue
    //           addReceivedDataToReceiveQueue(receivedTransactionViewModel,
    //           neighbor);
  }

  // Request bytes
  //       // Request bytes
  //
  //       // add request to reply queue (requestedHash, neighbor)
  //       Hash requestedHash =
  //           new Hash(receivedData, TransactionViewModel.SIZE, reqHashSize);
  //       if (requestedHash.equals(receivedTransactionHash)) {
  //         // requesting a random tip
  //         requestedHash = Hash.NULL_HASH;
  //       }
  //
  //       addReceivedDataToReplyQueue(requestedHash, neighbor);

  return true;
}

static void *processor_routine(processor_state_t *const state) {
  iota_packet_t packet;

  if (state == NULL) {
    return NULL;
  }
  while (state->running) {
    if (state->queue->vtable->pop(state->queue, &packet) ==
        CONCURRENT_QUEUE_SUCCESS) {
      pre_processor(state, &packet);
    }
  }
  return NULL;
}

bool processor_init(processor_state_t *const state, node_t *const node) {
  if (state == NULL || node == NULL) {
    return false;
  }
  logger_helper_init(PROCESSOR_COMPONENT_LOGGER_ID, LOGGER_DEBUG, true);
  state->running = false;
  if (INIT_CONCURRENT_QUEUE_OF(iota_packet_t, state->queue) !=
      CONCURRENT_QUEUE_SUCCESS) {
    log_critical(PROCESSOR_COMPONENT_LOGGER_ID,
                 "Initializing processor queue failed\n");
    return false;
  }
  state->node = node;
  return true;
}

bool processor_start(processor_state_t *const state) {
  if (state == NULL) {
    return false;
  }
  log_info(PROCESSOR_COMPONENT_LOGGER_ID, "Spawning processor thread\n");
  state->running = true;
  if (thread_handle_create(&state->thread, (thread_routine_t)processor_routine,
                           state) != 0) {
    log_critical(PROCESSOR_COMPONENT_LOGGER_ID,
                 "Spawning processor thread failed\n");
    return false;
  }
  return true;
}

bool processor_on_next(processor_state_t *const state,
                       iota_packet_t const packet) {
  if (state == NULL) {
    return false;
  }
  if (state->queue->vtable->push(state->queue, packet) !=
      CONCURRENT_QUEUE_SUCCESS) {
    log_warning(PROCESSOR_COMPONENT_LOGGER_ID,
                "Pushing to processor queue failed\n");
    return false;
  }
  return true;
}

bool processor_stop(processor_state_t *const state) {
  bool ret = true;

  if (state == NULL) {
    return false;
  }
  log_info(PROCESSOR_COMPONENT_LOGGER_ID, "Shutting down processor thread\n");
  state->running = false;
  if (thread_handle_join(state->thread, NULL) != 0) {
    log_error(PROCESSOR_COMPONENT_LOGGER_ID,
              "Shutting down processor thread failed\n");
    ret = false;
  }
  return ret;
}

bool processor_destroy(processor_state_t *const state) {
  bool ret = true;

  if (state == NULL) {
    return false;
  }
  if (state->running) {
    return false;
  }
  if (DESTROY_CONCURRENT_QUEUE_OF(iota_packet_t, state->queue) !=
      CONCURRENT_QUEUE_SUCCESS) {
    log_error(PROCESSOR_COMPONENT_LOGGER_ID,
              "Destroying processor queue failed\n");
    ret = false;
  }
  return ret;
}
