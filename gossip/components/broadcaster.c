/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <string.h>

#include "ciri/node.h"
#include "gossip/components/broadcaster.h"
#include "utils/containers/lists/concurrent_list_neighbor.h"
#include "utils/containers/queues/concurrent_queue_packet.h"
#include "utils/logger_helper.h"

#define BROADCASTER_COMPONENT_LOGGER_ID "broadcaster_component"

static void *broadcaster_routine(broadcaster_state_t *const state) {
  iota_packet_t packet;
  concurrent_list_node_neighbor_t *iter;

  if (state == NULL) {
    return NULL;
  }

  while (state->running) {
    if (CQ_POP(state->queue, &packet) == CQ_SUCCESS) {
      log_debug(BROADCASTER_COMPONENT_LOGGER_ID, "Broadcasting transaction\n");
      if (state->node->neighbors) {
        iter = state->node->neighbors->front;
        while (iter) {
          if (neighbor_send(state->node, &iter->data, &packet)) {
            log_warning(BROADCASTER_COMPONENT_LOGGER_ID,
                        "Broadcasting transaction failed\n");
          }
          iter = iter->next;
        }
      }
    }
  }
  return NULL;
}

retcode_t broadcaster_init(broadcaster_state_t *const state,
                           node_t *const node) {
  if (state == NULL) {
    return RC_BROADCASTER_COMPONENT_NULL_STATE;
  } else if (node == NULL) {
    return RC_BROADCASTER_COMPONENT_NULL_NODE;
  }

  logger_helper_init(BROADCASTER_COMPONENT_LOGGER_ID, LOGGER_DEBUG, true);
  memset(state, 0, sizeof(broadcaster_state_t));
  state->running = false;
  state->node = node;

  log_debug(BROADCASTER_COMPONENT_LOGGER_ID,
            "Initializing broadcaster queue\n");
  if (CQ_INIT(iota_packet_t, state->queue) != CQ_SUCCESS) {
    log_critical(BROADCASTER_COMPONENT_LOGGER_ID,
                 "Initializing broadcaster queue failed\n");
    return RC_BROADCASTER_COMPONENT_FAILED_INIT_QUEUE;
  }
  return RC_OK;
}

retcode_t broadcaster_start(broadcaster_state_t *const state) {
  if (state == NULL) {
    return RC_BROADCASTER_COMPONENT_NULL_STATE;
  }

  log_info(BROADCASTER_COMPONENT_LOGGER_ID, "Spawning broadcaster thread\n");
  state->running = true;
  if (thread_handle_create(&state->thread,
                           (thread_routine_t)broadcaster_routine, state) != 0) {
    log_critical(BROADCASTER_COMPONENT_LOGGER_ID,
                 "Spawning broadcaster thread failed\n");
    return RC_BROADCASTER_COMPONENT_FAILED_THREAD_SPAWN;
  }
  return RC_OK;
}

retcode_t broadcaster_on_next(broadcaster_state_t *const state,
                              iota_packet_t const packet) {
  if (state == NULL) {
    return RC_BROADCASTER_COMPONENT_NULL_STATE;
  }

  if (CQ_PUSH(state->queue, packet) != CQ_SUCCESS) {
    log_warning(BROADCASTER_COMPONENT_LOGGER_ID,
                "Adding packet to broadcaster queue failed\n");
    return RC_BROADCASTER_COMPONENT_FAILED_ADD_QUEUE;
  }
  return RC_OK;
}

retcode_t broadcaster_stop(broadcaster_state_t *const state) {
  retcode_t ret = RC_OK;

  if (state == NULL) {
    return RC_BROADCASTER_COMPONENT_NULL_STATE;
  } else if (state->running == false) {
    return RC_OK;
  }

  log_info(BROADCASTER_COMPONENT_LOGGER_ID,
           "Shutting down broadcaster thread\n");
  state->running = false;
  if (thread_handle_join(state->thread, NULL) != 0) {
    log_error(BROADCASTER_COMPONENT_LOGGER_ID,
              "Shutting down broadcaster thread failed\n");
    ret = RC_BROADCASTER_COMPONENT_FAILED_THREAD_JOIN;
  }
  return ret;
}

retcode_t broadcaster_destroy(broadcaster_state_t *const state) {
  retcode_t ret = RC_OK;

  if (state == NULL) {
    return RC_BROADCASTER_COMPONENT_NULL_STATE;
  } else if (state->running) {
    return RC_BROADCASTER_COMPONENT_STILL_RUNNING;
  }

  log_debug(BROADCASTER_COMPONENT_LOGGER_ID, "Destroying broadcaster queue\n");
  if (CQ_DESTROY(iota_packet_t, state->queue) != CQ_SUCCESS) {
    log_error(BROADCASTER_COMPONENT_LOGGER_ID,
              "Destroying broadcaster queue failed\n");
    ret = RC_BROADCASTER_COMPONENT_FAILED_DESTROY_QUEUE;
  }
  logger_helper_destroy(BROADCASTER_COMPONENT_LOGGER_ID);
  return ret;
}
