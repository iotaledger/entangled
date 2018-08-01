/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "ciri/node.h"
#include "common/network/logger.h"

// TODO(thibault) configuration variable
static uint16_t tcp_port_g = 14265;
static uint16_t udp_port_g = 14265;

bool node_init(node_t* const node) {
  if (INIT_CONCURRENT_LIST_OF(neighbor_t, node->neighbors, neighbor_cmp) !=
      CONCURRENT_QUEUE_SUCCESS) {
    return false;
  }
  log_info("Initializing broadcaster component");
  if (broadcaster_init(&node->broadcaster, node) == false) {
    log_fatal("Initializing broadcaster component failed");
    return false;
  }
  log_info("Initializing processor component");
  if (processor_init(&node->processor, node) == false) {
    log_fatal("Initializing processor component failed");
    return false;
  }
  log_info("Initializing receiver component");
  if (receiver_init(&node->receiver, node, tcp_port_g, udp_port_g) == false) {
    log_fatal("Initializing receiver component failed");
    return false;
  }
  log_info("Initializing requester component");
  if (requester_init(&node->requester, node) == false) {
    log_fatal("Initializing requester component failed");
    return false;
  }
  log_info("Initializing responder component");
  if (responder_init(&node->responder, node) == false) {
    log_fatal("Initializing responder component failed");
    return false;
  }
  return true;
}

bool node_start(node_t* const node) {
  log_info("Starting broadcaster component");
  if (broadcaster_start(&node->broadcaster) == false) {
    log_fatal("Starting broadcaster component failed");
    return false;
  }
  log_info("Starting processor component");
  if (processor_start(&node->processor) == false) {
    log_fatal("Starting processor component failed");
    return false;
  }
  log_info("Starting receiver component");
  if (receiver_start(&node->receiver) == false) {
    log_fatal("Starting receiver component failed");
    return false;
  }
  log_info("Starting responder component");
  if (responder_start(&node->responder) == false) {
    log_fatal("Starting responder component failed");
    return false;
  }
  return true;
}

bool node_stop(node_t* const node) {
  bool ret = true;

  log_info("Stopping broadcaster component");
  if (broadcaster_stop(&node->broadcaster) == false) {
    log_error("Stopping broadcaster component failed");
    ret = false;
  }
  log_info("Stopping processor component");
  if (processor_stop(&node->processor) == false) {
    log_error("Stopping processor component failed");
    ret = false;
  }
  log_info("Stopping receiver component");
  if (receiver_stop(&node->receiver) == false) {
    log_error("Stopping receiver component failed");
    ret = false;
  }
  log_info("Stopping responder component");
  if (responder_stop(&node->responder) == false) {
    log_error("Stopping responder component failed");
    ret = false;
  }
  return ret;
}

bool node_destroy(node_t* const node) {
  bool ret = true;

  log_info("Destroying broadcaster component");
  if (broadcaster_destroy(&node->broadcaster) == false) {
    log_error("Destroying broadcaster component failed");
    ret = false;
  }
  log_info("Destroying processor component");
  if (processor_destroy(&node->processor) == false) {
    log_error("Destroying processor component failed");
    ret = false;
  }
  log_info("Destroying processor component");
  if (requester_destroy(&node->requester) == false) {
    log_error("Destroying processor component failed");
    ret = false;
  }
  log_info("Destroying responder component");
  if (responder_destroy(&node->responder) == false) {
    log_error("Destroying responder component failed");
    ret = false;
  }
  if (DESTROY_CONCURRENT_LIST_OF(neighbor_t, node->neighbors) !=
      CONCURRENT_LIST_SUCCESS) {
    ret = false;
  }
  return ret;
}
