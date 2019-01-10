/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <string.h>

#include "consensus/tangle/tangle.h"
#include "gossip/components/broadcaster.h"
#include "gossip/node.h"
#include "utils/handles/lock.h"
#include "utils/logger_helper.h"

#define BROADCASTER_LOGGER_ID "broadcaster"
#define BROADCASTER_TIMEOUT_S 1

/*
 * Private functions
 */

static void *broadcaster_routine(broadcaster_t *const broadcaster) {
  neighbor_t *iter = NULL;
  flex_trit_t transaction_flex_trits[FLEX_TRIT_SIZE_8019];
  bool has_dequeued = false;
  connection_config_t db_conf = {.db_path = broadcaster->node->conf.db_path};
  tangle_t tangle;

  if (broadcaster == NULL) {
    return NULL;
  }

  if (iota_tangle_init(&tangle, &db_conf) != RC_OK) {
    log_critical(BROADCASTER_LOGGER_ID,
                 "Initializing tangle connection failed\n");
    return NULL;
  }

  lock_handle_t lock_cond;
  lock_handle_init(&lock_cond);
  lock_handle_lock(&lock_cond);

  while (broadcaster->running) {
    if (LF_MPMC_QUEUE_IS_EMPTY(&broadcaster->queue)) {
      cond_handle_timedwait(&broadcaster->cond, &lock_cond,
                            BROADCASTER_TIMEOUT_S);
    }

    if (lf_mpmc_queue_flex_trit_t_trydequeue(&broadcaster->queue,
                                             transaction_flex_trits,
                                             &has_dequeued) != RC_OK) {
      log_warning(BROADCASTER_LOGGER_ID, "Dequeuing transaction failed\n");
      continue;
    }

    if (!has_dequeued) {
      continue;
    }

    log_debug(BROADCASTER_LOGGER_ID, "Broadcasting transaction\n");
    rw_lock_handle_rdlock(&broadcaster->node->neighbors_lock);
    LL_FOREACH(broadcaster->node->neighbors, iter) {
      if (neighbor_send(broadcaster->node, &tangle, iter,
                        transaction_flex_trits) != RC_OK) {
        log_warning(BROADCASTER_LOGGER_ID, "Broadcasting transaction failed\n");
      }
    }
    rw_lock_handle_unlock(&broadcaster->node->neighbors_lock);
  }

  lock_handle_unlock(&lock_cond);
  lock_handle_destroy(&lock_cond);

  if (iota_tangle_destroy(&tangle) != RC_OK) {
    log_critical(BROADCASTER_LOGGER_ID,
                 "Destroying tangle connection failed\n");
  }

  return NULL;
}

/*
 * Public functions
 */

retcode_t broadcaster_init(broadcaster_t *const broadcaster,
                           node_t *const node) {
  retcode_t ret = RC_OK;

  if (broadcaster == NULL || node == NULL) {
    return RC_NULL_PARAM;
  }

  logger_helper_enable(BROADCASTER_LOGGER_ID, LOGGER_DEBUG, true);
  memset(broadcaster, 0, sizeof(broadcaster_t));
  broadcaster->running = false;
  broadcaster->node = node;

  cond_handle_init(&broadcaster->cond);

  if ((ret = lf_mpmc_queue_flex_trit_t_init(&broadcaster->queue,
                                            FLEX_TRIT_SIZE_8019)) != RC_OK) {
    log_critical(BROADCASTER_LOGGER_ID, "Initializing queue failed\n");
    return ret;
  }

  return ret;
}

retcode_t broadcaster_destroy(broadcaster_t *const broadcaster) {
  retcode_t ret = RC_OK;

  if (broadcaster == NULL) {
    return RC_NULL_PARAM;
  } else if (broadcaster->running) {
    return RC_BROADCASTER_STILL_RUNNING;
  }

  broadcaster->node = NULL;

  cond_handle_destroy(&broadcaster->cond);

  if ((ret = lf_mpmc_queue_flex_trit_t_destroy(&broadcaster->queue)) != RC_OK) {
    log_error(BROADCASTER_LOGGER_ID, "Destroying queue failed\n");
  }

  logger_helper_release(BROADCASTER_LOGGER_ID);

  return ret;
}

retcode_t broadcaster_start(broadcaster_t *const broadcaster) {
  if (broadcaster == NULL) {
    return RC_NULL_PARAM;
  }

  log_info(BROADCASTER_LOGGER_ID, "Spawning broadcaster thread\n");
  broadcaster->running = true;
  if (thread_handle_create(&broadcaster->thread,
                           (thread_routine_t)broadcaster_routine,
                           broadcaster) != 0) {
    return RC_FAILED_THREAD_SPAWN;
  }

  return RC_OK;
}

retcode_t broadcaster_on_next(broadcaster_t *const broadcaster,
                              flex_trit_t const *const transaction_flex_trits) {
  retcode_t ret = RC_OK;

  if (broadcaster == NULL || transaction_flex_trits == NULL) {
    return RC_NULL_PARAM;
  }

  if ((ret = lf_mpmc_queue_flex_trit_t_enqueue(
           &broadcaster->queue, transaction_flex_trits)) != RC_OK) {
    log_warning(BROADCASTER_LOGGER_ID, "Enqueuing transaction failed\n");
    return ret;
  }

  cond_handle_signal(&broadcaster->cond);

  return ret;
}

retcode_t broadcaster_stop(broadcaster_t *const broadcaster) {
  if (broadcaster == NULL) {
    return RC_NULL_PARAM;
  } else if (broadcaster->running == false) {
    return RC_OK;
  }

  log_info(BROADCASTER_LOGGER_ID, "Shutting down broadcaster thread\n");
  broadcaster->running = false;
  if (thread_handle_join(broadcaster->thread, NULL) != 0) {
    log_error(BROADCASTER_LOGGER_ID,
              "Shutting down broadcaster thread failed\n");
    return RC_FAILED_THREAD_JOIN;
  }

  return RC_OK;
}
