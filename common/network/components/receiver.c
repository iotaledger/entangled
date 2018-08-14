/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "common/network/components/receiver.h"
#include "ciri/node.h"
#include "utils/logger_helper.h"

#define RECEIVER_COMPONENT_LOGGER_ID "receiver_component"

bool receiver_init(receiver_state_t *const state, node_t *const node,
                   uint16_t tcp_port, uint16_t udp_port) {
  if (state == NULL || node == NULL) {
    return false;
  }
  logger_helper_init(RECEIVER_COMPONENT_LOGGER_ID, LOGGER_DEBUG, true);
  if (node->processor.queue == NULL) {
    return false;
  }
  state->running = false;
  state->tcp_service.port = tcp_port;
  state->tcp_service.protocol = PROTOCOL_TCP;
  state->tcp_service.context = NULL;
  state->tcp_service.state = state;
  state->tcp_service.queue = node->processor.queue;
  state->udp_service.port = udp_port;
  state->udp_service.protocol = PROTOCOL_UDP;
  state->udp_service.context = NULL;
  state->udp_service.state = state;
  state->udp_service.queue = node->processor.queue;
  state->node = node;
  return true;
}

bool receiver_start(receiver_state_t *const state) {
  if (state == NULL) {
    return false;
  }
  state->running = true;
  if (state->tcp_service.port != 0) {
    log_info(RECEIVER_COMPONENT_LOGGER_ID, "Spawning TCP receiver thread\n");
    if (thread_handle_create(&state->tcp_service.thread,
                             (thread_routine_t)receiver_service_start,
                             &state->tcp_service) != 0) {
      log_critical(RECEIVER_COMPONENT_LOGGER_ID,
                   "Spawning TCP receiver thread failed");
      return false;
    }
  }
  if (state->udp_service.port != 0) {
    log_info(RECEIVER_COMPONENT_LOGGER_ID, "Spawning UDP receiver thread\n");
    if (thread_handle_create(&state->udp_service.thread,
                             (thread_routine_t)receiver_service_start,
                             &state->udp_service) != 0) {
      log_critical(RECEIVER_COMPONENT_LOGGER_ID,
                   "Spawning UDP receiver thread failed\n");
      return false;
    }
  }
  return true;
}

bool receiver_stop(receiver_state_t *const state) {
  bool ret = true;

  if (state == NULL) {
    return false;
  }
  state->running = false;
  log_info(RECEIVER_COMPONENT_LOGGER_ID, "Shutting down TCP receiver thread");
  if (receiver_service_stop(&state->tcp_service) == false ||
      thread_handle_join(state->tcp_service.thread, NULL) != 0) {
    log_error(RECEIVER_COMPONENT_LOGGER_ID,
              "Shutting down TCP receiver thread failed");
    ret = false;
  }
  log_info(RECEIVER_COMPONENT_LOGGER_ID, "Shutting down UDP receiver thread");
  if (receiver_service_stop(&state->udp_service) == false ||
      thread_handle_join(state->udp_service.thread, NULL) != 0) {
    log_error(RECEIVER_COMPONENT_LOGGER_ID,
              "Shutting down UDP receiver thread failed");
    ret = false;
  }
  return ret;
}

bool receiver_destroy(receiver_state_t *const state) {
  if (state == NULL) {
    return false;
  }
  if (state->running) {
    return false;
  }
  logger_helper_destroy(RECEIVER_COMPONENT_LOGGER_ID);
  return true;
}
