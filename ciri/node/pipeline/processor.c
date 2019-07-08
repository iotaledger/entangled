/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <string.h>

#include "ciri/consensus/milestone/milestone_tracker.h"
#include "ciri/consensus/transaction_solidifier/transaction_solidifier.h"
#include "ciri/node/network/neighbor.h"
#include "ciri/node/node.h"
#include "ciri/node/pipeline/processor.h"
#include "common/crypto/curl-p/ptrit.h"
#include "common/trinary/trit_ptrit.h"
#include "utils/logger_helper.h"

#define PROCESSOR_LOGGER_ID "processor"
#define PROCESSOR_TIMEOUT_MS 1000ULL

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
 * @param hash The transaction hash
 *
 * @return a status code
 */
static retcode_t process_transaction_bytes(processor_stage_t const *const processor, tangle_t *const tangle,
                                           neighbor_t *const neighbor, protocol_gossip_t const *const packet,
                                           flex_trit_t const *const hash) {
  retcode_t ret = RC_OK;
  bool exists = false;
  iota_transaction_t transaction;
  flex_trit_t transaction_flex_trits[FLEX_TRIT_SIZE_8019];

  if (processor == NULL || packet == NULL || hash == NULL) {
    return RC_NULL_PARAM;
  }

  transaction_reset(&transaction);
  // Retreives the transaction from the packet
  if (flex_trits_from_bytes(transaction_flex_trits, NUM_TRITS_SERIALIZED_TRANSACTION, packet->content,
                            NUM_TRITS_SERIALIZED_TRANSACTION,
                            NUM_TRITS_SERIALIZED_TRANSACTION) != NUM_TRITS_SERIALIZED_TRANSACTION) {
    log_warning(logger_id, "Invalid transaction bytes\n");
    ret = RC_PROCESSOR_INVALID_TRANSACTION;
    goto failure;
  }

  // Deserializes the transaction
  if (transaction_deserialize_from_trits(&transaction, transaction_flex_trits, false) !=
      NUM_TRITS_SERIALIZED_TRANSACTION) {
    log_warning(logger_id, "Deserializing transaction failed\n");
    ret = RC_PROCESSOR_INVALID_TRANSACTION;
    goto failure;
  }
  transaction_set_hash(&transaction, hash);

  // Validates the transaction
  if (!iota_consensus_transaction_validate(processor->transaction_validator, &transaction)) {
    log_debug(logger_id, "Invalid transaction\n");
    goto failure;
  }

  // Checks if the transaction is already persisted
  if ((ret = iota_tangle_transaction_exist(tangle, TRANSACTION_FIELD_HASH, hash, &exists)) != RC_OK) {
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
    if ((ret = iota_consensus_transaction_solidifier_update_status(processor->transaction_solidifier, tangle,
                                                                   &transaction)) != RC_OK) {
      log_warning(logger_id, "Updating transaction status failed\n");
      return ret;
    }

    // TODO Store transaction metadata

    // Broadcast the new transaction
    if ((ret = broadcaster_stage_add(&processor->node->broadcaster, packet)) != RC_OK) {
      log_warning(logger_id, "Propagating packet to broadcaster failed\n");
      goto failure;
    }

    if (transaction_current_index(&transaction) == 0 &&
        memcmp(transaction_address(&transaction), processor->milestone_tracker->conf->coordinator_address,
               FLEX_TRIT_SIZE_243) == 0) {
      ret = iota_milestone_tracker_add_candidate(processor->milestone_tracker, transaction_hash(&transaction));
    }

    if (neighbor) {
      neighbor->nbr_new_txs++;
    }
  }

  return ret;

failure:
  if (neighbor) {
    neighbor->nbr_invalid_txs++;
  }

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
static retcode_t process_request_bytes(processor_stage_t const *const processor, neighbor_t *const neighbor,
                                       protocol_gossip_t const *const packet, flex_trit_t const *const hash) {
  retcode_t ret = RC_OK;
  flex_trit_t request_hash[FLEX_TRIT_SIZE_243];

  if (neighbor == NULL) {
    return RC_OK;
  }

  if (processor == NULL || packet == NULL || hash == NULL) {
    return RC_NULL_PARAM;
  }

  // Retreives the request hash from the packet
  if (flex_trits_from_bytes(request_hash, HASH_LENGTH_TRIT, packet->content + GOSSIP_TX_BYTES_LENGTH,
                            HASH_LENGTH_TRIT - processor->node->conf.mwm,
                            HASH_LENGTH_TRIT - processor->node->conf.mwm) !=
      HASH_LENGTH_TRIT - processor->node->conf.mwm) {
    log_warning(logger_id, "Invalid request bytes\n");
    return RC_PROCESSOR_INVALID_REQUEST;
  }

  // If requested hash is equal to transaction hash, sets the request hash to
  // null to request a random tip
  if (memcmp(request_hash, hash, FLEX_TRIT_SIZE_243) == 0) {
    memset(request_hash, FLEX_TRIT_NULL_VALUE, FLEX_TRIT_SIZE_243);
  }

  // Adds request to the responder stage queue
  if ((ret = responder_stage_add(&processor->node->responder, neighbor, request_hash)) != RC_OK) {
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
static retcode_t process_packet(processor_stage_t const *const processor, tangle_t *const tangle,
                                protocol_gossip_t const *const packet, flex_trit_t const *const hash, bool const cached,
                                uint64_t const digest) {
  retcode_t ret = RC_OK;
  neighbor_t *neighbor = NULL;

  if (processor == NULL || packet == NULL) {
    return RC_NULL_PARAM;
  }

  rw_lock_handle_rdlock(&processor->node->router.neighbors_lock);

  neighbor = neighbors_find_by_endpoint(&processor->node->router, &packet->source);

  if (neighbor || (packet->source.ip[0] == 0 && packet->source.port == 0)) {
    if (neighbor) {
      log_debug(logger_id, "Processing packet from tethered node tcp://%s:%d\n", neighbor->endpoint.host,
                neighbor->endpoint.port);
      neighbor->nbr_all_txs++;
    } else {
      log_debug(logger_id, "Processing packet from API\n");
    }

    if (!cached) {
      log_debug(logger_id, "Processing transaction bytes\n");
      if ((ret = process_transaction_bytes(processor, tangle, neighbor, packet, hash)) != RC_OK) {
        log_warning(logger_id, "Processing transaction bytes failed\n");
        goto done;
      }
      recent_seen_bytes_cache_put(&processor->node->recent_seen_bytes, digest, hash);
    }

    if (neighbor) {
      log_debug(logger_id, "Processing request bytes\n");
      if ((ret = process_request_bytes(processor, neighbor, packet, hash)) != RC_OK) {
        log_warning(logger_id, "Processing request bytes failed\n");
        goto done;
      }
    }
  } else {
    log_debug(logger_id, "Discarding packet from non-tethered node tcp://%s:%d\n", packet->source.ip,
              packet->source.port);
    // TODO Testnet add non-tethered neighbor
  }

done:
  rw_lock_handle_unlock(&processor->node->router.neighbors_lock);
  return ret;
}

typedef struct packet_digest_s {
  protocol_gossip_queue_entry_t *entry;
  uint64_t digest;
} packet_digest_t;

/**
 * Continuously looks for a packet from a processor packet queue and process it.
 *
 * @param processor The processor state
 */
static void *processor_stage_routine(processor_stage_t *const processor) {
  tangle_t tangle;
  size_t j;

  if (processor == NULL) {
    return NULL;
  }

  {
    connection_config_t db_conf = {.db_path = processor->node->conf.tangle_db_path};

    if (iota_tangle_init(&tangle, &db_conf) != RC_OK) {
      log_critical(logger_id, "Initializing tangle connection failed\n");
      return NULL;
    }
  }

  const size_t PACKET_MAX = 64;
  size_t packet_cnt = 0;
  protocol_gossip_queue_entry_t *entry = NULL;
  packet_digest_t *packets = (packet_digest_t *)calloc(PACKET_MAX, sizeof(packet_digest_t));

  trit_t *tx = (trit_t *)calloc(NUM_TRITS_SERIALIZED_TRANSACTION, sizeof(trit_t));
  trit_t *hash = (trit_t *)calloc(HASH_LENGTH_TRIT, sizeof(trit_t));

  PCurl *curl = (PCurl *)calloc(1, sizeof(PCurl));
  curl->type = 81;

  ptrit_t *txs_acc = (ptrit_t *)calloc(NUM_TRITS_SERIALIZED_TRANSACTION, sizeof(ptrit_t));

  flex_trit_t flex_hash[FLEX_TRIT_SIZE_243];
  uint64_t digest = 0;
  bool digest_found = false;

  lock_handle_t lock_cond;
  lock_handle_init(&lock_cond);
  lock_handle_lock(&lock_cond);

  while (processor->running) {
    packet_cnt = 0;
    while (packet_cnt < PACKET_MAX) {
      rw_lock_handle_wrlock(&processor->lock);
      entry = protocol_gossip_queue_pop(&processor->queue);
      rw_lock_handle_unlock(&processor->lock);
      if (entry == NULL) {
        break;
      }
      recent_seen_bytes_cache_hash(entry->packet.content, &digest);
      recent_seen_bytes_cache_get(&processor->node->recent_seen_bytes, digest, flex_hash, &digest_found);
      if (digest_found) {
        if (process_packet(processor, &tangle, &entry->packet, flex_hash, true, digest) != RC_OK) {
          log_warning(logger_id, "Processing packet failed\n");
        }
        free(entry);
      } else {
        packets[packet_cnt].entry = entry;
        packets[packet_cnt].digest = digest;
        packet_cnt++;
      }
    }

    if (packet_cnt == 0) {
      cond_handle_timedwait(&processor->cond, &lock_cond, PROCESSOR_TIMEOUT_MS);
      continue;
    }

    ptrit_curl_init(curl, CURL_P_81);
    memset(txs_acc, 0, NUM_TRITS_SERIALIZED_TRANSACTION * sizeof(ptrit_t));
    memset(flex_hash, FLEX_TRIT_NULL_VALUE, sizeof(flex_hash));

    for (j = 0; j < packet_cnt; j++) {
      bytes_to_trits(packets[j].entry->packet.content, GOSSIP_TX_BYTES_LENGTH, tx, NUM_TRITS_SERIALIZED_TRANSACTION);
      trits_to_ptrits(tx, txs_acc, j, NUM_TRITS_SERIALIZED_TRANSACTION);
    }

    ptrit_curl_absorb(curl, txs_acc, NUM_TRITS_SERIALIZED_TRANSACTION);
    ptrit_curl_squeeze(curl, txs_acc, HASH_LENGTH_TRIT);

    for (j = 0; j < packet_cnt; j++) {
      ptrits_to_trits(txs_acc, hash, j, HASH_LENGTH_TRIT);
      flex_trits_from_trits(flex_hash, HASH_LENGTH_TRIT, hash, HASH_LENGTH_TRIT, HASH_LENGTH_TRIT);

      if (process_packet(processor, &tangle, &packets[j].entry->packet, flex_hash, false, packets[j].digest) != RC_OK) {
        log_warning(logger_id, "Processing packet failed\n");
      }
      free(packets[j].entry);
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

retcode_t processor_stage_init(processor_stage_t *const processor, node_t *const node,
                               transaction_validator_t *const transaction_validator,
                               transaction_solidifier_t *const transaction_solidifier,
                               milestone_tracker_t *const milestone_tracker) {
  if (processor == NULL || node == NULL || transaction_validator == NULL || transaction_solidifier == NULL ||
      milestone_tracker == NULL) {
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

retcode_t processor_stage_start(processor_stage_t *const processor) {
  if (processor == NULL) {
    return RC_NULL_PARAM;
  }

  log_info(logger_id, "Spawning processor stage thread\n");
  processor->running = true;
  if (thread_handle_create(&processor->thread, (thread_routine_t)processor_stage_routine, processor) != 0) {
    log_critical(logger_id, "Spawning processor stage thread failed\n");
    return RC_THREAD_CREATE;
  }

  return RC_OK;
}

retcode_t processor_stage_stop(processor_stage_t *const processor) {
  if (processor == NULL) {
    return RC_NULL_PARAM;
  } else if (processor->running == false) {
    return RC_OK;
  }

  log_info(logger_id, "Shutting down processor stage thread\n");
  processor->running = false;
  cond_handle_signal(&processor->cond);
  if (thread_handle_join(processor->thread, NULL) != 0) {
    log_error(logger_id, "Shutting down processor stage thread failed\n");
    return RC_THREAD_JOIN;
  }

  return RC_OK;
}

retcode_t processor_stage_destroy(processor_stage_t *const processor) {
  if (processor == NULL) {
    return RC_NULL_PARAM;
  } else if (processor->running) {
    return RC_STILL_RUNNING;
  }

  protocol_gossip_queue_free(&processor->queue);
  rw_lock_handle_destroy(&processor->lock);
  cond_handle_destroy(&processor->cond);
  processor->node = NULL;
  processor->transaction_validator = NULL;
  processor->transaction_solidifier = NULL;
  processor->milestone_tracker = NULL;

  logger_helper_release(logger_id);

  return RC_OK;
}

retcode_t processor_stage_add(processor_stage_t *const processor, protocol_gossip_t const *const packet) {
  retcode_t ret = RC_OK;

  if (processor == NULL) {
    return RC_NULL_PARAM;
  }

  rw_lock_handle_wrlock(&processor->lock);
  ret = protocol_gossip_queue_push(&processor->queue, packet);
  rw_lock_handle_unlock(&processor->lock);

  if (ret != RC_OK) {
    log_warning(logger_id, "Pushing packet to processor stage queue failed\n");
    return ret;
  } else {
    cond_handle_signal(&processor->cond);
  }

  return RC_OK;
}

size_t processor_stage_size(processor_stage_t *const processor) {
  size_t size = 0;

  if (processor == NULL) {
    return 0;
  }

  rw_lock_handle_rdlock(&processor->lock);
  size = protocol_gossip_queue_count(processor->queue);
  rw_lock_handle_unlock(&processor->lock);

  return size;
}
