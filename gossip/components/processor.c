/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <string.h>

#include "gossip/components/processor.h"
#include "gossip/node.h"
#include "utils/containers/lists/concurrent_list_neighbor.h"
#include "utils/logger_helper.h"

#define PROCESSOR_LOGGER_ID "processor"
#define PROCESSOR_TIMEOUT_SEC 1

/*
 * Private functions
 */

/**
 * Converts transaction bytes from a packet to a transaction, validates it and
 * updates its status.
 * If valid and new: stores and broadcasts it.
 *
 * @param processor The processor state
 * @param neighbor The neighbor that sent the packet
 * @param packet The packet from which to process transaction bytes
 * @param hash A hash to be filled with transaction bytes hash
 *
 * @return a status code
 */
static retcode_t process_transaction_bytes(processor_t const *const processor,
                                           neighbor_t *const neighbor,
                                           iota_packet_t const *const packet,
                                           flex_trit_t *const hash) {
  retcode_t ret = RC_OK;
  bool exists = false;
  struct _iota_transaction transaction = {.snapshot_index = 0, .solid = 0};
  flex_trit_t transaction_flex_trits[FLEX_TRIT_SIZE_8019];
  trit_array_t key = {.trits = hash,
                      .num_trits = HASH_LENGTH_TRIT,
                      .num_bytes = FLEX_TRIT_SIZE_243,
                      .dynamic = 0};

  if (processor == NULL || neighbor == NULL || packet == NULL || hash == NULL) {
    return RC_NULL_PARAM;
  }

  // TODO Check if transaction hash is cached

  // Retreives the transaction from the packet
  if (flex_trits_from_bytes(transaction_flex_trits,
                            NUM_TRITS_SERIALIZED_TRANSACTION, packet->content,
                            NUM_TRITS_SERIALIZED_TRANSACTION,
                            NUM_TRITS_SERIALIZED_TRANSACTION) !=
      NUM_TRITS_SERIALIZED_TRANSACTION) {
    log_warning(PROCESSOR_LOGGER_ID, "Invalid transaction bytes\n");
    ret = RC_PROCESSOR_INVALID_TRANSACTION;
    goto failure;
  }

  // Deserializes the transaction
  if (transaction_deserialize_from_trits(&transaction,
                                         transaction_flex_trits) !=
      NUM_TRITS_SERIALIZED_TRANSACTION) {
    log_warning(PROCESSOR_LOGGER_ID, "Deserializing transaction failed\n");
    ret = RC_PROCESSOR_INVALID_TRANSACTION;
    goto failure;
  }

  // Validates the transaction
  if (!iota_consensus_transaction_validate(processor->transaction_validator,
                                           &transaction)) {
    log_warning(PROCESSOR_LOGGER_ID, "Invalid transaction\n");
    ret = RC_PROCESSOR_INVALID_TRANSACTION;
    goto failure;
  }

  // TODO Add transaction hash to cache

  memcpy(hash, transaction.hash, FLEX_TRIT_SIZE_243);

  // Checks if the transaction is already persisted
  if ((ret = iota_tangle_transaction_exist(
           processor->tangle, TRANSACTION_FIELD_HASH, &key, &exists)) !=
      RC_OK) {
    log_warning(PROCESSOR_LOGGER_ID, "Checking if transaction exists failed\n");
    goto failure;
  }

  if (!exists) {
    // Stores the new transaction
    log_debug(PROCESSOR_LOGGER_ID, "Storing new transaction\n");
    if ((ret = iota_tangle_transaction_store(processor->tangle,
                                             &transaction)) != RC_OK) {
      log_warning(PROCESSOR_LOGGER_ID, "Storing new transaction failed\n");
      goto failure;
    }

    // Updates transaction status
    if ((ret = iota_consensus_transaction_solidifier_update_status(
             processor->transaction_solidifier, &transaction)) != RC_OK) {
      log_warning(PROCESSOR_LOGGER_ID, "Updating transaction status failed\n");
      return ret;
    }

    // TODO Store transaction metadata

    // Broadcast the new transaction
    log_debug(PROCESSOR_LOGGER_ID, "Propagating packet to broadcaster\n");
    if ((ret = broadcaster_on_next(&processor->node->broadcaster,
                                   transaction_flex_trits)) != RC_OK) {
      log_warning(PROCESSOR_LOGGER_ID,
                  "Propagating packet to broadcaster failed\n");
      goto failure;
    }

    neighbor->nbr_new_tx++;
  }

  return ret;

failure:
  neighbor->nbr_invalid_tx++;
  return ret;
}

/**
 * Converts request bytes from a packet to a hash and adds it to the responder
 * queue.
 *
 * @param processor The processor state
 * @param neighbor The neighbor that sent the packet
 * @param packet The packet from which to process request bytes
 * @param hash Transaction bytes hash
 *
 * @return a status code
 */
