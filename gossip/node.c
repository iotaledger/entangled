/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "gossip/node.h"
#include "ciri/core.h"
#include "utils/containers/lists/concurrent_list_neighbor.h"
#include "utils/logger_helper.h"

#define NODE_LOGGER_ID "node"

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

  log_debug(NODE_LOGGER_ID, "Initializing neighbors list\n");
  if (CL_INIT(neighbor_t, node->neighbors, neighbor_cmp) != CL_SUCCESS) {
    log_critical(NODE_LOGGER_ID, "Initializing neighbors list failed\n");
    return RC_NODE_FAILED_NEIGHBORS_INIT;
  }

  ptr = cpy = strdup(node->conf.neighbors);
  while ((neighbor_uri = strsep(&cpy, " ")) != NULL) {
    if (neighbor_init_with_uri(&neighbor, neighbor_uri)) {
      log_warning(NODE_LOGGER_ID, "Initializing neighbor with URI %s failed\n",
                  neighbor_uri);
      continue;
    }
    log_info(NODE_LOGGER_ID, "Adding neighbor %s\n", neighbor_uri);
    if (neighbor_add(node->neighbors, neighbor) == false) {
      log_warning(NODE_LOGGER_ID, "Adding neighbor %s failed\n", neighbor_uri);
    }
  }

  free(ptr);
  return RC_OK;
}

static retcode_t node_transaction_requester_init(node_t* const node,
                                                 tangle_t* const tangle) {
  retcode_t ret = RC_OK;
  iota_stor_pack_t pack;

  if ((ret = requester_init(&node->transaction_requester, &node->conf,
                            tangle)) != RC_OK) {
    return ret;
  }

  if ((ret = hash_pack_init(&pack, node->conf.requester_queue_size)) != RC_OK) {
    log_error(NODE_LOGGER_ID, "Initializing request hash pack failed\n");
    goto done;
  }

  if ((ret = iota_tangle_transaction_load_hashes_of_requests(
           &node->core->consensus.tangle, &pack,
           node->conf.requester_queue_size)) != RC_OK) {
    log_error(NODE_LOGGER_ID,
              "Loading hashes of transactions to request failed\n");
    goto done;
  }

  for (size_t i = 0; i < pack.num_loaded; i++) {
    if ((ret = request_transaction(&node->transaction_requester,
                                   ((trit_array_t*)(pack.models[i]))->trits,
                                   false)) != RC_OK) {
      log_error(NODE_LOGGER_ID, "Requesting transaction failed\n");
      goto done;
    }
  }

  log_debug(NODE_LOGGER_ID, "Added %d transactions to request\n",
            pack.num_loaded);

done:
  hash_pack_free(&pack);
  return ret;
}

static retcode_t node_tips_cache_init(node_t* const node) {
  retcode_t ret = RC_OK;
  iota_stor_pack_t pack;

  if (node == NULL) {
    return RC_NODE_NULL_NODE;
  }

  if ((ret = tips_cache_init(&node->tips, node->conf.tips_cache_size)) !=
      RC_OK) {
    return ret;
  }

  if ((ret = hash_pack_init(&pack, node->conf.tips_cache_size)) != RC_OK) {
    log_error(NODE_LOGGER_ID, "Initializing tips pack failed\n");
    goto done;
  }

  if ((ret = iota_tangle_transaction_load_hashes_of_tips(
           &node->core->consensus.tangle, &pack, node->conf.tips_cache_size)) !=
      RC_OK) {
    log_error(NODE_LOGGER_ID, "Loading hashes of tips failed\n");
    goto done;
  }

  for (size_t i = 0; i < pack.num_loaded; i++) {
    if ((ret = tips_cache_add(
             &node->tips, ((trit_array_t*)(pack.models[i]))->trits)) != RC_OK) {
      log_error(NODE_LOGGER_ID, "Adding tip to cache failed\n");
      goto done;
    }
  }

  log_debug(NODE_LOGGER_ID, "Added %d tips to cache\n", pack.num_loaded);

done:
  hash_pack_free(&pack);
  return ret;
}

/*
 * Public functions
 */

