/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <string.h>

#include "common/curl-p/ptrit.h"
#include "common/trinary/trit_ptrit.h"
#include "consensus/milestone_tracker/milestone_tracker.h"
#include "consensus/transaction_solidifier/transaction_solidifier.h"
#include "gossip/components/processor.h"
#include "gossip/neighbor.h"
#include "gossip/node.h"
#include "utils/logger_helper.h"

#define PROCESSOR_LOGGER_ID "processor"
#define PROCESSOR_TIMEOUT_SEC 1

static logger_id_t logger_id;

/*
 * Private functions
 */

/**
 * Converts transaction bytes from a packet to a transaction, validates it and
 * updates its status.
 * If valid and new: stores and broadcasts it.
 *
 * @param processor The processor state
 * @param tangle A tangle
 * @param neighbor The neighbor that sent the packet
 * @param packet The packet from which to process transaction bytes
 * @param curl_hash The CurlP81 hash of the transaction
 *
 * @return a status code
 */
static retcode_t process_transaction_bytes(processor_t const *const processor,
                                           tangle_t *const tangle,
                                           neighbor_t *const neighbor,
                                           iota_packet_t const *const packet,
                                           flex_trit_t const *const curl_hash) {
  retcode_t ret = RC_OK;
  bool exists = false;
  iota_transaction_t transaction = {.metadata.snapshot_index = 0,
                                    .metadata.solid = 0,
                                    .loaded_columns_mask = 0};
  flex_trit_t transaction_flex_trits[FLEX_TRIT_SIZE_8019];

  if (processor == NULL || neighbor == NULL || packet == NULL ||
      curl_hash == NULL) {
    return RC_NULL_PARAM;
  }

  // TODO Check if transaction hash is cached

  // Retreives the transaction from the packet
  if (flex_trits_from_bytes(transaction_flex_trits,
                            NUM_TRITS_SERIALIZED_TRANSACTION, packet->content,
                            NUM_TRITS_SERIALIZED_TRANSACTION,
                            NUM_TRITS_SERIALIZED_TRANSACTION) !=
      NUM_TRITS_SERIALIZED_TRANSACTION) {
    log_warning(logger_id, "Invalid transaction bytes\n");
    ret = RC_PROCESSOR_INVALID_TRANSACTION;
    goto failure;
  }

  // Deserializes the transaction
  if (transaction_deserialize_from_trits(&transaction, transaction_flex_trits,
                                         false) !=
      NUM_TRITS_SERIALIZED_TRANSACTION) {
    log_warning(logger_id, "Deserializing transaction failed\n");
    ret = RC_PROCESSOR_INVALID_TRANSACTION;
    goto failure;
  }
  transaction_set_hash(&transaction, curl_hash);

  // Validates the transaction
  if (!iota_consensus_transaction_validate(processor->transaction_validator,
                                           &transaction)) {
    log_debug(logger_id, "Invalid transaction\n");
    goto failure;
  }

  // TODO Add transaction hash to cache

  // Checks if the transaction is already persisted
  if ((ret = iota_tangle_transaction_exist(tangle, TRANSACTION_FIELD_HASH,
                                           curl_hash, &exists)) != RC_OK) {
    log_warning(logger_id, "Checking if transaction exists failed\n");
    goto failure;
  }

  if (!exists) {
    // Stores the new transaction
    log_debug(logger_id, "Storing new transaction\n");
    if ((ret = iota_tangle_transaction_store(tangle, &transaction)) != RC_OK) {
      log_warning(logger_id, "Storing new transaction failed\n");
      goto failure;
    }

    // Updates transaction status
    if ((ret = iota_consensus_transaction_solidifier_update_status(
             processor->transaction_solidifier, tangle, &transaction)) !=
        RC_OK) {
      log_warning(logger_id, "Updating transaction status failed\n");
      return ret;
    }

    // TODO Store transaction metadata

    // Broadcast the new transaction
    if ((ret = broadcaster_on_next(&processor->node->broadcaster,
                                   transaction_flex_trits)) != RC_OK) {
      log_warning(logger_id, "Propagating packet to broadcaster failed\n");
      goto failure;
    }

    if (transaction_current_index(&transaction) == 0 &&
        memcmp(transaction_address(&transaction),
               processor->milestone_tracker->coordinator,
               FLEX_TRIT_SIZE_243) == 0) {
      ret = iota_milestone_tracker_add_candidate(
          processor->milestone_tracker, transaction_hash(&transaction));
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
                                       flex_trit_t const *const hash) {
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
    log_warning(logger_id, "Invalid request bytes\n");
    return RC_PROCESSOR_INVALID_REQUEST;
  }

  // If requested hash is equal to transaction hash, sets the request hash to
  // null to request a random tip
  if (memcmp(request_hash, hash, FLEX_TRIT_SIZE_243) == 0) {
    memset(request_hash, FLEX_TRIT_NULL_VALUE, FLEX_TRIT_SIZE_243);
  }

  // Adds request to the responder queue
  if ((ret = responder_on_next(&processor->node->responder, neighbor,
                               request_hash)) != RC_OK) {
    log_warning(logger_id, "Propagating request to responder failed\n");
    return ret;
  }

  return RC_OK;
}

/**
 * Processes a packet
 *
 * @param processor The processor state
 * @param tangle A tangle
 * @param packet The packet
 *
 * @return a status code
 */
static retcode_t process_packet(processor_t const *const processor,
                                tangle_t *const tangle,
                                iota_packet_t const *const packet,
                                flex_trit_t const *const hash) {
  retcode_t ret = RC_OK;
  neighbor_t *neighbor = NULL;
  char *protocol = NULL;

  if (processor == NULL || packet == NULL) {
    return RC_NULL_PARAM;
  }

  rw_lock_handle_rdlock(&processor->node->neighbors_lock);

  neighbor =
      neighbors_find_by_endpoint(processor->node->neighbors, &packet->source);
  protocol = packet->source.protocol == PROTOCOL_TCP ? "tcp" : "udp";

  if (neighbor) {
    log_debug(logger_id, "Processing packet from tethered node %s://%s:%d\n",
              protocol, neighbor->endpoint.host, neighbor->endpoint.port);
    neighbor->nbr_all_tx++;

    log_debug(logger_id, "Processing transaction bytes\n");
    if ((ret = process_transaction_bytes(processor, tangle, neighbor, packet,
                                         hash)) != RC_OK) {
      log_warning(logger_id, "Processing transaction bytes failed\n");
      goto done;
    }

    log_debug(logger_id, "Processing request bytes\n");
    if ((ret = process_request_bytes(processor, neighbor, packet, hash)) !=
        RC_OK) {
      log_warning(logger_id, "Processing request bytes failed\n");
      goto done;
    }
  } else {
    log_debug(logger_id,
              "Discarding packet from non-tethered node %s://%s:%d\n", protocol,
              packet->source.ip, packet->source.port);
    // TODO Testnet add non-tethered neighbor
  }

done:
  rw_lock_handle_unlock(&processor->node->neighbors_lock);
  return ret;
}

/**
 * Continuously looks for a packet from a processor packet queue and process it.
 *
 * @param processor The processor state
 */
static void *processor_routine(processor_t *const processor) {
  connection_config_t db_conf = {.db_path = processor->node->conf.db_path};
  tangle_t tangle;
  size_t j;

  if (processor == NULL) {
    return NULL;
  }

  if (iota_tangle_init(&tangle, &db_conf) != RC_OK) {
    log_critical(logger_id, "Initializing tangle connection failed\n");
    return NULL;
  }

  const size_t PACKET_MAX = 64;
  size_t packet_cnt = 0;
  iota_packet_t *packet_ptr = NULL;
  iota_packet_t *packets =
      (iota_packet_t *)calloc(PACKET_MAX, sizeof(iota_packet_t));

  trit_t *tx =
      (trit_t *)calloc(NUM_TRITS_SERIALIZED_TRANSACTION, sizeof(trit_t));
  trit_t *hash = (trit_t *)calloc(HASH_LENGTH_TRIT, sizeof(trit_t));

  PCurl *curl = (PCurl *)calloc(1, sizeof(PCurl));
  curl->type = 81;

  ptrit_t *txs_acc =
      (ptrit_t *)calloc(NUM_TRITS_SERIALIZED_TRANSACTION, sizeof(ptrit_t));

  flex_trit_t flex_hash[FLEX_TRIT_SIZE_243];

  lock_handle_t lock_cond;
  lock_handle_init(&lock_cond);
  lock_handle_lock(&lock_cond);

  while (processor->running) {
    if (processor_is_empty(processor)) {
      cond_handle_timedwait(&processor->cond, &lock_cond,
                            PROCESSOR_TIMEOUT_SEC);
    }

    rw_lock_handle_wrlock(&processor->lock);
    for (packet_cnt = 0; packet_cnt < PACKET_MAX; packet_cnt++) {
      packet_ptr = iota_packet_queue_peek(processor->queue);
      if (packet_ptr == NULL) {
        goto process_packets;
      }
      packets[packet_cnt] = *packet_ptr;
      iota_packet_queue_pop(&processor->queue);
    }

  process_packets:
    rw_lock_handle_unlock(&processor->lock);

    if (packet_cnt == 0) {
      continue;
    }

    ptrit_curl_init(curl, CURL_P_81);
    memset(txs_acc, 0, NUM_TRITS_SERIALIZED_TRANSACTION * sizeof(ptrit_t));
    memset(flex_hash, FLEX_TRIT_NULL_VALUE, sizeof(flex_hash));

    for (j = 0; j < packet_cnt; j++) {
      bytes_to_trits(packets[j].content, PACKET_TX_SIZE, tx,
                     NUM_TRITS_SERIALIZED_TRANSACTION);
      trits_to_ptrits(tx, txs_acc, j, NUM_TRITS_SERIALIZED_TRANSACTION);
    }

    ptrit_curl_absorb(curl, txs_acc, NUM_TRITS_SERIALIZED_TRANSACTION);
    ptrit_curl_squeeze(curl, txs_acc, HASH_LENGTH_TRIT);

    for (j = 0; j < packet_cnt; j++) {
      ptrits_to_trits(txs_acc, hash, j, HASH_LENGTH_TRIT);
      flex_trits_from_trits(flex_hash, HASH_LENGTH_TRIT, hash, HASH_LENGTH_TRIT,
                            HASH_LENGTH_TRIT);

      if (process_packet(processor, &tangle, &packets[j], flex_hash) != RC_OK) {
        log_warning(logger_id, "Processing packet failed\n");
      }
    }
  }

  lock_handle_unlock(&lock_cond);
  lock_handle_destroy(&lock_cond);

  if (iota_tangle_destroy(&tangle) != RC_OK) {
    log_critical(logger_id, "Destroying tangle connection failed\n");
  }

  free(curl);
  free(packets);
  free(tx);
  free(hash);
  free(txs_acc);

  return NULL;
}

/*
 * Public functions
 */

retcode_t processor_init(processor_t *const processor, node_t *const node,
                         transaction_validator_t *const transaction_validator,
                         transaction_solidifier_t *const transaction_solidifier,
                         milestone_tracker_t *const milestone_tracker) {
  if (processor == NULL || node == NULL || transaction_validator == NULL ||
      transaction_solidifier == NULL || milestone_tracker == NULL) {
    return RC_NULL_PARAM;
  }

  logger_id = logger_helper_enable(PROCESSOR_LOGGER_ID, LOGGER_DEBUG, true);

  processor->running = false;
  processor->queue = NULL;
  rw_lock_handle_init(&processor->lock);
  cond_handle_init(&processor->cond);
  processor->node = node;
  processor->transaction_validator = transaction_validator;
  processor->transaction_solidifier = transaction_solidifier;
  processor->milestone_tracker = milestone_tracker;

  return RC_OK;
}

retcode_t processor_start(processor_t *const processor) {
  if (processor == NULL) {
    return RC_NULL_PARAM;
  }

  log_info(logger_id, "Spawning processor thread\n");
  processor->running = true;
  if (thread_handle_create(&processor->thread,
                           (thread_routine_t)processor_routine,
                           processor) != 0) {
    log_critical(logger_id, "Spawning processor thread failed\n");
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

  log_info(logger_id, "Shutting down processor thread\n");
  processor->running = false;
  if (thread_handle_join(processor->thread, NULL) != 0) {
    log_error(logger_id, "Shutting down processor thread failed\n");
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
  processor->transaction_validator = NULL;
  processor->transaction_solidifier = NULL;
  processor->milestone_tracker = NULL;

  logger_helper_release(logger_id);

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
    log_warning(logger_id, "Pushing packet to processor queue failed\n");
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
