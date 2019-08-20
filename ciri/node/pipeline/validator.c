/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "ciri/node/pipeline/validator.h"
#include "ciri/consensus/milestone/milestone_tracker.h"
#include "ciri/consensus/tangle/tangle.h"
#include "ciri/consensus/transaction_solidifier/transaction_solidifier.h"
#include "ciri/consensus/transaction_validator/transaction_validator.h"
#include "ciri/node/node.h"
#include "utils/logger_helper.h"

#define VALIDATOR_LOGGER_ID "validator"

static logger_id_t logger_id;

/*
 * Private functions
 */

/**
 * Converts transaction bytes from a packet to a transaction, validates it and
 * updates its status.
 * If valid and new: stores and broadcasts it.
 *
 * @param validator The validator stage
 * @param tangle A tangle
 * @param neighbor The neighbor that sent the packet
 * @param packet The packet from which to process transaction bytes
 * @param hash The transaction hash
 *
 * @return a status code
 */
static retcode_t validate_transaction_bytes(validator_stage_t const *const validator, tangle_t *const tangle,
                                            neighbor_t *const neighbor, protocol_gossip_t const *const gossip,
                                            flex_trit_t const *const hash) {
  retcode_t ret = RC_OK;
  bool exists = false;
  iota_transaction_t transaction;
  flex_trit_t transaction_flex_trits[FLEX_TRIT_SIZE_8019];

  if (validator == NULL || gossip == NULL || hash == NULL) {
    return RC_NULL_PARAM;
  }

  transaction_reset(&transaction);
  // Retreives the transaction from the packet
  if (flex_trits_from_bytes(transaction_flex_trits, NUM_TRITS_SERIALIZED_TRANSACTION, gossip->content,
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
  if (!iota_consensus_transaction_validate(validator->transaction_validator, &transaction)) {
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
    if ((ret = iota_consensus_transaction_solidifier_update_status(validator->transaction_solidifier, tangle,
                                                                   &transaction)) != RC_OK) {
      log_warning(logger_id, "Updating transaction status failed\n");
      return ret;
    }

    // TODO Store transaction metadata

    // Broadcast the new transaction
    if ((ret = broadcaster_stage_add(&validator->node->broadcaster, gossip)) != RC_OK) {
      log_warning(logger_id, "Propagating packet to broadcaster failed\n");
      goto failure;
    }

    if (transaction_current_index(&transaction) == 0 &&
        memcmp(transaction_address(&transaction), validator->milestone_tracker->conf->coordinator_address,
               FLEX_TRIT_SIZE_243) == 0) {
      ret = iota_milestone_tracker_add_candidate(validator->milestone_tracker, transaction_hash(&transaction));
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

static void *validator_stage_routine(validator_stage_t *const validator) {
  validator_payload_queue_entry_t *entry = NULL;
  lock_handle_t lock_cond;
  tangle_t tangle;

  if (validator == NULL) {
    return NULL;
  }

  {
    storage_connection_config_t db_conf = {.db_path = validator->node->conf.tangle_db_path};

    if (iota_tangle_init(&tangle, &db_conf) != RC_OK) {
      log_critical(logger_id, "Initializing tangle connection failed\n");
      return NULL;
    }
  }

  lock_handle_init(&lock_cond);
  lock_handle_lock(&lock_cond);

  while (validator->running) {
    lock_handle_lock(&validator->lock);
    entry = validator_payload_queue_pop(&validator->queue);
    lock_handle_unlock(&validator->lock);

    if (entry == NULL) {
      cond_handle_wait(&validator->cond, &lock_cond);
      continue;
    }

    if (validate_transaction_bytes(validator, &tangle, entry->payload.neighbor, &entry->payload.gossip->packet,
                                   entry->payload.hash) != RC_OK) {
      log_warning(logger_id, "Processing packet failed\n");
    }
    recent_seen_bytes_cache_put(&validator->node->recent_seen_bytes, entry->payload.digest, entry->payload.hash);
    if (responder_process_request(&validator->node->responder, entry->payload.neighbor, &entry->payload.gossip->packet,
                                  entry->payload.hash) != RC_OK) {
      log_warning(logger_id, "Processing request bytes failed\n");
    }

    free(entry->payload.gossip);
    free(entry);
  }

  lock_handle_unlock(&lock_cond);
  lock_handle_destroy(&lock_cond);

  if (iota_tangle_destroy(&tangle) != RC_OK) {
    log_critical(logger_id, "Destroying tangle connection failed\n");
  }

  return NULL;
}

/*
 * Public functions
 */

retcode_t validator_stage_init(validator_stage_t *const validator, node_t *const node,
                               transaction_validator_t *const transaction_validator,
                               transaction_solidifier_t *const transaction_solidifier,
                               milestone_tracker_t *const milestone_tracker) {
  if (validator == NULL || node == NULL || transaction_validator == NULL || transaction_solidifier == NULL ||
      milestone_tracker == NULL) {
    return RC_NULL_PARAM;
  }

  logger_id = logger_helper_enable(VALIDATOR_LOGGER_ID, LOGGER_DEBUG, true);

  validator->running = false;
  validator->queue = NULL;
  lock_handle_init(&validator->lock);
  cond_handle_init(&validator->cond);
  validator->node = node;
  validator->transaction_validator = transaction_validator;
  validator->transaction_solidifier = transaction_solidifier;
  validator->milestone_tracker = milestone_tracker;

  return RC_OK;
}

retcode_t validator_stage_start(validator_stage_t *const validator) {
  if (validator == NULL) {
    return RC_NULL_PARAM;
  }

  log_info(logger_id, "Spawning validator stage thread\n");
  validator->running = true;
  if (thread_handle_create(&validator->thread, (thread_routine_t)validator_stage_routine, validator) != 0) {
    log_critical(logger_id, "Spawning validator stage thread failed\n");
    return RC_THREAD_CREATE;
  }

  return RC_OK;
}

retcode_t validator_stage_stop(validator_stage_t *const validator) {
  if (validator == NULL) {
    return RC_NULL_PARAM;
  } else if (validator->running == false) {
    return RC_OK;
  }

  log_info(logger_id, "Shutting down validator stage thread\n");
  validator->running = false;
  cond_handle_signal(&validator->cond);
  if (thread_handle_join(validator->thread, NULL) != 0) {
    log_error(logger_id, "Shutting down validator stage thread failed\n");
    return RC_THREAD_JOIN;
  }

  return RC_OK;
}

retcode_t validator_stage_destroy(validator_stage_t *const validator) {
  if (validator == NULL) {
    return RC_NULL_PARAM;
  } else if (validator->running) {
    return RC_STILL_RUNNING;
  }

  validator_payload_queue_free(&validator->queue);
  lock_handle_destroy(&validator->lock);
  cond_handle_destroy(&validator->cond);
  validator->node = NULL;

  logger_helper_release(logger_id);

  return RC_OK;
}

retcode_t validator_stage_add(validator_stage_t *const validator, protocol_gossip_queue_entry_t *const gossip,
                              uint64_t const digest, neighbor_t *const neighbor, flex_trit_t const *const hash) {
  retcode_t ret = RC_OK;

  if (validator == NULL) {
    return RC_NULL_PARAM;
  }

  lock_handle_lock(&validator->lock);
  ret = validator_payload_queue_push(&validator->queue, gossip, digest, neighbor, hash);
  lock_handle_unlock(&validator->lock);

  if (ret != RC_OK) {
    log_warning(logger_id, "Pushing packet to validator stage queue failed\n");
    return ret;
  } else {
    cond_handle_signal(&validator->cond);
  }

  return RC_OK;
}

size_t validator_stage_size(validator_stage_t *const validator) {
  size_t size = 0;

  if (validator == NULL) {
    return 0;
  }

  lock_handle_lock(&validator->lock);
  size = validator_payload_queue_count(validator->queue);
  lock_handle_unlock(&validator->lock);

  return size;
}

size_t validator_payload_queue_count(validator_payload_queue_t const queue) {
  validator_payload_queue_entry_t *iter = NULL;
  size_t count = 0;

  CDL_COUNT(queue, iter, count);

  return count;
}

retcode_t validator_payload_queue_push(validator_payload_queue_t *const queue,
                                       protocol_gossip_queue_entry_t *const gossip, uint64_t const digest,
                                       neighbor_t *const neighbor, flex_trit_t const *const hash) {
  validator_payload_queue_entry_t *entry = NULL;

  if (queue == NULL || gossip == NULL) {
    return RC_NULL_PARAM;
  }

  if ((entry = (validator_payload_queue_entry_t *)malloc(sizeof(validator_payload_queue_entry_t))) == NULL) {
    return RC_OOM;
  }

  entry->payload.gossip = gossip;
  entry->payload.digest = digest;
  entry->payload.neighbor = neighbor;
  memcpy(entry->payload.hash, hash, FLEX_TRIT_SIZE_243);
  CDL_APPEND(*queue, entry);

  return RC_OK;
}

validator_payload_queue_entry_t *validator_payload_queue_pop(validator_payload_queue_t *const queue) {
  validator_payload_queue_entry_t *front = NULL;

  if (queue == NULL) {
    return NULL;
  }

  front = *queue;
  if (front != NULL) {
    CDL_DELETE(*queue, front);
  }

  return front;
}

void validator_payload_queue_free(validator_payload_queue_t *const queue) {
  validator_payload_queue_entry_t *iter = NULL, *tmp1 = NULL, *tmp2 = NULL;

  if (queue == NULL) {
    return;
  }

  CDL_FOREACH_SAFE(*queue, iter, tmp1, tmp2) {
    CDL_DELETE(*queue, iter);
    free(iter->payload.gossip);
    free(iter);
  }
  *queue = NULL;
}
