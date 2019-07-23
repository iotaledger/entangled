/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "ciri/node/pipeline/processor.h"
#include "ciri/node/network/neighbor.h"
#include "ciri/node/node.h"
#include "utils/logger_helper.h"

#define PROCESSOR_LOGGER_ID "processor"

static logger_id_t logger_id;

/*
 * Private functions
 */

/**
 * Converts request bytes from a packet to a hash and adds it to the responder queue.
 *
 * @param processor The processor stage
 * @param neighbor The neighbor that sent the packet
 * @param packet The packet from which to process request bytes
 * @param hash Transaction hash
 *
 * @return a status code
 */
static retcode_t process_request_bytes(processor_stage_t const *const processor, neighbor_t *const neighbor,
                                       protocol_gossip_t const *const packet, flex_trit_t const *const hash) {
  retcode_t ret = RC_OK;
  // TODO memset ?
  flex_trit_t request_hash[FLEX_TRIT_SIZE_243];

  if (processor == NULL || neighbor == NULL || packet == NULL || hash == NULL) {
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

  // If requested hash is equal to transaction hash, sets the request hash to null to request a random tip
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
 * Continuously looks for a packet from a processor packet queue and process it.
 *
 * @param processor The processor stage
 */
static void *processor_stage_routine(processor_stage_t *const processor) {
  protocol_gossip_queue_entry_t *entry = NULL;
  neighbor_t *neighbor = NULL;
  flex_trit_t hash[FLEX_TRIT_SIZE_243];
  uint64_t digest = 0;
  bool cached = false;
  lock_handle_t lock_cond;

  if (processor == NULL) {
    return NULL;
  }

  lock_handle_init(&lock_cond);
  lock_handle_lock(&lock_cond);

  while (processor->running) {
    lock_handle_lock(&processor->lock);
    entry = protocol_gossip_queue_pop(&processor->queue);
    lock_handle_unlock(&processor->lock);

    if (entry == NULL) {
      cond_handle_wait(&processor->cond, &lock_cond);
      continue;
    }

    rw_lock_handle_rdlock(&processor->node->router.neighbors_lock);
    // TODO can do without ?
    neighbor = router_neighbor_find_by_endpoint(&processor->node->router, &entry->packet.source);
    // TODO ALL ?
    rw_lock_handle_unlock(&processor->node->router.neighbors_lock);

    if (neighbor) {
      log_debug(logger_id, "Processing packet from neighbor tcp://%s:%d\n", neighbor->endpoint.domain,
                neighbor->endpoint.port);
      neighbor->nbr_all_txs++;
    } else {
      log_debug(logger_id, "Processing packet from API\n");
    }

    recent_seen_bytes_cache_hash(entry->packet.content, &digest);
    recent_seen_bytes_cache_get(&processor->node->recent_seen_bytes, digest, hash, &cached);

    if (cached) {
      if (neighbor) {
        log_debug(logger_id, "Processing request bytes\n");
        if (process_request_bytes(processor, neighbor, &entry->packet, hash) != RC_OK) {
          log_warning(logger_id, "Processing request bytes failed\n");
        }
      }
      free(entry);
    } else {
      if (hasher_stage_add(&processor->node->hasher, entry, digest) != RC_OK) {
        log_warning(logger_id, "Sending payload to hasher stage failed\n");
      }
    }
  }

  lock_handle_unlock(&lock_cond);
  lock_handle_destroy(&lock_cond);

  return NULL;
}

/*
 * Public functions
 */

retcode_t processor_stage_init(processor_stage_t *const processor, node_t *const node) {
  if (processor == NULL || node == NULL) {
    return RC_NULL_PARAM;
  }

  logger_id = logger_helper_enable(PROCESSOR_LOGGER_ID, LOGGER_DEBUG, true);

  processor->running = false;
  processor->queue = NULL;
  lock_handle_init(&processor->lock);
  cond_handle_init(&processor->cond);
  processor->node = node;

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
  lock_handle_destroy(&processor->lock);
  cond_handle_destroy(&processor->cond);
  processor->node = NULL;

  logger_helper_release(logger_id);

  return RC_OK;
}

retcode_t processor_stage_add(processor_stage_t *const processor, protocol_gossip_t const *const packet) {
  retcode_t ret = RC_OK;

  if (processor == NULL) {
    return RC_NULL_PARAM;
  }

  lock_handle_lock(&processor->lock);
  ret = protocol_gossip_queue_push(&processor->queue, packet);
  lock_handle_unlock(&processor->lock);

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

  lock_handle_lock(&processor->lock);
  size = protocol_gossip_queue_count(processor->queue);
  lock_handle_unlock(&processor->lock);

  return size;
}
