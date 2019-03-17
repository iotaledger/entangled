/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "gossip/node.h"
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
    return RC_NODE_NULL_NODE;
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

static retcode_t node_transaction_requester_init(node_t* const node, tangle_t* const tangle) {
  retcode_t ret = RC_OK;
  iota_stor_pack_t pack;

  if ((ret = requester_init(&node->transaction_requester, node)) != RC_OK) {
    return ret;
  }

  if ((ret = hash_pack_init(&pack, node->conf.requester_queue_size)) != RC_OK) {
    log_error(logger_id, "Initializing request hash pack failed\n");
    goto done;
  }

  if ((ret = iota_tangle_transaction_load_hashes_of_requests(tangle, &pack, node->conf.requester_queue_size)) !=
      RC_OK) {
    log_error(logger_id, "Loading hashes of transactions to request failed\n");
    goto done;
  }

  for (size_t i = 0; i < pack.num_loaded; i++) {
    if ((ret = request_transaction(&node->transaction_requester, tangle, ((flex_trit_t*)(pack.models[i])), false)) !=
        RC_OK) {
      log_error(logger_id, "Requesting transaction failed\n");
      goto done;
    }
  }

  log_info(logger_id, "Added %d transactions to request\n", requester_size(&node->transaction_requester));

done:
  hash_pack_free(&pack);
  return ret;
}

static retcode_t node_tips_cache_init(node_t* const node, tangle_t* const tangle) {
  retcode_t ret = RC_OK;
  iota_stor_pack_t pack;

  if (node == NULL) {
    return RC_NODE_NULL_NODE;
  }

  if ((ret = tips_cache_init(&node->tips, node->conf.tips_cache_size)) != RC_OK) {
    return ret;
  }

  if ((ret = hash_pack_init(&pack, node->conf.tips_cache_size)) != RC_OK) {
    log_error(logger_id, "Initializing tips pack failed\n");
    goto done;
  }

  if ((ret = iota_tangle_transaction_load_hashes_of_tips(tangle, &pack, node->conf.tips_cache_size)) != RC_OK) {
    log_error(logger_id, "Loading hashes of tips failed\n");
    goto done;
  }

  for (size_t i = 0; i < pack.num_loaded; i++) {
    if ((ret = tips_cache_add(&node->tips, ((flex_trit_t*)(pack.models[i])))) != RC_OK) {
      log_error(logger_id, "Adding tip to cache failed\n");
      goto done;
    }
  }

  log_info(logger_id, "Added %d tips to cache\n", tips_cache_size(&node->tips));

done:
  hash_pack_free(&pack);
  return ret;
}

/*
 * Public functions
 */

