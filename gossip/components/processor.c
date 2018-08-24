/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <string.h>

#include "ciri/core.h"
#include "common/storage/sql/defs.h"
#include "gossip/components/processor.h"
#include "utils/containers/lists/concurrent_list_neighbor.h"
#include "utils/containers/queues/concurrent_queue_packet.h"
#include "utils/logger_helper.h"

#define PROCESSOR_COMPONENT_LOGGER_ID "processor_component"

static retcode_t process_transaction_bytes(processor_state_t *const state,
                                           neighbor_t *const neighbor,
                                           iota_packet_t *const packet,
                                           iota_transaction_t *const tx) {
  retcode_t ret = RC_OK;
  bool exists = false;

  if (state == NULL) {
    return RC_PROCESSOR_COMPONENT_NULL_STATE;
  }
  if (neighbor == NULL) {
    return RC_PROCESSOR_COMPONENT_NULL_NEIGHBOR;
  }
  if (packet == NULL) {
    return RC_PROCESSOR_COMPONENT_NULL_PACKET;
  }
  if (tx == NULL) {
    return RC_PROCESSOR_COMPONENT_NULL_TX;
  }

  if (true /* TODO(thibault): if !cached */) {
    trit_t tx_trits[TX_TRITS_SIZE];
    flex_trit_t tx_flex_trits[FLEX_TRIT_SIZE_8019];

    bytes_to_trits(packet->content, TX_BYTES_SIZE, tx_trits, TX_TRITS_SIZE);
    flex_trits_from_trits(tx_flex_trits, TX_TRITS_SIZE, tx_trits, TX_TRITS_SIZE,
                          TX_TRITS_SIZE);
    if ((*tx = transaction_deserialize(tx_flex_trits)) == NULL) {
      neighbor->nbr_invalid_tx++;
      log_warning(PROCESSOR_COMPONENT_LOGGER_ID,
                  "Deserializing transaction failed\n");
      return RC_PROCESSOR_COMPONENT_INVALID_TX;
    }
    // TODO(thibault): Transaction validation
    // TODO(thibault): Add to cache

    TRIT_ARRAY_DECLARE(hash, NUM_TRITS_HASH);
    trit_array_set_trits(&hash, (*tx)->hash, NUM_TRITS_HASH);

    if ((ret = iota_tangle_transaction_exist(&state->node->core->tangle,
                                             COL_HASH, &hash, &exists))) {
      return ret;
    }
    if (exists == false) {
      neighbor->nbr_new_tx++;
      // Store new transaction
      log_debug(PROCESSOR_COMPONENT_LOGGER_ID, "Storing new transaction\n");
      if ((ret = iota_tangle_transaction_store(&state->node->core->tangle,
                                               *tx))) {
        log_warning(PROCESSOR_COMPONENT_LOGGER_ID,
                    "Storing new transaction failed\n");
        neighbor->nbr_invalid_tx++;
        return ret;
      }
      // TODO(thibault): Store transaction metadata
      // Broadcast new transaction
      log_debug(PROCESSOR_COMPONENT_LOGGER_ID,
                "Propagating packet to broadcaster\n");
      if ((ret = broadcaster_on_next(&state->node->broadcaster, *packet))) {
        log_warning(PROCESSOR_COMPONENT_LOGGER_ID,
                    "Propagating packet to broadcaster failed\n");
        return ret;
      }
    }
  }
  return RC_OK;
}

static retcode_t process_request_bytes(processor_state_t *const state,
                                       neighbor_t *const neighbor,
                                       iota_packet_t *const packet,
                                       iota_transaction_t const tx) {
  retcode_t ret = RC_OK;
  trit_t request_hash_trits[NUM_TRITS_HASH] = {0};
  trit_array_p request_hash = NULL;

  if (state == NULL) {
    return RC_PROCESSOR_COMPONENT_NULL_STATE;
  }
  if (neighbor == NULL) {
    return RC_PROCESSOR_COMPONENT_NULL_NEIGHBOR;
  }
  if (packet == NULL) {
    return RC_PROCESSOR_COMPONENT_NULL_PACKET;
  }
  if (tx == NULL) {
    return RC_PROCESSOR_COMPONENT_NULL_TX;
  }

  bytes_to_trits(packet->content + TX_BYTES_SIZE, REQ_HASH_BYTES_SIZE,
                 request_hash_trits, NUM_TRITS_HASH);
  if ((request_hash = trit_array_new(NUM_TRITS_HASH)) == NULL) {
    return RC_PROCESSOR_COMPONENT_OOM;
  }
  flex_trits_from_trits(request_hash->trits, NUM_TRITS_HASH, request_hash_trits,
                        NUM_TRITS_HASH, NUM_TRITS_HASH);
  if (memcmp(request_hash->trits, tx->hash, request_hash->num_bytes) == 0) {
    // If requested hash is equal to transaction hash: request a random tip
    trit_array_set_null(request_hash);
  }
  log_debug(PROCESSOR_COMPONENT_LOGGER_ID, "Propagating packet to responder\n");
  if ((ret = responder_on_next(&state->node->responder, neighbor,
                               request_hash))) {
    log_warning(PROCESSOR_COMPONENT_LOGGER_ID,
                "Propagating packet to responder failed\n");
    return ret;
  }
  return RC_OK;
}

