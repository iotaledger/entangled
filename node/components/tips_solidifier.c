/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "node/components/tips_solidifier.h"
#include "consensus/tangle/tangle.h"
#include "consensus/transaction_solidifier/transaction_solidifier.h"
#include "utils/logger_helper.h"
#include "utils/time.h"

#define TIPS_SOLIDIFIER_LOGGER_ID "tips_solidifier"
#define TIPS_SOLIDIFICATION_INTERVAL_MS 750ULL

static logger_id_t logger_id;

/*
 * Private functions
 */

/**
 * Routine that continuously selects a random tip from a tips cache, checks if
 * it's still a tip and tries to solidify it
 *
 * @param tips_solidifier The tips solidifier
 *
 * @return a status code
 */
static void *tips_solidifier_routine(tips_solidifier_t *const tips_solidifier) {
  bool is_solid = false;
  size_t approvers_count = 0;
  flex_trit_t tip[FLEX_TRIT_SIZE_243];
  tangle_t tangle;

  if (tips_solidifier == NULL) {
    return NULL;
  }

  {
    connection_config_t db_conf = {.db_path = tips_solidifier->conf->tangle_db_path};

    if (iota_tangle_init(&tangle, &db_conf) != RC_OK) {
      log_critical(logger_id, "Initializing tangle connection failed\n");
      return NULL;
    }
  }

  lock_handle_t lock_cond;
  lock_handle_init(&lock_cond);
  lock_handle_lock(&lock_cond);

  while (tips_solidifier->running) {
    cond_handle_timedwait(&tips_solidifier->cond, &lock_cond, TIPS_SOLIDIFICATION_INTERVAL_MS);
    if (tips_cache_non_solid_size(tips_solidifier->tips) == 0) {
      continue;
    }

    if (tips_cache_random_tip(tips_solidifier->tips, tip) != RC_OK) {
      log_warning(logger_id, "Accessing random tip from cache failed\n");
      continue;
    }

    if (iota_tangle_transaction_approvers_count(&tangle, tip, &approvers_count) != RC_OK) {
      log_warning(logger_id, "Counting number of approvers of tip failed\n");
      continue;
    }

    if (approvers_count != 0) {
      if (tips_cache_remove(tips_solidifier->tips, tip) != RC_OK) {
        log_warning(logger_id, "Removing tip from cache failed\n");
      }
      continue;
    }

    is_solid = false;
    if (iota_consensus_transaction_solidifier_check_solidity(tips_solidifier->transaction_solidifier, &tangle, tip,
                                                             false, &is_solid) != RC_OK) {
      log_warning(logger_id, "Checking solidity of tip failed\n");
      continue;
    }

    if (is_solid) {
      if (tips_cache_set_solid(tips_solidifier->tips, tip) != RC_OK) {
        log_warning(logger_id, "Changing status of tip to solid failed\n");
      }
    }
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

retcode_t tips_solidifier_init(tips_solidifier_t *const tips_solidifier, iota_node_conf_t *const conf,
                               tips_cache_t *const tips, transaction_solidifier_t *const transaction_solidifier) {
  if (tips_solidifier == NULL || conf == NULL || tips == NULL || transaction_solidifier == NULL) {
    return RC_NULL_PARAM;
  }

  logger_id = logger_helper_enable(TIPS_SOLIDIFIER_LOGGER_ID, LOGGER_DEBUG, true);
  tips_solidifier->conf = conf;
  tips_solidifier->running = false;
  tips_solidifier->tips = tips;
  tips_solidifier->transaction_solidifier = transaction_solidifier;
  cond_handle_init(&tips_solidifier->cond);

  return RC_OK;
}

retcode_t tips_solidifier_start(tips_solidifier_t *const tips_solidifier) {
  if (tips_solidifier == NULL) {
    return RC_NULL_PARAM;
  }

  log_info(logger_id, "Spawning tips solidifier thread\n");
  tips_solidifier->running = true;
  if (thread_handle_create(&tips_solidifier->thread, (thread_routine_t)tips_solidifier_routine, tips_solidifier) != 0) {
    log_error(logger_id, "Spawning tips solidifier thread failed\n");
    return RC_FAILED_THREAD_SPAWN;
  }

  return RC_OK;
}

retcode_t tips_solidifier_stop(tips_solidifier_t *const tips_solidifier) {
  if (tips_solidifier == NULL) {
    return RC_NULL_PARAM;
  } else if (tips_solidifier->running == false) {
    return RC_OK;
  }

  log_info(logger_id, "Shutting down tips solidifier thread\n");
  tips_solidifier->running = false;
  cond_handle_signal(&tips_solidifier->cond);
  if (thread_handle_join(tips_solidifier->thread, NULL) != 0) {
    log_error(logger_id, "Shutting down tips solidifier thread failed\n");
    return RC_FAILED_THREAD_JOIN;
  }

  return RC_OK;
}

retcode_t tips_solidifier_destroy(tips_solidifier_t *const tips_solidifier) {
  if (tips_solidifier == NULL) {
    return RC_NULL_PARAM;
  } else if (tips_solidifier->running) {
    return RC_STILL_RUNNING;
  }

  tips_solidifier->conf = NULL;
  tips_solidifier->tips = NULL;
  tips_solidifier->transaction_solidifier = NULL;
  cond_handle_destroy(&tips_solidifier->cond);

  logger_helper_release(logger_id);

  return RC_OK;
}