retcode_t node_init(node_t* const node, core_t* const core,
                    tangle_t* const tangle) {
  retcode_t ret = RC_OK;

  if (node == NULL) {
    return RC_NODE_NULL_NODE;
  } else if (core == NULL) {
    return RC_NODE_NULL_CORE;
  }

  logger_helper_init(NODE_LOGGER_ID, LOGGER_DEBUG, true);
  node->running = false;
  node->core = core;

  log_info(NODE_LOGGER_ID, "Initializing neighbors\n");
  if (node_neighbors_init(node) != RC_OK) {
    log_info(NODE_LOGGER_ID, "Initializing neighbors failed\n");
    return RC_NODE_FAILED_NEIGHBORS_INIT;
  }

  log_info(NODE_LOGGER_ID, "Initializing broadcaster component\n");
  if (broadcaster_init(&node->broadcaster, node) != RC_OK) {
    log_critical(NODE_LOGGER_ID, "Initializing broadcaster component failed\n");
    return RC_NODE_FAILED_BROADCASTER_INIT;
  }

  log_info(NODE_LOGGER_ID, "Initializing processor component\n");
  if (processor_init(&node->processor, node, tangle,
                     &core->consensus.transaction_validator,
                     &core->consensus.transaction_solidifier) != RC_OK) {
    log_critical(NODE_LOGGER_ID, "Initializing processor component failed\n");
    return RC_NODE_FAILED_PROCESSOR_INIT;
  }

  log_info(NODE_LOGGER_ID, "Initializing receiver component\n");
  if (receiver_init(&node->receiver, node, node->conf.tcp_receiver_port,
                    node->conf.udp_receiver_port) != RC_OK) {
    log_critical(NODE_LOGGER_ID, "Initializing receiver component failed\n");
    return RC_NODE_FAILED_RECEIVER_INIT;
  }

  log_info(NODE_LOGGER_ID, "Initializing responder component\n");
  if (responder_init(&node->responder, node, tangle) != RC_OK) {
    log_critical(NODE_LOGGER_ID, "Initializing responder component failed\n");
    return RC_NODE_FAILED_RESPONDER_INIT;
  }

  log_info(NODE_LOGGER_ID, "Initializing tips requester component\n");
  if ((ret = tips_requester_init(&node->tips_requester, node,
                                 &core->consensus.tangle)) != RC_OK) {
    log_critical(NODE_LOGGER_ID,
                 "Initializing  tips requester component failed\n");
    return ret;
  }

  log_info(NODE_LOGGER_ID, "Initializing transaction requester component\n");
  if (node_transaction_requester_init(node, tangle) != RC_OK) {
    log_critical(NODE_LOGGER_ID,
                 "Initializing transaction requester component failed\n");
    return RC_NODE_FAILED_REQUESTER_INIT;
  }

  log_info(NODE_LOGGER_ID, "Initializing tips cache\n");
  if ((ret = node_tips_cache_init(node)) != RC_OK) {
    log_error(NODE_LOGGER_ID, "Initializing tips cache failed\n");
    return ret;
  }

  return ret;
}

