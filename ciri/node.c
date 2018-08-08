/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "ciri/node.h"
#include "common/logger_helper.h"

// TODO(thibault) configuration variable
static uint16_t tcp_port_g = 14260;
static uint16_t udp_port_g = 14261;
static char const node_logger[] = "node";

bool node_init(node_t* const node) {
  if (node == NULL) {
    return false;
  }
  logger_helper_init(node_logger, LOGGER_DEBUG, true);
  log_info(node_logger, "Initializing neighbors list\n");
  if (INIT_CONCURRENT_LIST_OF(neighbor_t, node->neighbors, neighbor_cmp) !=
      CONCURRENT_QUEUE_SUCCESS) {
    log_critical(node_logger, "Initializing neighbors list failed\n");
    return false;
  }
  log_info(node_logger, "Initializing broadcaster component\n");
  if (broadcaster_init(&node->broadcaster, node) == false) {
    log_critical(node_logger, "Initializing broadcaster component failed\n");
    return false;
  }
  log_info(node_logger, "Initializing processor component\n");
  if (processor_init(&node->processor, node) == false) {
    log_critical(node_logger, "Initializing processor component failed\n");
    return false;
  }
  log_info(node_logger, "Initializing receiver component\n");
  if (receiver_init(&node->receiver, node, tcp_port_g, udp_port_g) == false) {
    log_critical(node_logger, "Initializing receiver component failed\n");
    return false;
  }
  log_info(node_logger, "Initializing requester component\n");
  if (requester_init(&node->requester, node) == false) {
    log_critical(node_logger, "Initializing requester component failed\n");
    return false;
  }
  log_info(node_logger, "Initializing responder component\n");
  if (responder_init(&node->responder, node) == false) {
    log_critical(node_logger, "Initializing responder component failed\n");
    return false;
  }
  return true;
}

bool node_start(node_t* const node) {
  if (node == NULL) {
    return false;
  }
  log_info(node_logger, "Starting broadcaster component\n");
  if (broadcaster_start(&node->broadcaster) == false) {
    log_critical(node_logger, "Starting broadcaster component failed\n");
    return false;
  }
  log_info(node_logger, "Starting processor component\n");
  if (processor_start(&node->processor) == false) {
    log_critical(node_logger, "Starting processor component failed\n");
    return false;
  }
  log_info(node_logger, "Starting receiver component\n");
  if (receiver_start(&node->receiver) == false) {
    log_critical(node_logger, "Starting receiver component failed\n");
    return false;
  }
  log_info(node_logger, "Starting responder component\n");
  if (responder_start(&node->responder) == false) {
    log_critical(node_logger, "Starting responder component failed\n");
    return false;
  }
  return true;
}

bool node_stop(node_t* const node) {
  bool ret = true;

  if (node == NULL) {
    return false;
  }
  log_info(node_logger, "Stopping broadcaster component\n");
  if (broadcaster_stop(&node->broadcaster) == false) {
    log_error(node_logger, "Stopping broadcaster component failed\n");
    ret = false;
  }
  log_info(node_logger, "Stopping processor component\n");
  if (processor_stop(&node->processor) == false) {
    log_error(node_logger, "Stopping processor component failed\n");
    ret = false;
  }
  log_info(node_logger, "Stopping receiver component\n");
  if (receiver_stop(&node->receiver) == false) {
    log_error(node_logger, "Stopping receiver component failed\n");
    ret = false;
  }
  log_info(node_logger, "Stopping responder component\n");
  if (responder_stop(&node->responder) == false) {
    log_error(node_logger, "Stopping responder component failed\n");
    ret = false;
  }
  return ret;
}

bool node_destroy(node_t* const node) {
  bool ret = true;

  if (node == NULL) {
    return false;
  }
  log_info(node_logger, "Destroying broadcaster component\n");
  if (broadcaster_destroy(&node->broadcaster) == false) {
    log_error(node_logger, "Destroying broadcaster component failed\n");
    ret = false;
  }
  log_info(node_logger, "Destroying processor component\n");
  if (processor_destroy(&node->processor) == false) {
    log_error(node_logger, "Destroying processor component failed\n");
    ret = false;
  }
  log_info(node_logger, "Destroying processor component\n");
  if (requester_destroy(&node->requester) == false) {
    log_error(node_logger, "Destroying processor component failed\n");
    ret = false;
  }
  log_info(node_logger, "Destroying responder component\n");
  if (responder_destroy(&node->responder) == false) {
    log_error(node_logger, "Destroying responder component failed\n");
    ret = false;
  }
  log_info(node_logger, "Destroying neighbors list\n");
  if (DESTROY_CONCURRENT_LIST_OF(neighbor_t, node->neighbors) !=
      CONCURRENT_LIST_SUCCESS) {
    log_error(node_logger, "Destroying neighbors list failed\n");
    ret = false;
  }
  logger_helper_destroy(node_logger);
  return ret;
}
