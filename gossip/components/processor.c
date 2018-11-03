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

#define PROCESSOR_COMPONENT_LOGGER_ID "processor"

/*
 * Private functions
 */

static retcode_t process_transaction_bytes(processor_state_t *const processor,
                                           neighbor_t *const neighbor,
                                           iota_packet_t *const packet,
                                           iota_transaction_t transaction) {
  retcode_t ret = RC_OK;

  if (processor == NULL || neighbor == NULL || packet == NULL ||
      transaction == NULL) {
    return RC_NULL_PARAM;
  }

  // TODO(thibault): if !cached
  if (true) {
    bool exists = false;
    flex_trit_t transaction_flex_trits[FLEX_TRIT_SIZE_8019];
    trit_array_t hash = {.trits = NULL,
                         .num_trits = HASH_LENGTH_TRIT,
                         .num_bytes = FLEX_TRIT_SIZE_243,
                         .dynamic = 0};

    flex_trits_from_bytes(transaction_flex_trits,
                          NUM_TRITS_SERIALIZED_TRANSACTION, packet->content,
                          NUM_TRITS_SERIALIZED_TRANSACTION,
                          NUM_TRITS_SERIALIZED_TRANSACTION);
    if (transaction_deserialize_from_trits(transaction,
                                           transaction_flex_trits) == 0) {
      neighbor->nbr_invalid_tx++;
      log_warning(PROCESSOR_COMPONENT_LOGGER_ID,
                  "Deserializing transaction failed\n");
      return RC_PROCESSOR_COMPONENT_INVALID_TX;
    }
    // TODO(thibault): Transaction validation
    // TODO(thibault): Add to cache

    hash.trits = transaction->hash;
    if ((ret = iota_tangle_transaction_exist(
             processor->tangle, TRANSACTION_FIELD_HASH, &hash, &exists))) {
      return ret;
    }
    if (exists == false) {
      neighbor->nbr_new_tx++;
      // Store new transaction
      log_debug(PROCESSOR_COMPONENT_LOGGER_ID, "Storing new transaction\n");
      if ((ret =
               iota_tangle_transaction_store(processor->tangle, transaction))) {
        log_warning(PROCESSOR_COMPONENT_LOGGER_ID,
                    "Storing new transaction failed\n");
        neighbor->nbr_invalid_tx++;
        return ret;
      }
      // TODO(thibault): Store transaction metadata
      // Broadcast new transaction
      log_debug(PROCESSOR_COMPONENT_LOGGER_ID,
                "Propagating packet to broadcaster\n");
      if ((ret = broadcaster_on_next(&processor->node->broadcaster,
                                     transaction_flex_trits))) {
        log_warning(PROCESSOR_COMPONENT_LOGGER_ID,
                    "Propagating packet to broadcaster failed\n");
        return ret;
      }
    }
  }
  return RC_OK;
}

static retcode_t process_request_bytes(processor_state_t *const processor,
                                       neighbor_t *const neighbor,
                                       iota_packet_t *const packet,
                                       iota_transaction_t const transaction) {
  retcode_t ret = RC_OK;
  trit_t request_hash_trits[NUM_TRITS_HASH] = {0};
  trit_array_p request_hash = NULL;

  if (processor == NULL || neighbor == NULL || packet == NULL ||
      transaction == NULL) {
    return RC_NULL_PARAM;
  }

  bytes_to_trits(packet->content + PACKET_TX_SIZE, REQUEST_HASH_SIZE,
                 request_hash_trits, REQUEST_HASH_SIZE_TRITS);
  if ((request_hash = trit_array_new(NUM_TRITS_HASH)) == NULL) {
    return RC_PROCESSOR_COMPONENT_OOM;
  }
  flex_trits_from_trits(request_hash->trits, NUM_TRITS_HASH, request_hash_trits,
                        NUM_TRITS_HASH, NUM_TRITS_HASH);
  if (memcmp(request_hash->trits, transaction->hash, request_hash->num_bytes) ==
      0) {
    // If requested hash is equal to transaction hash: request a random tip
    trit_array_set_null(request_hash);
  }

  if ((ret = iota_consensus_transaction_solidifier_check_and_update_solid_state(
           &processor->node->core->consensus.transaction_solidifier,
           transaction->hash))) {
    return ret;
  }
  log_debug(PROCESSOR_COMPONENT_LOGGER_ID, "Propagating packet to responder\n");
  if ((ret = responder_on_next(&processor->node->responder, neighbor,
                               request_hash))) {
    log_warning(PROCESSOR_COMPONENT_LOGGER_ID,
                "Propagating packet to responder failed\n");
    return ret;
  }
  return RC_OK;
}