retcode_t node_init(node_t* const node, core_t* const core, tangle_t* const tangle) {
  retcode_t ret = RC_OK;

  if (node == NULL) {
    return RC_NODE_NULL_NODE;
  } else if (core == NULL) {
    return RC_NODE_NULL_CORE;
  }

  logger_id = logger_helper_enable(NODE_LOGGER_ID, LOGGER_DEBUG, true);
  node->running = false;
  node->core = core;

  log_info(logger_id, "Initializing neighbors\n");
  if (node_neighbors_init(node) != RC_OK) {
    log_info(logger_id, "Initializing neighbors failed\n");
    return RC_NODE_FAILED_NEIGHBORS_INIT;
  }

  log_info(logger_id, "Initializing broadcaster component\n");
  if (broadcaster_init(&node->broadcaster, node) != RC_OK) {
    log_critical(logger_id, "Initializing broadcaster component failed\n");
    return RC_NODE_FAILED_BROADCASTER_INIT;
  }

  log_info(logger_id, "Initializing processor component\n");
  if (processor_init(&node->processor, node, &core->consensus.transaction_validator,
                     &core->consensus.transaction_solidifier, &core->consensus.milestone_tracker) != RC_OK) {
    log_critical(logger_id, "Initializing processor component failed\n");
    return RC_NODE_FAILED_PROCESSOR_INIT;
  }

  log_info(logger_id, "Initializing receiver component\n");
  if (receiver_init(&node->receiver, node, node->conf.tcp_receiver_port, node->conf.udp_receiver_port) != RC_OK) {
    log_critical(logger_id, "Initializing receiver component failed\n");
    return RC_NODE_FAILED_RECEIVER_INIT;
  }

  log_info(logger_id, "Initializing responder component\n");
  if (responder_init(&node->responder, node) != RC_OK) {
    log_critical(logger_id, "Initializing responder component failed\n");
    return RC_NODE_FAILED_RESPONDER_INIT;
  }

  log_info(logger_id, "Initializing tips requester component\n");
  if ((ret = tips_requester_init(&node->tips_requester, node)) != RC_OK) {
    log_critical(logger_id, "Initializing  tips requester component failed\n");
    return ret;
  }

  log_info(logger_id, "Initializing transaction requester component\n");
  if (node_transaction_requester_init(node, tangle) != RC_OK) {
    log_critical(logger_id, "Initializing transaction requester component failed\n");
    return RC_NODE_FAILED_REQUESTER_INIT;
  }

  log_info(logger_id, "Initializing tips solidifier component\n");
  if ((ret = tips_solidifier_init(&node->tips_solidifier, &node->conf, &node->tips,
                                  &core->consensus.transaction_solidifier)) != RC_OK) {
    log_critical(logger_id, "Initializing  tips solidifier component failed\n");
    return ret;
  }

  log_info(logger_id, "Initializing tips cache\n");
  if ((ret = node_tips_cache_init(node, tangle)) != RC_OK) {
    log_error(logger_id, "Initializing tips cache failed\n");
    return ret;
  }

  return ret;
}

