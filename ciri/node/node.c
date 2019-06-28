/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "ciri/node/node.h"
#include "ciri/core.h"
#include "utils/logger_helper.h"

#define NODE_LOGGER_ID "node"

static logger_id_t logger_id;

/*
 * Private functions
 */

static retcode_t node_neighbors_init(node_t* const node) {
  neighbor_t neighbor;
  char *neighbor_uri, *cpy, *ptr;

  if (node == NULL) {
    return RC_NULL_PARAM;
  } else if (node->conf.neighbors == NULL) {
    return RC_OK;
  }

  node->neighbors = NULL;
  rw_lock_handle_init(&node->neighbors_lock);

  ptr = cpy = strdup(node->conf.neighbors);
  while ((neighbor_uri = strsep(&cpy, " ")) != NULL) {
    if (neighbor_init_with_uri(&neighbor, neighbor_uri) != RC_OK) {
      log_warning(logger_id, "Initializing neighbor with URI %s failed\n", neighbor_uri);
      continue;
    }
    log_info(logger_id, "Adding neighbor %s\n", neighbor_uri);
    rw_lock_handle_wrlock(&node->neighbors_lock);
    if (neighbors_add(&node->neighbors, &neighbor) != RC_OK) {
      log_warning(logger_id, "Adding neighbor %s failed\n", neighbor_uri);
    }
    rw_lock_handle_unlock(&node->neighbors_lock);
  }

  free(ptr);
  return RC_OK;
}

/*
 * Public functions
 */

retcode_t node_init(node_t* const node, core_t* const core, tangle_t* const tangle) {
  retcode_t ret = RC_OK;

  if (node == NULL || core == NULL || tangle == NULL) {
    return RC_NULL_PARAM;
  }

  logger_id = logger_helper_enable(NODE_LOGGER_ID, LOGGER_DEBUG, true);
  node->running = false;
  node->core = core;

  log_info(logger_id, "Initializing neighbors\n");
  if ((ret = node_neighbors_init(node)) != RC_OK) {
    log_info(logger_id, "Initializing neighbors failed\n");
    return ret;
  }

  log_info(logger_id, "Initializing broadcaster component\n");
  if ((ret = broadcaster_init(&node->broadcaster, node)) != RC_OK) {
    log_critical(logger_id, "Initializing broadcaster component failed\n");
    return ret;
  }

  log_info(logger_id, "Initializing processor component\n");
  if ((ret = processor_init(&node->processor, node, &core->consensus.transaction_validator,
                            &core->consensus.transaction_solidifier, &core->consensus.milestone_tracker)) != RC_OK) {
    log_critical(logger_id, "Initializing processor component failed\n");
    return ret;
  }

  log_info(logger_id, "Initializing receiver component\n");
  if ((ret = receiver_init(&node->receiver, node, node->conf.tcp_receiver_port, node->conf.udp_receiver_port)) !=
      RC_OK) {
    log_critical(logger_id, "Initializing receiver component failed\n");
    return ret;
  }

  log_info(logger_id, "Initializing responder component\n");
  if ((ret = responder_init(&node->responder, node)) != RC_OK) {
    log_critical(logger_id, "Initializing responder component failed\n");
    return ret;
  }

  log_info(logger_id, "Initializing tips requester component\n");
  if ((ret = tips_requester_init(&node->tips_requester, node)) != RC_OK) {
    log_critical(logger_id, "Initializing  tips requester component failed\n");
    return ret;
  }

  log_info(logger_id, "Initializing transaction requester component\n");
  if ((ret = requester_init(&node->transaction_requester, node)) != RC_OK) {
    log_critical(logger_id, "Initializing transaction requester component failed\n");
    return ret;
  }

  if (node->conf.tips_solidifier_enabled) {
    log_info(logger_id, "Initializing tips solidifier component\n");
    if ((ret = tips_solidifier_init(&node->tips_solidifier, &node->conf, &node->tips,
                                    &core->consensus.transaction_solidifier)) != RC_OK) {
      log_critical(logger_id, "Initializing  tips solidifier component failed\n");
      return ret;
    }
  }

  log_info(logger_id, "Initializing tips cache\n");
  if ((ret = tips_cache_init(&node->tips, node->conf.tips_cache_size)) != RC_OK) {
    log_error(logger_id, "Initializing tips cache failed\n");
    return ret;
  }

  log_info(logger_id, "Initializing recent seen bytes cache\n");
  if ((ret = recent_seen_bytes_cache_init(&node->recent_seen_bytes, node->conf.recent_seen_bytes_cache_size)) !=
      RC_OK) {
    log_error(logger_id, "Initializing recent seen bytes cache failed\n");
    return ret;
  }

  return ret;
}

