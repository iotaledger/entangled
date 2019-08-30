/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "ciri/node/pipeline/tips_requester.h"
#include "ciri/consensus/tangle/tangle.h"
#include "ciri/node/node.h"
#include "ciri/node/protocol/gossip.h"
#include "common/model/milestone.h"
#include "utils/logger_helper.h"

#define TIPS_REQUESTER_LOGGER_ID "tips_requester"
#define TIPS_REQUESTER_INTERVAL_MS 5000ULL

static logger_id_t logger_id;

/*
 * Private functions
 */

static void *tips_requester_routine(tips_requester_t *const tips_requester) {
  protocol_gossip_t packet;
  neighbor_t *neighbor = NULL;
  DECLARE_PACK_SINGLE_TX(transaction, transaction_ptr, transaction_pack);
  DECLARE_PACK_SINGLE_MILESTONE(latest_milestone, latest_milestone_ptr, milestone_pack);
  flex_trit_t transaction_flex_trits[FLEX_TRIT_SIZE_8019];
  lock_handle_t lock_cond;
  tangle_t tangle;

  if (tips_requester == NULL) {
    return NULL;
  }

  {
    storage_connection_config_t db_conf = {.db_path = tips_requester->node->conf.tangle_db_path};

    if (iota_tangle_init(&tangle, &db_conf) != RC_OK) {
      log_critical(logger_id, "Initializing tangle connection failed\n");
      return NULL;
    }
  }

  lock_handle_init(&lock_cond);
  lock_handle_lock(&lock_cond);

  while (tips_requester->running) {
    cond_handle_timedwait(&tips_requester->cond, &lock_cond, TIPS_REQUESTER_INTERVAL_MS);
    hash_pack_reset(&milestone_pack);
    if (iota_tangle_milestone_load_last(&tangle, &milestone_pack) != RC_OK || milestone_pack.num_loaded == 0) {
      continue;
    }
    hash_pack_reset(&transaction_pack);
    if (iota_tangle_transaction_load(&tangle, TRANSACTION_FIELD_HASH, latest_milestone.hash, &transaction_pack) !=
            RC_OK ||
        transaction_pack.num_loaded == 0) {
      continue;
    }
    transaction_serialize_on_flex_trits(transaction_ptr, transaction_flex_trits);
    if (protocol_gossip_set_transaction(&packet, transaction_flex_trits) != RC_OK) {
      continue;
    }
    if (protocol_gossip_set_request(&packet, latest_milestone.hash,
                                    HASH_LENGTH_TRIT - tips_requester->node->conf.mwm) != RC_OK) {
      continue;
    }

    rw_lock_handle_rdlock(&tips_requester->node->router.neighbors_lock);
    NEIGHBORS_FOREACH(tips_requester->node->router.neighbors, neighbor) {
      if (neighbor->endpoint.stream != NULL) {
        if (neighbor_send_packet(tips_requester->node, neighbor, &packet) != RC_OK) {
          log_warning(logger_id, "Sending tip request to neighbor failed\n");
        }
      }
    }
    rw_lock_handle_unlock(&tips_requester->node->router.neighbors_lock);
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

retcode_t tips_requester_init(tips_requester_t *const tips_requester, node_t *const node) {
  if (tips_requester == NULL || node == NULL) {
    return RC_NULL_PARAM;
  }

  logger_id = logger_helper_enable(TIPS_REQUESTER_LOGGER_ID, LOGGER_DEBUG, true);

  tips_requester->running = false;
  tips_requester->node = node;
  cond_handle_init(&tips_requester->cond);

  return RC_OK;
}

retcode_t tips_requester_start(tips_requester_t *const tips_requester) {
  if (tips_requester == NULL) {
    return RC_NULL_PARAM;
  }

  log_info(logger_id, "Spawning tips requester thread\n");
  tips_requester->running = true;
  if (thread_handle_create(&tips_requester->thread, (thread_routine_t)tips_requester_routine, tips_requester) != 0) {
    log_critical(logger_id, "Spawning tips requester thread failed\n");
    return RC_THREAD_CREATE;
  }

  return RC_OK;
}

retcode_t tips_requester_stop(tips_requester_t *const tips_requester) {
  if (tips_requester == NULL) {
    return RC_NULL_PARAM;
  } else if (tips_requester->running == false) {
    return RC_OK;
  }

  log_info(logger_id, "Shutting down tips requester thread\n");
  tips_requester->running = false;
  cond_handle_signal(&tips_requester->cond);
  if (thread_handle_join(tips_requester->thread, NULL) != 0) {
    log_error(logger_id, "Shutting down tips requester thread failed\n");
    return RC_THREAD_JOIN;
  }

  return RC_OK;
}

retcode_t tips_requester_destroy(tips_requester_t *const tips_requester) {
  if (tips_requester == NULL) {
    return RC_NULL_PARAM;
  } else if (tips_requester->running) {
    return RC_STILL_RUNNING;
  }

  tips_requester->node = NULL;
  cond_handle_destroy(&tips_requester->cond);

  logger_helper_release(logger_id);

  return RC_OK;
}
