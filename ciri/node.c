/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "ciri/node.h"
#include "utils/logger_helper.h"

#define NODE_LOGGER_ID "node"

// TODO(thibault) configuration variable
static uint16_t tcp_port_g = 14260;
static uint16_t udp_port_g = 14261;

bool node_init(node_t* const node, core_t* const core) {
  if (node == NULL) {
    return false;
  }
  logger_helper_init(NODE_LOGGER_ID, LOGGER_DEBUG, true);
  log_info(NODE_LOGGER_ID, "Initializing neighbors list\n");
  if (INIT_CONCURRENT_LIST_OF(neighbor_t, node->neighbors, neighbor_cmp) !=
      CONCURRENT_QUEUE_SUCCESS) {
    log_critical(NODE_LOGGER_ID, "Initializing neighbors list failed\n");
    return false;
  }
  log_info(NODE_LOGGER_ID, "Initializing broadcaster component\n");
  if (broadcaster_init(&node->broadcaster, node) == false) {
    log_critical(NODE_LOGGER_ID, "Initializing broadcaster component failed\n");
    return false;
  }
  log_info(NODE_LOGGER_ID, "Initializing processor component\n");
  if (processor_init(&node->processor, node) == false) {
    log_critical(NODE_LOGGER_ID, "Initializing processor component failed\n");
    return false;
  }
  log_info(NODE_LOGGER_ID, "Initializing receiver component\n");
  if (receiver_init(&node->receiver, node, tcp_port_g, udp_port_g) == false) {
    log_critical(NODE_LOGGER_ID, "Initializing receiver component failed\n");
    return false;
  }
  log_info(NODE_LOGGER_ID, "Initializing requester component\n");
  if (requester_init(&node->requester, node) == false) {
    log_critical(NODE_LOGGER_ID, "Initializing requester component failed\n");
    return false;
  }
  log_info(NODE_LOGGER_ID, "Initializing responder component\n");
  if (responder_init(&node->responder, node) == false) {
    log_critical(NODE_LOGGER_ID, "Initializing responder component failed\n");
    return false;
  }
  node->core = core;
  return true;
}

bool node_start(node_t* const node) {
  if (node == NULL) {
    return false;
  }
  log_info(NODE_LOGGER_ID, "Starting broadcaster component\n");
  if (broadcaster_start(&node->broadcaster) == false) {
    log_critical(NODE_LOGGER_ID, "Starting broadcaster component failed\n");
    return false;
  }
  log_info(NODE_LOGGER_ID, "Starting processor component\n");
  if (processor_start(&node->processor) == false) {
    log_critical(NODE_LOGGER_ID, "Starting processor component failed\n");
    return false;
  }
  log_info(NODE_LOGGER_ID, "Starting receiver component\n");
  if (receiver_start(&node->receiver) == false) {
    log_critical(NODE_LOGGER_ID, "Starting receiver component failed\n");
    return false;
  }
  log_info(NODE_LOGGER_ID, "Starting responder component\n");
  if (responder_start(&node->responder) == false) {
    log_critical(NODE_LOGGER_ID, "Starting responder component failed\n");
    return false;
  }
  return true;
}

bool node_stop(node_t* const node) {
  bool ret = true;

  if (node == NULL) {
    return false;
  }
  log_info(NODE_LOGGER_ID, "Stopping broadcaster component\n");
  if (broadcaster_stop(&node->broadcaster) == false) {
    log_error(NODE_LOGGER_ID, "Stopping broadcaster component failed\n");
    ret = false;
  }
  log_info(NODE_LOGGER_ID, "Stopping processor component\n");
  if (processor_stop(&node->processor) == false) {
    log_error(NODE_LOGGER_ID, "Stopping processor component failed\n");
    ret = false;
  }
  log_info(NODE_LOGGER_ID, "Stopping receiver component\n");
  if (receiver_stop(&node->receiver) == false) {
    log_error(NODE_LOGGER_ID, "Stopping receiver component failed\n");
    ret = false;
  }
  log_info(NODE_LOGGER_ID, "Stopping responder component\n");
  if (responder_stop(&node->responder) == false) {
    log_error(NODE_LOGGER_ID, "Stopping responder component failed\n");
    ret = false;
  }
  return ret;
}

bool node_destroy(node_t* const node) {
  bool ret = true;

  if (node == NULL) {
    return false;
  }
  log_info(NODE_LOGGER_ID, "Destroying broadcaster component\n");
  if (broadcaster_destroy(&node->broadcaster) == false) {
    log_error(NODE_LOGGER_ID, "Destroying broadcaster component failed\n");
    ret = false;
  }
  log_info(NODE_LOGGER_ID, "Destroying processor component\n");
  if (processor_destroy(&node->processor) == false) {
    log_error(NODE_LOGGER_ID, "Destroying processor component failed\n");
    ret = false;
  }
  log_info(NODE_LOGGER_ID, "Destroying processor component\n");
  if (requester_destroy(&node->requester) == false) {
    log_error(NODE_LOGGER_ID, "Destroying processor component failed\n");
    ret = false;
  }
  log_info(NODE_LOGGER_ID, "Destroying responder component\n");
  if (responder_destroy(&node->responder) == false) {
    log_error(NODE_LOGGER_ID, "Destroying responder component failed\n");
    ret = false;
  }
  log_info(NODE_LOGGER_ID, "Destroying neighbors list\n");
  if (DESTROY_CONCURRENT_LIST_OF(neighbor_t, node->neighbors) !=
      CONCURRENT_LIST_SUCCESS) {
    log_error(NODE_LOGGER_ID, "Destroying neighbors list failed\n");
    ret = false;
  }
  logger_helper_destroy(NODE_LOGGER_ID);
  return ret;
}
