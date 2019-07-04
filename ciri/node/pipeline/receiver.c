/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdlib.h>

#include "ciri/node/network/event.h"
#include "ciri/node/node.h"
#include "ciri/node/pipeline/receiver.h"
#include "utils/logger_helper.h"

#define RECEIVER_LOGGER_ID "receiver"

static logger_id_t logger_id;

retcode_t receiver_stage_init(receiver_stage_t *const receiver, node_t *const node, uint16_t port) {
  if (receiver == NULL || node == NULL) {
    return RC_NULL_PARAM;
  }

  logger_id = logger_helper_enable(RECEIVER_LOGGER_ID, LOGGER_DEBUG, true);

  receiver->running = false;
  receiver->node = node;
  receiver->port = port;

  return RC_OK;
}

retcode_t receiver_stage_start(receiver_stage_t *const receiver) {
  if (receiver == NULL) {
    return RC_NULL_PARAM;
  }

  log_info(logger_id, "Spawning receiver stage thread\n");
  receiver->running = true;
  if (thread_handle_create(&receiver->thread, (thread_routine_t)event_routine, receiver) != 0) {
    log_critical(logger_id, "Spawning receiver stage thread failed\n");
    return RC_THREAD_CREATE;
  }

  return RC_OK;
}

retcode_t receiver_stage_stop(receiver_stage_t *const receiver) {
  retcode_t ret = RC_OK;

  if (receiver == NULL) {
    return RC_NULL_PARAM;
  } else if (receiver->running == false) {
    return RC_OK;
  }

  log_info(logger_id, "Shutting down receiver stage thread\n");
  receiver->running = false;
  if (thread_handle_join(receiver->thread, NULL) != 0) {
    log_error(logger_id, "Shutting down receiver stage thread failed\n");
    ret = RC_THREAD_JOIN;
  }

  return ret;
}

retcode_t receiver_stage_destroy(receiver_stage_t *const receiver) {
  if (receiver == NULL) {
    return RC_NULL_PARAM;
  } else if (receiver->running) {
    return RC_STILL_RUNNING;
  }

  logger_helper_release(logger_id);

  return RC_OK;
}