static retcode_t process_request_bytes(processor_t const *const processor,
                                       neighbor_t *const neighbor,
                                       iota_packet_t const *const packet,
                                       flex_trit_t *const hash) {
  retcode_t ret = RC_OK;
  flex_trit_t request_hash[FLEX_TRIT_SIZE_243];

  if (processor == NULL || neighbor == NULL || packet == NULL || hash == NULL) {
    return RC_NULL_PARAM;
  }

  // Retreives the request hash from the packet
  if (flex_trits_from_bytes(request_hash, HASH_LENGTH_TRIT,
                            packet->content + PACKET_TX_SIZE,
                            processor->node->conf.request_hash_size_trit,
                            processor->node->conf.request_hash_size_trit) !=
      processor->node->conf.request_hash_size_trit) {
    log_warning(PROCESSOR_LOGGER_ID, "Invalid request bytes\n");
    return RC_PROCESSOR_INVALID_REQUEST;
  }

  // If requested hash is equal to transaction hash, sets the request hash to
  // null to request a random tip
  if (memcmp(request_hash, hash, FLEX_TRIT_SIZE_243) == 0) {
    memset(request_hash, FLEX_TRIT_NULL_VALUE, FLEX_TRIT_SIZE_243);
  }

  // Adds request to the responder queue
  log_debug(PROCESSOR_LOGGER_ID, "Propagating request to responder\n");
  if ((ret = responder_on_next(&processor->node->responder, neighbor,
                               request_hash)) != RC_OK) {
    log_warning(PROCESSOR_LOGGER_ID,
                "Propagating request to responder failed\n");
    return ret;
  }

  return RC_OK;
}

/**
 * Processes a packet
 *
 * @param processor The processor state
 * @param packet The packet
 *
 * @return a status code
 */
static retcode_t process_packet(processor_t const *const processor,
                                iota_packet_t const *const packet) {
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
  } else {
    // TODO Testnet add non-tethered neighbor
  }

  return RC_OK;
}

/**
 * Continuously looks for a packet from a processor packet queue and process it.
 *
 * @param processor The processor state
 */
static void *processor_routine(processor_t *const processor) {
  iota_packet_t *packet_ptr = NULL;
  iota_packet_t packet;

  if (processor == NULL) {
    return NULL;
  }

  lock_handle_t lock_cond;
  lock_handle_init(&lock_cond);
  lock_handle_lock(&lock_cond);

  while (processor->running) {
    if (processor_size(processor) == 0) {
      cond_handle_timedwait(&processor->cond, &lock_cond,
                            PROCESSOR_TIMEOUT_SEC);
    }

    rw_lock_handle_wrlock(&processor->lock);
    packet_ptr = iota_packet_queue_peek(processor->queue);
    if (packet_ptr == NULL) {
      rw_lock_handle_unlock(&processor->lock);
      continue;
    }
    packet = *packet_ptr;
    iota_packet_queue_pop(&processor->queue);
    rw_lock_handle_unlock(&processor->lock);

    log_debug(PROCESSOR_LOGGER_ID, "Processing packet\n");
    if (process_packet(processor, &packet) != RC_OK) {
      log_warning(PROCESSOR_LOGGER_ID, "Processing packet failed\n");
    }
  }

  lock_handle_unlock(&lock_cond);
  lock_handle_destroy(&lock_cond);

  return NULL;
}

/*
 * Public functions
 */

retcode_t processor_init(
    processor_t *const processor, node_t *const node, tangle_t *const tangle,
    transaction_validator_t *const transaction_validator,
    transaction_solidifier_t *const transaction_solidifier) {
  if (processor == NULL || node == NULL || tangle == NULL ||
      transaction_validator == NULL || transaction_solidifier == NULL) {
    return RC_NULL_PARAM;
  }

  logger_helper_init(PROCESSOR_LOGGER_ID, LOGGER_DEBUG, true);

  processor->running = false;
  processor->queue = NULL;
  rw_lock_handle_init(&processor->lock);
  cond_handle_init(&processor->cond);
  processor->node = node;
  processor->tangle = tangle;
  processor->transaction_validator = transaction_validator;
  processor->transaction_solidifier = transaction_solidifier;

  return RC_OK;
}

retcode_t processor_start(processor_t *const processor) {
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

retcode_t processor_stop(processor_t *const processor) {
  if (processor == NULL) {
    return RC_NULL_PARAM;
  } else if (processor->running == false) {
    return RC_OK;
  }

  log_info(PROCESSOR_LOGGER_ID, "Shutting down processor thread\n");
  processor->running = false;
  if (thread_handle_join(processor->thread, NULL) != 0) {
    log_error(PROCESSOR_LOGGER_ID, "Shutting down processor thread failed\n");
    return RC_FAILED_THREAD_JOIN;
  }

  return RC_OK;
}

retcode_t processor_destroy(processor_t *const processor) {
  if (processor == NULL) {
    return RC_NULL_PARAM;
  } else if (processor->running) {
    return RC_STILL_RUNNING;
  }

  iota_packet_queue_free(&processor->queue);
  rw_lock_handle_destroy(&processor->lock);
  cond_handle_destroy(&processor->cond);
  processor->node = NULL;
  processor->tangle = NULL;
  processor->transaction_validator = NULL;
  processor->transaction_solidifier = NULL;

  logger_helper_destroy(PROCESSOR_LOGGER_ID);

  return RC_OK;
}

retcode_t processor_on_next(processor_t *const processor,
                            iota_packet_t const packet) {
  retcode_t ret = RC_OK;

  if (processor == NULL) {
    return RC_NULL_PARAM;
  }

  rw_lock_handle_wrlock(&processor->lock);
  ret = iota_packet_queue_push(&processor->queue, &packet);
  rw_lock_handle_unlock(&processor->lock);

  if (ret != RC_OK) {
    log_warning(PROCESSOR_LOGGER_ID,
                "Pushing packet to processor queue failed\n");
    return ret;
  } else {
    cond_handle_signal(&processor->cond);
  }

  return RC_OK;
}

size_t processor_size(processor_t *const processor) {
  size_t size = 0;

  if (processor == NULL) {
    return 0;
  }

  rw_lock_handle_rdlock(&processor->lock);
  size = iota_packet_queue_count(processor->queue);
  rw_lock_handle_unlock(&processor->lock);

  return size;
}