static retcode_t process_packet(processor_state_t *const state,
                                iota_packet_t *const packet) {
  neighbor_t *neighbor = NULL;
  iota_transaction_t tx = NULL;

  if (state == NULL) {
    return RC_PROCESSOR_COMPONENT_NULL_STATE;
  }
  if (packet == NULL) {
    return RC_PROCESSOR_COMPONENT_NULL_PACKET;
  }

  neighbor = neighbor_find_by_endpoint(state->node->neighbors, &packet->source);

  if (neighbor) {
    neighbor->nbr_all_tx++;

    // TODO(thibault): Random drop transaction

    log_debug(PROCESSOR_COMPONENT_LOGGER_ID, "Processing transaction bytes\n");
    if (process_transaction_bytes(state, neighbor, packet, &tx)) {
      log_warning(PROCESSOR_COMPONENT_LOGGER_ID,
                  "Processing transaction bytes failed\n");
      return RC_PROCESSOR_COMPONENT_FAILED_TX_PROCESSING;
    }

    log_debug(PROCESSOR_COMPONENT_LOGGER_ID, "Processing request bytes\n");
    if (process_request_bytes(state, neighbor, packet, tx)) {
      log_warning(PROCESSOR_COMPONENT_LOGGER_ID,
                  "Processing request bytes failed\n");
      return RC_PROCESSOR_COMPONENT_FAILED_REQ_PROCESSING;
    }

    // TODO(thibault): Recent seen bytes statistics
  } else {
    // TODO(thibault): Testnet add non-tethered neighbor
  }

  return RC_OK;
}

static void *processor_routine(processor_state_t *const state) {
  iota_packet_t packet;

  if (state == NULL) {
    return NULL;
  }

  while (state->running) {
    if (CQ_POP(state->queue, &packet) == CQ_SUCCESS) {
      log_debug(PROCESSOR_COMPONENT_LOGGER_ID, "Processing packet\n");
      if (process_packet(state, &packet)) {
        log_warning(PROCESSOR_COMPONENT_LOGGER_ID,
                    "Processing packet failed\n");
      }
    }
  }
  return NULL;
}

retcode_t processor_init(processor_state_t *const state, node_t *const node) {
  if (state == NULL) {
    return RC_PROCESSOR_COMPONENT_NULL_STATE;
  }
  if (node == NULL) {
    return RC_PROCESSOR_COMPONENT_NULL_NODE;
  }

  logger_helper_init(PROCESSOR_COMPONENT_LOGGER_ID, LOGGER_DEBUG, true);
  memset(state, 0, sizeof(processor_state_t));
  state->running = false;
  state->node = node;

  log_debug(PROCESSOR_COMPONENT_LOGGER_ID, "Initializing processor queue\n");
  if (CQ_INIT(iota_packet_t, state->queue) != CQ_SUCCESS) {
    log_critical(PROCESSOR_COMPONENT_LOGGER_ID,
                 "Initializing processor queue failed\n");
    return RC_PROCESSOR_COMPONENT_FAILED_INIT_QUEUE;
  }

  return RC_OK;
}

retcode_t processor_start(processor_state_t *const state) {
  if (state == NULL) {
    return RC_PROCESSOR_COMPONENT_NULL_STATE;
  }

  log_info(PROCESSOR_COMPONENT_LOGGER_ID, "Spawning processor thread\n");
  state->running = true;
  if (thread_handle_create(&state->thread, (thread_routine_t)processor_routine,
                           state) != 0) {
    log_critical(PROCESSOR_COMPONENT_LOGGER_ID,
                 "Spawning processor thread failed\n");
    return RC_PROCESSOR_COMPONENT_FAILED_THREAD_SPAWN;
  }
  return RC_OK;
}

retcode_t processor_on_next(processor_state_t *const state,
                            iota_packet_t const packet) {
  if (state == NULL) {
    return RC_PROCESSOR_COMPONENT_NULL_STATE;
  }

  if (CQ_PUSH(state->queue, packet) != CQ_SUCCESS) {
    log_warning(PROCESSOR_COMPONENT_LOGGER_ID,
                "Adding packet to processor queue failed\n");
    return RC_PROCESSOR_COMPONENT_FAILED_ADD_QUEUE;
  }
  return RC_OK;
}

retcode_t processor_stop(processor_state_t *const state) {
  bool ret = RC_OK;

  if (state == NULL) {
    return RC_PROCESSOR_COMPONENT_NULL_STATE;
  }

  log_info(PROCESSOR_COMPONENT_LOGGER_ID, "Shutting down processor thread\n");
  state->running = false;
  if (thread_handle_join(state->thread, NULL) != 0) {
    log_error(PROCESSOR_COMPONENT_LOGGER_ID,
              "Shutting down processor thread failed\n");
    ret = RC_PROCESSOR_COMPONENT_FAILED_THREAD_JOIN;
  }
  return ret;
}

retcode_t processor_destroy(processor_state_t *const state) {
  bool ret = RC_OK;

  if (state == NULL) {
    return RC_PROCESSOR_COMPONENT_NULL_STATE;
  }
  if (state->running) {
    return RC_PROCESSOR_COMPONENT_STILL_RUNNING;
  }

  log_debug(PROCESSOR_COMPONENT_LOGGER_ID, "Destroying processor queue\n");
  if (CQ_DESTROY(iota_packet_t, state->queue) != CQ_SUCCESS) {
    log_error(PROCESSOR_COMPONENT_LOGGER_ID,
              "Destroying processor queue failed\n");
    ret = RC_PROCESSOR_COMPONENT_FAILED_DESTROY_QUEUE;
  }
  return ret;
}
