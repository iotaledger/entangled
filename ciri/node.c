/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "ciri/node.h"
#include "common/network/logger.h"

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
  log_info("Starting requester component");
  if (requester_start(&node->requester) == false) {
    log_fatal("Starting requester component failed");
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
  log_info("Stopping processor component");
  if (requester_stop(&node->requester) == false) {
    log_error("Stopping processor component failed");
    ret = false;
  }
  log_info("Stopping responder component");
  if (responder_stop(&node->responder) == false) {
    log_error("Stopping responder component failed");
    ret = false;
  }
  return ret;
}
