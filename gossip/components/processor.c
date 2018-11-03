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

#define PROCESSOR_LOGGER_ID "processor"

/*
 * Private functions
 */

/**
 * Converts transaction bytes from a packet to a transaction and validates it.
 * If valid and new: stores and broadcasts it.
 *
 * @param processor The processor state
 * @param neighbor The neighbor that sent the packet
 * @param packet The packet from which to process transaction bytes
 * @param hash A hash to be filled with transaction bytes hash
 *
 * @return a status code
 */
static retcode_t process_transaction_bytes(
    processor_state_t const *const processor, neighbor_t *const neighbor,
    iota_packet_t const *const packet, flex_trit_t *const hash) {
  retcode_t ret = RC_OK;

  if (processor == NULL || neighbor == NULL || packet == NULL || hash == NULL) {
    return RC_NULL_PARAM;
  }

  // TODO Check if transaction hash is cached

  if (false) {
  } else {
    bool exists = false;
    struct _iota_transaction transaction = {.snapshot_index = 0, .solid = 0};
    flex_trit_t transaction_flex_trits[FLEX_TRIT_SIZE_8019];
    trit_array_t key = {.trits = hash,
                        .num_trits = HASH_LENGTH_TRIT,
                        .num_bytes = FLEX_TRIT_SIZE_243,
                        .dynamic = 0};

    if (flex_trits_from_bytes(transaction_flex_trits,
                              NUM_TRITS_SERIALIZED_TRANSACTION, packet->content,
                              NUM_TRITS_SERIALIZED_TRANSACTION,
                              NUM_TRITS_SERIALIZED_TRANSACTION) !=
        NUM_TRITS_SERIALIZED_TRANSACTION) {
      log_warning(PROCESSOR_LOGGER_ID, "Invalid transaction bytes\n");
      ret = RC_PROCESSOR_INVALID_TRANSACTION;
      goto failure;
    }

    if (transaction_deserialize_from_trits(&transaction,
                                           transaction_flex_trits) !=
        NUM_TRITS_SERIALIZED_TRANSACTION) {
      log_warning(PROCESSOR_LOGGER_ID, "Deserializing transaction failed\n");
      ret = RC_PROCESSOR_INVALID_TRANSACTION;
      goto failure;
    }

    if (!iota_consensus_transaction_validate(processor->transaction_validator,
                                             &transaction)) {
      log_warning(PROCESSOR_LOGGER_ID, "Invalid transaction\n");
      ret = RC_PROCESSOR_INVALID_TRANSACTION;
      goto failure;
    }

    // TODO Add transaction hash to cache

    memcpy(hash, transaction.hash, FLEX_TRIT_SIZE_243);

    if ((ret = iota_tangle_transaction_exist(
             processor->tangle, TRANSACTION_FIELD_HASH, &key, &exists)) !=
        RC_OK) {
      log_warning(PROCESSOR_LOGGER_ID,
                  "Checking if transaction exists failed\n");
      goto failure;
    }

    if (!exists) {
      // Store new transaction
      log_debug(PROCESSOR_LOGGER_ID, "Storing new transaction\n");
      if ((ret = iota_tangle_transaction_store(processor->tangle,
                                               &transaction)) != RC_OK) {
        log_warning(PROCESSOR_LOGGER_ID, "Storing new transaction failed\n");
        goto failure;
      }

      // TODO Store transaction metadata

      // Broadcast new transaction
      log_debug(PROCESSOR_LOGGER_ID, "Propagating packet to broadcaster\n");
      if ((ret = broadcaster_on_next(&processor->node->broadcaster,
                                     transaction_flex_trits)) != RC_OK) {
        log_warning(PROCESSOR_LOGGER_ID,
                    "Propagating packet to broadcaster failed\n");
        goto failure;
      }

      neighbor->nbr_new_tx++;
    }
  }

  return ret;

failure:
  neighbor->nbr_invalid_tx++;
  return ret;
}

/**
 * Converts request bytes to a hash, check its solidity and adds it to the
 * responder queue.
 *
 * @param processor The processor state
 * @param neighbor The neighbor that sent the packet
 * @param packet The packet from which to process request bytes
 * @param hash Transaction bytes hash
 *
 * @return a status code
 */
static retcode_t process_request_bytes(processor_state_t *const processor,
                                       neighbor_t *const neighbor,
                                       iota_packet_t *const packet,
                                       flex_trit_t *const hash) {
  retcode_t ret = RC_OK;
  trit_t request_hash_trits[NUM_TRITS_HASH] = {0};
  trit_array_p request_hash = NULL;

  if (processor == NULL || neighbor == NULL || packet == NULL || hash == NULL) {
    return RC_NULL_PARAM;
  }

  bytes_to_trits(packet->content + PACKET_TX_SIZE, REQUEST_HASH_SIZE,
                 request_hash_trits, REQUEST_HASH_SIZE_TRITS);
  if ((request_hash = trit_array_new(NUM_TRITS_HASH)) == NULL) {
    return RC_PROCESSOR_OOM;
  }
  flex_trits_from_trits(request_hash->trits, NUM_TRITS_HASH, request_hash_trits,
                        NUM_TRITS_HASH, NUM_TRITS_HASH);
  if (memcmp(request_hash->trits, hash, request_hash->num_bytes) == 0) {
    // If requested hash is equal to transaction hash: request a random tip
    trit_array_set_null(request_hash);
  }

  if ((ret = iota_consensus_transaction_solidifier_check_and_update_solid_state(
           &processor->node->core->consensus.transaction_solidifier, hash))) {
    return ret;
  }
  log_debug(PROCESSOR_LOGGER_ID, "Propagating packet to responder\n");
  if ((ret = responder_on_next(&processor->node->responder, neighbor,
                               request_hash))) {
    log_warning(PROCESSOR_LOGGER_ID,
                "Propagating packet to responder failed\n");
    return ret;
  }
  return RC_OK;
}