retcode_t node_start(node_t* const node) {
  retcode_t ret = RC_OK;

  if (node == NULL) {
    return RC_NODE_NULL_NODE;
  }

  log_info(logger_id, "Starting broadcaster component\n");
  if (broadcaster_start(&node->broadcaster) != RC_OK) {
    log_critical(logger_id, "Starting broadcaster component failed\n");
    return RC_NODE_FAILED_BROADCASTER_START;
  }

  log_info(logger_id, "Starting processor component\n");
  if (processor_start(&node->processor) != RC_OK) {
    log_critical(logger_id, "Starting processor component failed\n");
    return RC_NODE_FAILED_PROCESSOR_START;
  }

  log_info(logger_id, "Starting receiver component\n");
  if (receiver_start(&node->receiver) != RC_OK) {
    log_critical(logger_id, "Starting receiver component failed\n");
    return RC_NODE_FAILED_RECEIVER_START;
  }

  log_info(logger_id, "Starting responder component\n");
  if (responder_start(&node->responder) != RC_OK) {
    log_critical(logger_id, "Starting responder component failed\n");
    return RC_NODE_FAILED_RESPONDER_START;
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

  log_info(logger_id, "Starting tips solidifier component\n");
  if ((ret = tips_solidifier_start(&node->tips_solidifier)) != RC_OK) {
    log_critical(logger_id, "Starting tips solidifier component failed\n");
    return ret;
  }

  node->running = true;

  return ret;
}

retcode_t node_stop(node_t* const node) {
  retcode_t ret = RC_OK;

  if (node == NULL) {
    return RC_NODE_NULL_NODE;
  } else if (node->running == false) {
    return RC_OK;
  }

  node->running = false;

  log_info(logger_id, "Stopping broadcaster component\n");
  if (broadcaster_stop(&node->broadcaster) != RC_OK) {
    log_error(logger_id, "Stopping broadcaster component failed\n");
    ret = RC_NODE_FAILED_BROADCASTER_STOP;
  }

  log_info(logger_id, "Stopping processor component\n");
  if (processor_stop(&node->processor) != RC_OK) {
    log_error(logger_id, "Stopping processor component failed\n");
    ret = RC_NODE_FAILED_PROCESSOR_STOP;
  }

  log_info(logger_id, "Stopping responder component\n");
  if (responder_stop(&node->responder) != RC_OK) {
    log_error(logger_id, "Stopping responder component failed\n");
    ret = RC_NODE_FAILED_RESPONDER_STOP;
  }

  log_info(logger_id, "Stopping receiver component\n");
  if (receiver_stop(&node->receiver) != RC_OK) {
    log_error(logger_id, "Stopping receiver component failed\n");
    ret = RC_NODE_FAILED_RECEIVER_STOP;
  }

  log_info(logger_id, "Stopping tips requester component\n");
  if ((ret = tips_requester_stop(&node->tips_requester)) != RC_OK) {
    log_error(logger_id, "Stopping tips requester component failed\n");
  }

  log_info(logger_id, "Stopping transaction requester component\n");
  if ((ret = requester_stop(&node->transaction_requester)) != RC_OK) {
    log_error(logger_id, "Stopping transaction requester component failed\n");
  }

  log_info(logger_id, "Stopping tips solidifier component\n");
  if ((ret = tips_solidifier_stop(&node->tips_solidifier)) != RC_OK) {
    log_error(logger_id, "Stopping tips solidifier component failed\n");
  }

  return ret;
}

retcode_t node_destroy(node_t* const node) {
  retcode_t ret = RC_OK;

  if (node == NULL) {
    return RC_NODE_NULL_NODE;
  } else if (node->running) {
    return RC_NODE_STILL_RUNNING;
  }

  log_info(logger_id, "Destroying transaction requester component\n");
  if (requester_destroy(&node->transaction_requester) != RC_OK) {
    log_error(logger_id, "Destroying transaction requester component failed\n");
    ret = RC_NODE_FAILED_REQUESTER_DESTROY;
  }

  log_info(logger_id, "Destroying tips solidifier component\n");
  if ((ret = tips_solidifier_destroy(&node->tips_solidifier)) != RC_OK) {
    log_error(logger_id, "Destroying tips solidifier component failed\n");
  }

  log_info(logger_id, "Destroying tips requester component\n");
  if ((ret = tips_requester_destroy(&node->tips_requester)) != RC_OK) {
    log_error(logger_id, "Destroying tips requester component failed\n");
  }

  log_info(logger_id, "Destroying broadcaster component\n");
  if (broadcaster_destroy(&node->broadcaster) != RC_OK) {
    log_error(logger_id, "Destroying broadcaster component failed\n");
    ret = RC_NODE_FAILED_BROADCASTER_DESTROY;
  }

  log_info(logger_id, "Destroying receiver component\n");
  if (receiver_destroy(&node->receiver) != RC_OK) {
    log_error(logger_id, "Destroying receiver component failed\n");
    ret = RC_NODE_FAILED_RECEIVER_DESTROY;
  }

  log_info(logger_id, "Destroying processor component\n");
  if (processor_destroy(&node->processor) != RC_OK) {
    log_error(logger_id, "Destroying processor component failed\n");
    ret = RC_NODE_FAILED_PROCESSOR_DESTROY;
  }

  log_info(logger_id, "Destroying responder component\n");
  if (responder_destroy(&node->responder) != RC_OK) {
    log_error(logger_id, "Destroying responder component failed\n");
    ret = RC_NODE_FAILED_RESPONDER_DESTROY;
  }

  log_debug(logger_id, "Destroying neighbors\n");
  rw_lock_handle_wrlock(&node->neighbors_lock);
  neighbors_free(&node->neighbors);
  rw_lock_handle_unlock(&node->neighbors_lock);
  rw_lock_handle_destroy(&node->neighbors_lock);

  tips_cache_destroy(&node->tips);
  free(node->conf.neighbors);

  logger_helper_release(logger_id);

  return ret;
}
