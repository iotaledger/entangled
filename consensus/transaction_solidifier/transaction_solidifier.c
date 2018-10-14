/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "transaction_solidifier.h"

#define TRANSACTION_SOLIDIFIER_LOGGER_ID "consensus_transaction_solidifier"

static void *spawn_solid_transactions_propogation(void *arg) {
  transaction_solidifier_t *ts = (transaction_solidifier_t *)arg;
  while (ts->running) {
    sleep(0.5);
  }
  return NULL;
}

void iota_consensus_transaction_solidifier_init(
    transaction_solidifier_t *const ts, requester_state_t *const requester) {
  ts->requester = requester;
  ts->running = false;
}

retcode_t iota_consensus_transaction_solidifier_start(
    transaction_solidifier_t *const ts) {
  if (ts == NULL) {
    return RC_CONSENSUS_NULL_PTR;
  }

  ts->running = true;
  log_info(TRANSACTION_SOLIDIFIER_LOGGER_ID,
           "Spawning transaction_solidifier thread\n");
  if (thread_handle_create(
          &ts->thread, (thread_routine_t)spawn_solid_transactions_propogation,
          ts) != 0) {
    log_critical(TRANSACTION_SOLIDIFIER_LOGGER_ID,
                 "Spawning cIRI API thread failed\n");
    return RC_CIRI_API_FAILED_THREAD_SPAWN;
  }
  return RC_OK;
}

retcode_t iota_consensus_transaction_solidifier_stop(
    transaction_solidifier_t *const ts) {
  retcode_t ret = RC_OK;

  if (ts == NULL) {
    return RC_CONSENSUS_NULL_PTR;
  } else if (ts->running == false) {
    return RC_OK;
  }

  ts->running = false;

  log_info(TRANSACTION_SOLIDIFIER_LOGGER_ID,
           "Shutting down transaction_solidifier thread\n");
  if (thread_handle_join(ts->thread, NULL) != 0) {
    log_error(TRANSACTION_SOLIDIFIER_LOGGER_ID,
              "Shutting down transaction_solidifier thread failed\n");
    ret = RC_CIRI_API_FAILED_THREAD_JOIN;
  }
  return ret;
}

retcode_t iota_consensus_transaction_solidifier_destroy(
    transaction_solidifier_t *const ts) {
  if (ts == NULL) {
    return RC_CONSENSUS_NULL_PTR;
  } else if (ts->running) {
    return RC_CIRI_API_STILL_RUNNING;
  }

  logger_helper_destroy(TRANSACTION_SOLIDIFIER_LOGGER_ID);
  return RC_OK;
}