static retcode_t process_packet(processor_state_t *const processor,
                                iota_packet_t *const packet) {
  retcode_t ret = RC_OK;
  neighbor_t *neighbor = NULL;
  flex_trit_t hash[FLEX_TRIT_SIZE_243];

  if (processor == NULL || packet == NULL) {
    return RC_NULL_PARAM;
  }

  neighbor =
      neighbor_find_by_endpoint(processor->node->neighbors, &packet->source);

  if (neighbor) {
    neighbor->nbr_all_tx++;

    log_debug(PROCESSOR_LOGGER_ID, "Processing transaction bytes\n");
    if ((ret = process_transaction_bytes(processor, neighbor, packet, hash)) !=
        RC_OK) {
      log_warning(PROCESSOR_LOGGER_ID, "Processing transaction bytes failed\n");
      return ret;
    }

    log_debug(PROCESSOR_LOGGER_ID, "Processing request bytes\n");
    if ((ret = process_request_bytes(processor, neighbor, packet, hash)) !=
        RC_OK) {
      log_warning(PROCESSOR_LOGGER_ID, "Processing request bytes failed\n");
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
      log_debug(PROCESSOR_LOGGER_ID, "Processing packet\n");
      if (process_packet(processor, &packet)) {
        log_warning(PROCESSOR_LOGGER_ID, "Processing packet failed\n");
      }
    }
  }
  return NULL;
}

/*
 * Public functions
 */

retcode_t processor_init(processor_state_t *const processor, node_t *const node,
                         tangle_t *const tangle,
                         transaction_validator_t *const transaction_validator) {
  if (processor == NULL || node == NULL || tangle == NULL ||
      transaction_validator == NULL) {
    return RC_NULL_PARAM;
  }

  logger_helper_init(PROCESSOR_LOGGER_ID, LOGGER_DEBUG, true);
  memset(processor, 0, sizeof(processor_state_t));
  processor->running = false;
  processor->node = node;
  processor->tangle = tangle;
  processor->transaction_validator = transaction_validator;

  log_debug(PROCESSOR_LOGGER_ID, "Initializing processor queue\n");
  if (CQ_INIT(iota_packet_t, processor->queue) != CQ_SUCCESS) {
    log_critical(PROCESSOR_LOGGER_ID, "Initializing processor queue failed\n");
    return RC_PROCESSOR_FAILED_INIT_QUEUE;
  }

  return RC_OK;
}

retcode_t processor_start(processor_state_t *const processor) {
  if (processor == NULL) {
    return RC_NULL_PARAM;
  }

  log_info(PROCESSOR_LOGGER_ID, "Spawning processor thread\n");
  processor->running = true;
  if (thread_handle_create(&processor->thread,
                           (thread_routine_t)processor_routine,
                           processor) != 0) {
    log_critical(PROCESSOR_LOGGER_ID, "Spawning processor thread failed\n");
    return RC_FAILED_THREAD_SPAWN;
  }
  return RC_OK;
}

retcode_t processor_on_next(processor_state_t *const processor,
                            iota_packet_t const packet) {
  if (processor == NULL) {
    return RC_NULL_PARAM;
  }

  if (CQ_PUSH(processor->queue, packet) != CQ_SUCCESS) {
    log_warning(PROCESSOR_LOGGER_ID,
                "Adding packet to processor queue failed\n");
    return RC_PROCESSOR_FAILED_ADD_QUEUE;
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

  log_info(PROCESSOR_LOGGER_ID, "Shutting down processor thread\n");
  processor->running = false;
  if (thread_handle_join(processor->thread, NULL) != 0) {
    log_error(PROCESSOR_LOGGER_ID, "Shutting down processor thread failed\n");
    ret = RC_FAILED_THREAD_JOIN;
  }
  return ret;
}

retcode_t processor_destroy(processor_state_t *const processor) {
  bool ret = RC_OK;

  if (processor == NULL) {
    return RC_NULL_PARAM;
  } else if (processor->running) {
    return RC_PROCESSOR_STILL_RUNNING;
  }

  log_debug(PROCESSOR_LOGGER_ID, "Destroying processor queue\n");
  if (CQ_DESTROY(iota_packet_t, processor->queue) != CQ_SUCCESS) {
    log_error(PROCESSOR_LOGGER_ID, "Destroying processor queue failed\n");
    ret = RC_PROCESSOR_FAILED_DESTROY_QUEUE;
  }
  return ret;
}
