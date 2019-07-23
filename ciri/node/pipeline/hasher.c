/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <string.h>

#include "ciri/node/pipeline/hasher.h"
#include "common/crypto/curl-p/ptrit.h"
#include "common/model/transaction.h"
#include "common/trinary/flex_trit.h"
#include "common/trinary/trit_byte.h"
#include "common/trinary/trit_ptrit.h"
#include "utils/logger_helper.h"

#define HASHER_LOGGER_ID "hasher"
#define HASHER_MAX 64

static logger_id_t logger_id;

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
// static retcode_t process_transaction_bytes(processor_stage_t const *const processor, tangle_t *const tangle,
//                                            neighbor_t *const neighbor, hasher_stage_payload_ const *const packet,
//                                            flex_trit_t const *const hash) {
//   retcode_t ret = RC_OK;
//   bool exists = false;
//   iota_transaction_t transaction;
//   flex_trit_t transaction_flex_trits[FLEX_TRIT_SIZE_8019];
//
//   if (processor == NULL || packet == NULL || hash == NULL) {
//     return RC_NULL_PARAM;
//   }
//
//   transaction_reset(&transaction);
//   // Retreives the transaction from the packet
//   if (flex_trits_from_bytes(transaction_flex_trits, NUM_TRITS_SERIALIZED_TRANSACTION, packet->content,
//                             NUM_TRITS_SERIALIZED_TRANSACTION,
//                             NUM_TRITS_SERIALIZED_TRANSACTION) != NUM_TRITS_SERIALIZED_TRANSACTION) {
//     log_warning(logger_id, "Invalid transaction bytes\n");
//     ret = RC_PROCESSOR_INVALID_TRANSACTION;
//     goto failure;
//   }
//
//   // Deserializes the transaction
//   if (transaction_deserialize_from_trits(&transaction, transaction_flex_trits, false) !=
//       NUM_TRITS_SERIALIZED_TRANSACTION) {
//     log_warning(logger_id, "Deserializing transaction failed\n");
//     ret = RC_PROCESSOR_INVALID_TRANSACTION;
//     goto failure;
//   }
//   transaction_set_hash(&transaction, hash);
//
//   // Validates the transaction
//   if (!iota_consensus_transaction_validate(processor->transaction_validator, &transaction)) {
//     log_debug(logger_id, "Invalid transaction\n");
//     goto failure;
//   }
//
//   // Checks if the transaction is already persisted
//   if ((ret = iota_tangle_transaction_exist(tangle, TRANSACTION_FIELD_HASH, hash, &exists)) != RC_OK) {
//     log_warning(logger_id, "Checking if transaction exists failed\n");
//     goto failure;
//   }
//
//   if (!exists) {
//     // Stores the new transaction
//     log_debug(logger_id, "Storing new transaction\n");
//     if ((ret = iota_tangle_transaction_store(tangle, &transaction)) != RC_OK) {
//       log_warning(logger_id, "Storing new transaction failed\n");
//       goto failure;
//     }
//
//     // Updates transaction status
//     if ((ret = iota_consensus_transaction_solidifier_update_status(processor->transaction_solidifier, tangle,
//                                                                    &transaction)) != RC_OK) {
//       log_warning(logger_id, "Updating transaction status failed\n");
//       return ret;
//     }
//
//     // TODO Store transaction metadata
//
//     // Broadcast the new transaction
//     if ((ret = broadcaster_stage_add(&processor->node->broadcaster, packet)) != RC_OK) {
//       log_warning(logger_id, "Propagating packet to broadcaster failed\n");
//       goto failure;
//     }
//
//     if (transaction_current_index(&transaction) == 0 &&
//         memcmp(transaction_address(&transaction), processor->milestone_tracker->conf->coordinator_address,
//                FLEX_TRIT_SIZE_243) == 0) {
//       ret = iota_milestone_tracker_add_candidate(processor->milestone_tracker, transaction_hash(&transaction));
//     }
//
//     if (neighbor) {
//       neighbor->nbr_new_txs++;
//     }
//   }
//
//   return ret;
//
// failure:
//   if (neighbor) {
//     neighbor->nbr_invalid_txs++;
//   }
//
//   return ret;
// }

