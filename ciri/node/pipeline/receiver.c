/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <string.h>

#include "ciri/node/node.h"
#include "ciri/node/pipeline/receiver.h"
#include "utils/logger_helper.h"

#define RECEIVER_COMPONENT_LOGGER_ID "receiver"

static logger_id_t logger_id;

retcode_t receiver_init(receiver_t *const receiver, node_t *const node, uint16_t tcp_port) {
  if (receiver == NULL || node == NULL) {
    return RC_NULL_PARAM;
  }

  logger_id = logger_helper_enable(RECEIVER_COMPONENT_LOGGER_ID, LOGGER_DEBUG, true);
  memset(receiver, 0, sizeof(receiver_t));
  receiver->running = false;
  receiver->node = node;

  return RC_OK;
}

retcode_t receiver_start(receiver_t *const receiver) {
  if (receiver == NULL) {
    return RC_NULL_PARAM;
  }

  receiver->running = true;

  return RC_OK;
}

retcode_t receiver_stop(receiver_t *const receiver) {
  retcode_t ret = RC_OK;

  if (receiver == NULL) {
    return RC_NULL_PARAM;
  } else if (receiver->running == false) {
    return RC_OK;
  }

  receiver->running = false;

  return ret;
}

retcode_t receiver_destroy(receiver_t *const receiver) {
  if (receiver == NULL) {
    return RC_NULL_PARAM;
  } else if (receiver->running) {
    return RC_STILL_RUNNING;
  }

  logger_helper_release(logger_id);

  return RC_OK;
}
