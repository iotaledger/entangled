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

retcode_t receiver_init(receiver_state_t *const state, node_t *const node, uint16_t tcp_port, uint16_t udp_port) {
  if (state == NULL || node == NULL) {
    return RC_NULL_PARAM;
  }

  logger_id = logger_helper_enable(RECEIVER_COMPONENT_LOGGER_ID, LOGGER_DEBUG, true);
  memset(state, 0, sizeof(receiver_state_t));
  state->running = false;
  state->tcp_service.port = tcp_port;
  state->tcp_service.protocol = PROTOCOL_TCP;
  state->tcp_service.state = state;
  state->tcp_service.processor = &node->processor;
  state->tcp_service.context = NULL;
  state->tcp_service.opaque_socket = NULL;
  state->udp_service.port = udp_port;
  state->udp_service.protocol = PROTOCOL_UDP;
  state->udp_service.state = state;
  state->udp_service.processor = &node->processor;
  state->udp_service.context = NULL;
  state->udp_service.opaque_socket = NULL;
  state->node = node;
  return RC_OK;
}

retcode_t receiver_start(receiver_state_t *const state) {
  if (state == NULL) {
    return RC_NULL_PARAM;
  }

  state->running = true;
  if (state->tcp_service.port != 0) {
    log_info(logger_id, "Spawning TCP receiver thread\n");
    if (thread_handle_create(&state->tcp_service.thread, (thread_routine_t)receiver_service_start,
                             &state->tcp_service) != 0) {
      log_critical(logger_id, "Spawning TCP receiver thread failed\n");
      return RC_THREAD_CREATE;
    }
  }
  if (state->udp_service.port != 0) {
    log_info(logger_id, "Spawning UDP receiver thread\n");
    if (thread_handle_create(&state->udp_service.thread, (thread_routine_t)receiver_service_start,
                             &state->udp_service) != 0) {
      log_critical(logger_id, "Spawning UDP receiver thread failed\n");
      return RC_THREAD_CREATE;
    }
  }
  return RC_OK;
}

retcode_t receiver_stop(receiver_state_t *const state) {
  retcode_t ret = RC_OK;

  if (state == NULL) {
    return RC_NULL_PARAM;
  } else if (state->running == false) {
    return RC_OK;
  }

  state->running = false;
  log_info(logger_id, "Shutting down TCP receiver thread\n");
  if (receiver_service_stop(&state->tcp_service) == false || thread_handle_join(state->tcp_service.thread, NULL) != 0) {
    log_error(logger_id, "Shutting down TCP receiver thread failed\n");
    ret = RC_THREAD_JOIN;
  }
  log_info(logger_id, "Shutting down UDP receiver thread\n");
  if (receiver_service_stop(&state->udp_service) == false || thread_handle_join(state->udp_service.thread, NULL) != 0) {
    log_error(logger_id, "Shutting down UDP receiver thread failed\n");
    ret = RC_THREAD_JOIN;
  }
  return ret;
}

retcode_t receiver_destroy(receiver_state_t *const state) {
  if (state == NULL) {
    return RC_NULL_PARAM;
  } else if (state->running) {
    return RC_STILL_RUNNING;
  }

  logger_helper_release(logger_id);
  return RC_OK;
}