// if (!cached) {
//   log_debug(logger_id, "Processing transaction bytes\n");
//   if ((ret = process_transaction_bytes(processor, tangle, neighbor, packet, hash)) != RC_OK) {
//     log_warning(logger_id, "Processing transaction bytes failed\n");
//     goto done;
//   }
//   recent_seen_bytes_cache_put(&processor->node->recent_seen_bytes, digest, hash);
// }

/**
 * Private functions
 */

static void *hasher_stage_routine(hasher_stage_t *const hasher) {
  hasher_payload_queue_entry_t *entry = NULL;
  lock_handle_t lock_cond;
  size_t packets_num = 0;
  trit_t tx[NUM_TRITS_SERIALIZED_TRANSACTION];
  ptrit_t acc[NUM_TRITS_SERIALIZED_TRANSACTION];
  trit_t hash[HASH_LENGTH_TRIT];
  flex_trit_t flex_hash[FLEX_TRIT_SIZE_243];
  PCurl curl;
  // packet_digest_t *packets = (packet_digest_t *)calloc(PACKET_MAX, sizeof(packet_digest_t));

  if (hasher == NULL) {
    return NULL;
  }

  lock_handle_init(&lock_cond);
  lock_handle_lock(&lock_cond);

  while (hasher->running) {
    packets_num = 0;

    while (hasher->running && packets_num < HASHER_MAX) {
      lock_handle_lock(&hasher->lock);
      entry = hasher_payload_queue_pop(&hasher->queue);
      lock_handle_unlock(&hasher->lock);

      if (entry == NULL) {
        break;
      }

      bytes_to_trits(entry->payload.gossip->packet.content, GOSSIP_TX_BYTES_LENGTH, tx,
                     NUM_TRITS_SERIALIZED_TRANSACTION);
      trits_to_ptrits(tx, acc, packets_num, NUM_TRITS_SERIALIZED_TRANSACTION);

      packets_num++;
    }

    if (!hasher->running) {
      break;
    }

    if (packets_num == 0) {
      cond_handle_wait(&hasher->cond, &lock_cond);
      continue;
    }

    ptrit_curl_init(&curl, CURL_P_81);
    memset(acc, 0, NUM_TRITS_SERIALIZED_TRANSACTION * sizeof(ptrit_t));
    memset(flex_hash, FLEX_TRIT_NULL_VALUE, sizeof(flex_hash));

    ptrit_curl_absorb(&curl, acc, NUM_TRITS_SERIALIZED_TRANSACTION);
    ptrit_curl_squeeze(&curl, acc, HASH_LENGTH_TRIT);

    for (size_t j = 0; j < packets_num; j++) {
      ptrits_to_trits(acc, hash, j, HASH_LENGTH_TRIT);
      flex_trits_from_trits(flex_hash, HASH_LENGTH_TRIT, hash, HASH_LENGTH_TRIT, HASH_LENGTH_TRIT);

      // if (process_packet(processor, &tangle, &packets[j].entry->packet, flex_hash, false, packets[j].digest) !=
      // RC_OK) {
      //   log_warning(logger_id, "Processing packet failed\n");
      // }
      // free(packets[j].entry);
    }
  }

  lock_handle_unlock(&lock_cond);
  lock_handle_destroy(&lock_cond);

  return NULL;
}

/**
 * Public functions
 */

retcode_t hasher_stage_init(hasher_stage_t *const hasher) {
  if (hasher == NULL) {
    return RC_NULL_PARAM;
  }

  logger_id = logger_helper_enable(HASHER_LOGGER_ID, LOGGER_DEBUG, true);

  hasher->running = false;
  // hasher->queue = NULL;
  lock_handle_init(&hasher->lock);
  cond_handle_init(&hasher->cond);

  return RC_OK;
}

