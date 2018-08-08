/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "common/network/components/processor.h"
#include "common/logger_helper.h"

static char const processor_component_logger[] = "processor_component";

static void *processor_routine(processor_state_t *const state) {
  iota_packet_t packet;

  if (state == NULL) {
    return NULL;
  }
  while (state->running) {
    if (state->queue->vtable->pop(state->queue, &packet) ==
        CONCURRENT_QUEUE_SUCCESS) {
      log_debug(processor_component_logger, "Processing packet\n");
      // TODO(thibault) process the hash
    }
  }
  return NULL;
}

bool processor_init(processor_state_t *const state, node_t *const node) {
  if (state == NULL || node == NULL) {
    return false;
  }
  logger_helper_init(processor_component_logger, LOGGER_DEBUG, true);
  state->running = false;
  if (INIT_CONCURRENT_QUEUE_OF(iota_packet_t, state->queue) !=
      CONCURRENT_QUEUE_SUCCESS) {
    log_critical(processor_component_logger,
                 "Initializing processor queue failed\n");
    return false;
  }
  state->node = node;
  return true;
}

bool processor_start(processor_state_t *const state) {
  if (state == NULL) {
    return false;
  }
  log_info(processor_component_logger, "Spawning processor thread\n");
  state->running = true;
  if (thread_handle_create(&state->thread, (thread_routine_t)processor_routine,
                           state) != 0) {
    log_critical(processor_component_logger,
                 "Spawning processor thread failed\n");
    return false;
  }
  return true;
}

bool processor_on_next(processor_state_t *const state,
                       iota_packet_t const packet) {
  if (state == NULL) {
    return false;
  }
  if (state->queue->vtable->push(state->queue, packet) !=
      CONCURRENT_QUEUE_SUCCESS) {
    log_warning(processor_component_logger,
                "Pushing to processor queue failed\n");
    return false;
  }
  return true;
}

bool processor_stop(processor_state_t *const state) {
  bool ret = true;

  if (state == NULL) {
    return false;
  }
  log_info(processor_component_logger, "Shutting down processor thread\n");
  state->running = false;
  if (thread_handle_join(state->thread, NULL) != 0) {
    log_error(processor_component_logger,
              "Shutting down processor thread failed\n");
    ret = false;
  }
  return ret;
}

bool processor_destroy(processor_state_t *const state) {
  bool ret = true;

  if (state == NULL) {
    return false;
  }
  if (DESTROY_CONCURRENT_QUEUE_OF(iota_packet_t, state->queue) !=
      CONCURRENT_QUEUE_SUCCESS) {
    log_error(processor_component_logger,
              "Destroying processor queue failed\n");
    ret = false;
  }
  return ret;
}
