/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "gossip/components/tips_requester.h"
#include "common/model/milestone.h"
#include "gossip/iota_packet.h"
#include "gossip/node.h"
#include "utils/containers/lists/concurrent_list_neighbor.h"
#include "utils/logger_helper.h"

#define TIPS_REQUESTER_LOGGER_ID "tips_requester"
#define TIPS_REQUESTER_INTERVAL 5

/*
 * Private functions
 */

static void *tips_requester_routine(tips_requester_t *const tips_requester) {
  iota_packet_t packet;
  concurrent_list_node_neighbor_t *iter = NULL;
  DECLARE_PACK_SINGLE_TX(transaction, transaction_ptr, transaction_pack);
  DECLARE_PACK_SINGLE_MILESTONE(latest_milestone, latest_milestone_ptr,
                                milestone_pack);
  trit_array_t const key = {.trits = latest_milestone.hash,
                            .num_trits = HASH_LENGTH_TRIT,
                            .num_bytes = FLEX_TRIT_SIZE_243,
                            .dynamic = 0};
  flex_trit_t transaction_flex_trits[FLEX_TRIT_SIZE_8019];

  if (tips_requester == NULL) {
    return NULL;
  }

  while (tips_requester->running) {
    hash_pack_reset(&milestone_pack);
    if (iota_tangle_milestone_load_last(tips_requester->tangle,
                                        &milestone_pack) != RC_OK ||
        milestone_pack.num_loaded == 0) {
      continue;
    }
    hash_pack_reset(&transaction_pack);
    if (iota_tangle_transaction_load(tips_requester->tangle,
                                     TRANSACTION_FIELD_HASH, &key,
                                     &transaction_pack) != RC_OK ||
        transaction_pack.num_loaded == 0) {
      continue;
    }
    transaction_serialize_on_flex_trits(transaction_ptr,
                                        transaction_flex_trits);
    if (iota_packet_set_transaction(&packet, transaction_flex_trits) != RC_OK) {
      continue;
    }
    if (iota_packet_set_request(
            &packet, latest_milestone.hash,
            tips_requester->node->conf.request_hash_size_trit) != RC_OK) {
      continue;
    }

    if (tips_requester->node->neighbors) {
      iter = tips_requester->node->neighbors->front;
      while (iter) {
        if (neighbor_send_packet(tips_requester->node, &iter->data, &packet) !=
            RC_OK) {
          log_warning(TIPS_REQUESTER_LOGGER_ID,
                      "Sending packet to neighbor failed\n");
        }
        iter = iter->next;
      }
    }
    sleep(TIPS_REQUESTER_INTERVAL);
  }

  return NULL;
}

/*
 * Public functions
 */

retcode_t tips_requester_init(tips_requester_t *const tips_requester,
                              node_t *const node, tangle_t *const tangle) {
  if (tips_requester == NULL || node == NULL || tangle == NULL) {
    return RC_NULL_PARAM;
  }

  logger_helper_init(TIPS_REQUESTER_LOGGER_ID, LOGGER_DEBUG, true);

  tips_requester->running = false;
  tips_requester->node = node;
  tips_requester->tangle = tangle;

  return RC_OK;
}

retcode_t tips_requester_start(tips_requester_t *const tips_requester) {
  if (tips_requester == NULL) {
    return RC_NULL_PARAM;
  }

  log_info(TIPS_REQUESTER_LOGGER_ID, "Spawning tips requester thread\n");
  tips_requester->running = true;
  if (thread_handle_create(&tips_requester->thread,
                           (thread_routine_t)tips_requester_routine,
                           tips_requester) != 0) {
    log_critical(TIPS_REQUESTER_LOGGER_ID,
                 "Spawning tips requester thread failed\n");
    return RC_FAILED_THREAD_SPAWN;
  }

  return RC_OK;
}

retcode_t tips_requester_stop(tips_requester_t *const tips_requester) {
  if (tips_requester == NULL) {
    return RC_NULL_PARAM;
  } else if (tips_requester->running == false) {
    return RC_OK;
  }

  log_info(TIPS_REQUESTER_LOGGER_ID, "Shutting down tips requester thread\n");
  tips_requester->running = false;
  if (thread_handle_join(tips_requester->thread, NULL) != 0) {
    log_error(TIPS_REQUESTER_LOGGER_ID,
              "Shutting down tips requester thread failed\n");
    return RC_FAILED_THREAD_JOIN;
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
  tips_requester->tangle = NULL;

  logger_helper_destroy(TIPS_REQUESTER_LOGGER_ID);

  return RC_OK;
}