retcode_t hasher_stage_start(hasher_stage_t *const hasher) {
  if (hasher == NULL) {
    return RC_NULL_PARAM;
  }

  log_info(logger_id, "Spawning hasher stage thread\n");
  hasher->running = true;
  if (thread_handle_create(&hasher->thread, (thread_routine_t)hasher_stage_routine, hasher) != 0) {
    log_critical(logger_id, "Spawning hasher stage thread failed\n");
    return RC_THREAD_CREATE;
  }

  return RC_OK;
}

retcode_t hasher_stage_stop(hasher_stage_t *const hasher) {
  retcode_t ret = RC_OK;

  if (hasher == NULL) {
    return RC_NULL_PARAM;
  } else if (hasher->running == false) {
    return RC_OK;
  }

  log_info(logger_id, "Shutting down hasher stage thread\n");
  hasher->running = false;
  cond_handle_signal(&hasher->cond);
  if (thread_handle_join(hasher->thread, NULL) != 0) {
    log_error(logger_id, "Shutting down hasher stage thread failed\n");
    ret = RC_THREAD_JOIN;
  }

  return ret;
}

retcode_t hasher_stage_destroy(hasher_stage_t *const hasher) {
  retcode_t ret = RC_OK;

  if (hasher == NULL) {
    return RC_NULL_PARAM;
  } else if (hasher->running) {
    return RC_STILL_RUNNING;
  }

  hasher_payload_queue_free(&hasher->queue);
  lock_handle_destroy(&hasher->lock);
  cond_handle_destroy(&hasher->cond);

  logger_helper_release(logger_id);

  return ret;
}

retcode_t hasher_stage_add(hasher_stage_t *const hasher, protocol_gossip_queue_entry_t *const gossip,
                           uint64_t const digest) {
  retcode_t ret = RC_OK;

  if (hasher == NULL) {
    return RC_NULL_PARAM;
  }

  lock_handle_lock(&hasher->lock);
  ret = hasher_payload_queue_push(&hasher->queue, gossip, digest);
  lock_handle_unlock(&hasher->lock);

  if (ret != RC_OK) {
    log_warning(logger_id, "Pushing packet to hasher stage queue failed\n");
    return ret;
  } else {
    cond_handle_signal(&hasher->cond);
  }

  return RC_OK;
}

size_t hasher_stage_size(hasher_stage_t *const hasher) {
  size_t size = 0;

  if (hasher == NULL) {
    return 0;
  }

  lock_handle_lock(&hasher->lock);
  size = hasher_payload_queue_count(hasher->queue);
  lock_handle_unlock(&hasher->lock);

  return size;
}

size_t hasher_payload_queue_count(hasher_payload_queue_t const queue) {
  hasher_payload_queue_entry_t *iter = NULL;
  size_t count = 0;

  CDL_COUNT(queue, iter, count);

  return count;
}

retcode_t hasher_payload_queue_push(hasher_payload_queue_t *const queue, protocol_gossip_queue_entry_t *const gossip,
                                    uint64_t const digest) {
  hasher_payload_queue_entry_t *entry = NULL;

  if (queue == NULL || gossip == NULL) {
    return RC_NULL_PARAM;
  }

  if ((entry = (hasher_payload_queue_entry_t *)malloc(sizeof(hasher_payload_queue_entry_t))) == NULL) {
    return RC_OOM;
  }

  entry->payload.gossip = gossip;
  entry->payload.digest = digest;
  CDL_APPEND(*queue, entry);

  return RC_OK;
}

hasher_payload_queue_entry_t *hasher_payload_queue_pop(hasher_payload_queue_t *const queue) {
  hasher_payload_queue_entry_t *front = NULL;

  if (queue == NULL) {
    return NULL;
  }

  front = *queue;
  if (front != NULL) {
    CDL_DELETE(*queue, front);
  }

  return front;
}

void hasher_payload_queue_free(hasher_payload_queue_t *const queue) {
  hasher_payload_queue_entry_t *iter = NULL, *tmp1 = NULL, *tmp2 = NULL;

  if (queue == NULL) {
    return;
  }

  CDL_FOREACH_SAFE(*queue, iter, tmp1, tmp2) {
    CDL_DELETE(*queue, iter);
    free(iter);
  }
  *queue = NULL;
}