static retcode_t process_packet(processor_state_t *const processor,
                                iota_packet_t *const packet) {
  retcode_t ret = RC_OK;
  neighbor_t *neighbor = NULL;
  struct _iota_transaction transaction;

  if (processor == NULL || packet == NULL) {
    return RC_NULL_PARAM;
  }

  transaction.snapshot_index = 0;
  transaction.solid = 0;

  neighbor =
      neighbor_find_by_endpoint(processor->node->neighbors, &packet->source);

  if (neighbor) {
    neighbor->nbr_all_tx++;

    // TODO(thibault): Random drop transaction

    log_debug(PROCESSOR_COMPONENT_LOGGER_ID, "Processing transaction bytes\n");
    if ((ret = process_transaction_bytes(processor, neighbor, packet,
                                         &transaction)) != RC_OK) {
      log_warning(PROCESSOR_COMPONENT_LOGGER_ID,
                  "Processing transaction bytes failed\n");
      return ret;
    }

    log_debug(PROCESSOR_COMPONENT_LOGGER_ID, "Processing request bytes\n");
    if ((ret = process_request_bytes(processor, neighbor, packet,
                                     &transaction)) != RC_OK) {
      log_warning(PROCESSOR_COMPONENT_LOGGER_ID,
                  "Processing request bytes failed\n");
      return ret;
    }

    // TODO(thibault): Recent seen bytes statistics
  } else {
    // TODO(thibault): Testnet add non-tethered neighbor
  }

  return ret;
}

static void *processor_routine(processor_state_t *const processor) {
  iota_packet_t packet;

  if (processor == NULL) {
    return NULL;
  }

  while (processor->running) {
    if (CQ_POP(processor->queue, &packet) == CQ_SUCCESS) {
      log_debug(PROCESSOR_COMPONENT_LOGGER_ID, "Processing packet\n");
      if (process_packet(processor, &packet)) {
        log_warning(PROCESSOR_COMPONENT_LOGGER_ID,
                    "Processing packet failed\n");
      }
    }
  }
  return NULL;
}

/*
 * Public functions
 */

retcode_t processor_init(processor_state_t *const processor, node_t *const node,
                         tangle_t *const tangle) {
  if (processor == NULL || node == NULL || tangle == NULL) {
    return RC_NULL_PARAM;
  }

  logger_helper_init(PROCESSOR_COMPONENT_LOGGER_ID, LOGGER_DEBUG, true);
  memset(processor, 0, sizeof(processor_state_t));
  processor->running = false;
  processor->node = node;
  processor->tangle = tangle;

  log_debug(PROCESSOR_COMPONENT_LOGGER_ID, "Initializing processor queue\n");
  if (CQ_INIT(iota_packet_t, processor->queue) != CQ_SUCCESS) {
    log_critical(PROCESSOR_COMPONENT_LOGGER_ID,
                 "Initializing processor queue failed\n");
    return RC_PROCESSOR_COMPONENT_FAILED_INIT_QUEUE;
  }

  return RC_OK;
}

retcode_t processor_start(processor_state_t *const processor) {
  if (processor == NULL) {
    return RC_NULL_PARAM;
  }

  log_info(PROCESSOR_COMPONENT_LOGGER_ID, "Spawning processor thread\n");
  processor->running = true;
  if (thread_handle_create(&processor->thread,
                           (thread_routine_t)processor_routine,
                           processor) != 0) {
    log_critical(PROCESSOR_COMPONENT_LOGGER_ID,
                 "Spawning processor thread failed\n");
    return RC_PROCESSOR_COMPONENT_FAILED_THREAD_SPAWN;
  }
  return RC_OK;
}

retcode_t processor_on_next(processor_state_t *const processor,
                            iota_packet_t const packet) {
  if (processor == NULL) {
    return RC_NULL_PARAM;
  }

  if (CQ_PUSH(processor->queue, packet) != CQ_SUCCESS) {
    log_warning(PROCESSOR_COMPONENT_LOGGER_ID,
                "Adding packet to processor queue failed\n");
    return RC_PROCESSOR_COMPONENT_FAILED_ADD_QUEUE;
  }
  return RC_OK;
}

retcode_t processor_stop(processor_state_t *const processor) {
  bool ret = RC_OK;

  if (processor == NULL) {
    return RC_NULL_PARAM;
  } else if (processor->running == false) {
    return RC_OK;
  }

  log_info(PROCESSOR_COMPONENT_LOGGER_ID, "Shutting down processor thread\n");
  processor->running = false;
  if (thread_handle_join(processor->thread, NULL) != 0) {
    log_error(PROCESSOR_COMPONENT_LOGGER_ID,
              "Shutting down processor thread failed\n");
    ret = RC_PROCESSOR_COMPONENT_FAILED_THREAD_JOIN;
  }
  return ret;
}

retcode_t processor_destroy(processor_state_t *const processor) {
  bool ret = RC_OK;

  if (processor == NULL) {
    return RC_NULL_PARAM;
  } else if (processor->running) {
    return RC_PROCESSOR_COMPONENT_STILL_RUNNING;
  }

  log_debug(PROCESSOR_COMPONENT_LOGGER_ID, "Destroying processor queue\n");
  if (CQ_DESTROY(iota_packet_t, processor->queue) != CQ_SUCCESS) {
    log_error(PROCESSOR_COMPONENT_LOGGER_ID,
              "Destroying processor queue failed\n");
    ret = RC_PROCESSOR_COMPONENT_FAILED_DESTROY_QUEUE;
  }
  return ret;
}