retcode_t node_start(node_t* const node) {
  retcode_t ret = RC_OK;

  if (node == NULL) {
    return RC_NULL_PARAM;
  }

  log_info(logger_id, "Starting broadcaster component\n");
  if ((ret = broadcaster_start(&node->broadcaster)) != RC_OK) {
    log_critical(logger_id, "Starting broadcaster component failed\n");
    return ret;
  }

  log_info(logger_id, "Starting processor component\n");
  if ((ret = processor_start(&node->processor)) != RC_OK) {
    log_critical(logger_id, "Starting processor component failed\n");
    return ret;
  }

  log_info(logger_id, "Starting receiver component\n");
  if ((ret = receiver_start(&node->receiver)) != RC_OK) {
    log_critical(logger_id, "Starting receiver component failed\n");
    return ret;
  }

  log_info(logger_id, "Starting responder component\n");
  if ((ret = responder_start(&node->responder)) != RC_OK) {
    log_critical(logger_id, "Starting responder component failed\n");
    return ret;
  }

  log_info(logger_id, "Starting tips requester component\n");
  if ((ret = tips_requester_start(&node->tips_requester)) != RC_OK) {
    log_critical(logger_id, "Starting tips requester component failed\n");
    return ret;
  }

  log_info(logger_id, "Starting transaction requester component\n");
  if ((ret = requester_start(&node->transaction_requester)) != RC_OK) {
    log_critical(logger_id, "Starting transaction requester component failed\n");
    return ret;
  }

  if (node->conf.tips_solidifier_enabled) {
    log_info(logger_id, "Starting tips solidifier component\n");
    if ((ret = tips_solidifier_start(&node->tips_solidifier)) != RC_OK) {
      log_critical(logger_id, "Starting tips solidifier component failed\n");
      return ret;
    }
  }

  node->running = true;

  return ret;
}

retcode_t node_stop(node_t* const node) {
  retcode_t ret = RC_OK;

  if (node == NULL) {
    return RC_NULL_PARAM;
  } else if (node->running == false) {
    return RC_OK;
  }

  node->running = false;

  log_info(logger_id, "Stopping broadcaster component\n");
  if ((ret = broadcaster_stop(&node->broadcaster)) != RC_OK) {
    log_error(logger_id, "Stopping broadcaster component failed\n");
  }

  log_info(logger_id, "Stopping processor component\n");
  if ((ret = processor_stop(&node->processor)) != RC_OK) {
    log_error(logger_id, "Stopping processor component failed\n");
  }

  log_info(logger_id, "Stopping responder component\n");
  if ((ret = responder_stop(&node->responder)) != RC_OK) {
    log_error(logger_id, "Stopping responder component failed\n");
  }

  log_info(logger_id, "Stopping receiver component\n");
  if ((ret = receiver_stop(&node->receiver)) != RC_OK) {
    log_error(logger_id, "Stopping receiver component failed\n");
  }

  log_info(logger_id, "Stopping tips requester component\n");
  if ((ret = tips_requester_stop(&node->tips_requester)) != RC_OK) {
    log_error(logger_id, "Stopping tips requester component failed\n");
  }

  log_info(logger_id, "Stopping transaction requester component\n");
  if ((ret = requester_stop(&node->transaction_requester)) != RC_OK) {
    log_error(logger_id, "Stopping transaction requester component failed\n");
  }

  if (node->conf.tips_solidifier_enabled) {
    log_info(logger_id, "Stopping tips solidifier component\n");
    if ((ret = tips_solidifier_stop(&node->tips_solidifier)) != RC_OK) {
      log_error(logger_id, "Stopping tips solidifier component failed\n");
    }
  }

  return ret;
}

retcode_t node_destroy(node_t* const node) {
  retcode_t ret = RC_OK;

  if (node == NULL) {
    return RC_NULL_PARAM;
  } else if (node->running) {
    return RC_STILL_RUNNING;
  }

  log_info(logger_id, "Destroying transaction requester component\n");
  if ((ret = requester_destroy(&node->transaction_requester)) != RC_OK) {
    log_error(logger_id, "Destroying transaction requester component failed\n");
  }

  if (node->conf.tips_solidifier_enabled) {
    log_info(logger_id, "Destroying tips solidifier component\n");
    if ((ret = tips_solidifier_destroy(&node->tips_solidifier)) != RC_OK) {
      log_error(logger_id, "Destroying tips solidifier component failed\n");
    }
  }

  log_info(logger_id, "Destroying tips requester component\n");
  if ((ret = tips_requester_destroy(&node->tips_requester)) != RC_OK) {
    log_error(logger_id, "Destroying tips requester component failed\n");
  }

  log_info(logger_id, "Destroying broadcaster component\n");
  if ((ret = broadcaster_destroy(&node->broadcaster)) != RC_OK) {
    log_error(logger_id, "Destroying broadcaster component failed\n");
  }

  log_info(logger_id, "Destroying receiver component\n");
  if ((ret = receiver_destroy(&node->receiver)) != RC_OK) {
    log_error(logger_id, "Destroying receiver component failed\n");
  }

  log_info(logger_id, "Destroying processor component\n");
  if ((ret = processor_destroy(&node->processor)) != RC_OK) {
    log_error(logger_id, "Destroying processor component failed\n");
  }

  log_info(logger_id, "Destroying responder component\n");
  if ((ret = responder_destroy(&node->responder)) != RC_OK) {
    log_error(logger_id, "Destroying responder component failed\n");
  }

  log_debug(logger_id, "Destroying neighbors\n");
  rw_lock_handle_wrlock(&node->neighbors_lock);
  neighbors_free(&node->neighbors);
  rw_lock_handle_unlock(&node->neighbors_lock);
  rw_lock_handle_destroy(&node->neighbors_lock);

  tips_cache_destroy(&node->tips);
  recent_seen_bytes_cache_destroy(&node->recent_seen_bytes);
  free(node->conf.neighbors);

  logger_helper_release(logger_id);

  return ret;
}
