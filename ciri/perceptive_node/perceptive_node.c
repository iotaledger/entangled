/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "ciri/perceptive_node/perceptive_node.h"

#define PERCEPTIVE_NODE_LOGGER_ID "perceptive_node"

/*
 * Private functions
 */

static void *perceptive_node_routine(iota_perceptive_node_t *const pn) {
  while (pn->running) {
    // TODO -
    // Pick a neighbor and start monitoring
    log_info(PERCEPTIVE_NODE_LOGGER_ID, "In %s, interval expired\n",
             __FUNCTION__);
    sleep(pn->conf.monitoring_interval_seconds);
  }

  return NULL;
}

retcode_t iota_perceptive_node_init(struct iota_perceptive_node_s *const pn,
                                    iota_consensus_t const *const consensus) {
  if (pn == NULL) {
    return RC_NULL_PARAM;
  }

  logger_helper_init(PERCEPTIVE_NODE_LOGGER_ID, LOGGER_DEBUG, true);
  pn->running = false;
  pn->consensus = consensus;
  return RC_OK;
}
retcode_t iota_perceptive_node_start(iota_perceptive_node_t *const pn) {
  if (pn == NULL) {
    return RC_NULL_PARAM;
  }

  log_info(PERCEPTIVE_NODE_LOGGER_ID, "Starting perceptive node\n");

  pn->running = true;

  if (thread_handle_create(
          &pn->thread, (thread_routine_t)perceptive_node_routine, pn) != 0) {
    return RC_FAILED_THREAD_SPAWN;
  }

  return RC_OK;
}
retcode_t iota_perceptive_node_stop(iota_perceptive_node_t *const pn) {
  if (pn == NULL) {
    return RC_NULL_PARAM;
  } else if (pn->running == false) {
    return RC_OK;
  }

  log_info(PERCEPTIVE_NODE_LOGGER_ID, "Shutting down perceptive node thread\n");
  pn->running = false;
  if (thread_handle_join(pn->thread, NULL) != 0) {
    log_error(PERCEPTIVE_NODE_LOGGER_ID,
              "Shutting down perceptive_node thread failed\n");
    return RC_FAILED_THREAD_JOIN;
  }

  return RC_OK;
}

retcode_t iota_perceptive_node_destroy(iota_perceptive_node_t *const pn) {
  if (pn == NULL) {
    return RC_NULL_PARAM;
  } else if (pn->running) {
    return RC_PERCEPTIVE_NODE_STILL_RUNNING;
  }
  logger_helper_destroy(PERCEPTIVE_NODE_LOGGER_ID);

  return RC_OK;
}
