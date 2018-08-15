/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <string.h>

#include "ciri/core.h"
#include "ciri/node.h"
#include "common/model/transaction.h"
#include "common/network/components/processor.h"
#include "common/network/neighbor.h"
#include "common/storage/storage.h"
#include "utils/containers/lists/concurrent_list_neighbor.h"
#include "utils/logger_helper.h"

#define PROCESSOR_COMPONENT_LOGGER_ID "processor_component"

static bool process(processor_state_t *const state, iota_transaction_t const tx,
                    neighbor_t *const neighbor) {
  bool exists = false;

  if (state == NULL) {
    return false;
  }
  if (tx == NULL) {
    return false;
  }
  if (neighbor == NULL) {
    return false;
  }

  // if (iota_stor_exist(&state->node->core->db_conn, COL_HASH, hash, &exists))
  // {
  //   return false;
  // }
  if (exists == false) {
    // Store new transaction
    if (iota_stor_store(&state->node->core->db_conn, tx)) {
      neighbor->nbr_invalid_tx++;
      return false;
    }
    neighbor->nbr_new_tx++;
    // receivedTransactionViewModel.setArrivalTime(System.currentTimeMillis());
    // transactionValidator.updateStatus(receivedTransactionViewModel);
    // receivedTransactionViewModel.update(tangle, "arrivalTime|sender");
  }

  // // if new, then broadcast to all neighbors
  // neighbor.incNewTransactions();
  // broadcast(receivedTransactionViewModel);
  return true;
}

static bool pre_process(processor_state_t *const state,
                        iota_packet_t *const packet) {
  neighbor_t *neighbor = NULL;
  iota_transaction_t tx = NULL;
  trit_array_p request_hash = NULL;

  if (state == NULL) {
    return false;
  }
  if (packet == NULL) {
    return false;
  }

  log_debug(PROCESSOR_COMPONENT_LOGGER_ID, "Pre-processing packet\n");
  neighbor = neighbor_find_by_endpoint(state->node->neighbors, &packet->source);

  if (neighbor) {
    neighbor->nbr_all_tx++;
    // Transaction bytes
    // TODO(thibault): Random drop transaction
    // TODO(thibault): If !cached
    if (true) {
      trit_t tx_trits[TX_TRITS_SIZE];
      flex_trit_t tx_flex_trits[FLEX_TRIT_SIZE_8019];

      bytes_to_trits(packet->content, TX_BYTES_SIZE, tx_trits, TX_TRITS_SIZE);
      int8_to_flex_trit_array(tx_flex_trits, FLEX_TRIT_SIZE_8019, tx_trits,
                              TX_TRITS_SIZE, TX_TRITS_SIZE);
      if ((tx = transaction_deserialize(tx_flex_trits)) == NULL) {
        return false;
      }
      // TODO(thibault): Transaction validation
      // TODO(thibault): Add to cache
      // TODO(thibault): Add to receive queue
      process(state, tx, neighbor);
    }

    // Request bytes
    {
      trit_t request_hash_trits[NUM_TRITS_HASH] = {0};
      bytes_to_trits(packet->content + TX_BYTES_SIZE, REQ_HASH_BYTES_SIZE,
                     request_hash_trits, NUM_TRITS_HASH);
      if ((request_hash = trit_array_new(NUM_TRITS_HASH)) == NULL) {
        return false;
      }
      int8_to_flex_trit_array(request_hash->trits, request_hash->num_bytes,
                              request_hash_trits, NUM_TRITS_HASH,
                              NUM_TRITS_HASH);
    }
    if (memcmp(request_hash->trits, tx->hash, request_hash->num_bytes) == 0) {
      // If requested hash is equal to transaction hash: request a random tip
      trit_array_set_null(request_hash);
    }
    responder_on_next(&state->node->responder, request_hash, neighbor);

    // TODO(thibault): Recent seen bytes statistics
  } else {
    // TODO(thibault): Testnet add non-tethered neighbor
  }

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
      pre_process(state, &packet);
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