retcode_t node_start(node_t* const node) {
  retcode_t ret = RC_OK;

  if (node == NULL) {
    return RC_NODE_NULL_NODE;
  }

  log_info(NODE_LOGGER_ID, "Starting broadcaster component\n");
  if (broadcaster_start(&node->broadcaster) != RC_OK) {
    log_critical(NODE_LOGGER_ID, "Starting broadcaster component failed\n");
    return RC_NODE_FAILED_BROADCASTER_START;
  }

  log_info(NODE_LOGGER_ID, "Starting processor component\n");
  if (processor_start(&node->processor) != RC_OK) {
    log_critical(NODE_LOGGER_ID, "Starting processor component failed\n");
    return RC_NODE_FAILED_PROCESSOR_START;
  }

  log_info(NODE_LOGGER_ID, "Starting receiver component\n");
  if (receiver_start(&node->receiver) != RC_OK) {
    log_critical(NODE_LOGGER_ID, "Starting receiver component failed\n");
    return RC_NODE_FAILED_RECEIVER_START;
  }

  log_info(NODE_LOGGER_ID, "Starting responder component\n");
  if (responder_start(&node->responder) != RC_OK) {
    log_critical(NODE_LOGGER_ID, "Starting responder component failed\n");
    return RC_NODE_FAILED_RESPONDER_START;
  }

  log_info(NODE_LOGGER_ID, "Starting tips requester component\n");
  if ((ret = tips_requester_start(&node->tips_requester)) != RC_OK) {
    log_critical(NODE_LOGGER_ID, "Starting tips requester component failed\n");
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

  log_info(NODE_LOGGER_ID, "Stopping broadcaster component\n");
  if (broadcaster_stop(&node->broadcaster) != RC_OK) {
    log_error(NODE_LOGGER_ID, "Stopping broadcaster component failed\n");
    ret = RC_NODE_FAILED_BROADCASTER_STOP;
  }

  log_info(NODE_LOGGER_ID, "Stopping processor component\n");
  if (processor_stop(&node->processor) != RC_OK) {
    log_error(NODE_LOGGER_ID, "Stopping processor component failed\n");
    ret = RC_NODE_FAILED_PROCESSOR_STOP;
  }

  log_info(NODE_LOGGER_ID, "Stopping receiver component\n");
  if (receiver_stop(&node->receiver) != RC_OK) {
    log_error(NODE_LOGGER_ID, "Stopping receiver component failed\n");
    ret = RC_NODE_FAILED_RECEIVER_STOP;
  }

  log_info(NODE_LOGGER_ID, "Stopping responder component\n");
  if (responder_stop(&node->responder) != RC_OK) {
    log_error(NODE_LOGGER_ID, "Stopping responder component failed\n");
    ret = RC_NODE_FAILED_RESPONDER_STOP;
  }

  log_info(NODE_LOGGER_ID, "Stopping tips requester component\n");
  if ((ret = tips_requester_stop(&node->tips_requester)) != RC_OK) {
    log_error(NODE_LOGGER_ID, "Stopping tips requester component failed\n");
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

  log_info(NODE_LOGGER_ID, "Destroying transaction requester component\n");
  if (requester_destroy(&node->transaction_requester) != RC_OK) {
    log_error(NODE_LOGGER_ID,
              "Destroying transaction requester component failed\n");
    ret = RC_NODE_FAILED_REQUESTER_DESTROY;
  }

  log_info(NODE_LOGGER_ID, "Destroying tips requester component\n");
  if ((ret = tips_requester_destroy(&node->tips_requester)) != RC_OK) {
    log_error(NODE_LOGGER_ID, "Destroying tips requester component failed\n");
  }

  log_info(NODE_LOGGER_ID, "Destroying broadcaster component\n");
  if (broadcaster_destroy(&node->broadcaster) != RC_OK) {
    log_error(NODE_LOGGER_ID, "Destroying broadcaster component failed\n");
    ret = RC_NODE_FAILED_BROADCASTER_DESTROY;
  }

  log_info(NODE_LOGGER_ID, "Destroying processor component\n");
  if (processor_destroy(&node->processor) != RC_OK) {
    log_error(NODE_LOGGER_ID, "Destroying processor component failed\n");
    ret = RC_NODE_FAILED_PROCESSOR_DESTROY;
  }

  log_info(NODE_LOGGER_ID, "Destroying receiver component\n");
  if (receiver_destroy(&node->receiver) != RC_OK) {
    log_error(NODE_LOGGER_ID, "Destroying receiver component failed\n");
    ret = RC_NODE_FAILED_RECEIVER_DESTROY;
  }

  log_info(NODE_LOGGER_ID, "Destroying responder component\n");
  if (responder_destroy(&node->responder) != RC_OK) {
    log_error(NODE_LOGGER_ID, "Destroying responder component failed\n");
    ret = RC_NODE_FAILED_RESPONDER_DESTROY;
  }

  log_debug(NODE_LOGGER_ID, "Destroying neighbors list\n");
  if (CL_DESTROY(neighbor_t, node->neighbors) != CL_SUCCESS) {
    log_error(NODE_LOGGER_ID, "Destroying neighbors list failed\n");
    ret = RC_NODE_FAILED_NEIGHBORS_DESTROY;
  }

  tips_cache_destroy(&node->tips);

  logger_helper_destroy(NODE_LOGGER_ID);

  return ret;
}
