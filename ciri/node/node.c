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

  log_info(logger_id, "Initializing broadcaster stage\n");
  if ((ret = broadcaster_stage_init(&node->broadcaster, node)) != RC_OK) {
    log_critical(logger_id, "Initializing broadcaster stage failed\n");
    return ret;
  }

  log_info(logger_id, "Initializing processor stage\n");
  if ((ret = processor_stage_init(&node->processor, node, &core->consensus.transaction_validator,
                                  &core->consensus.transaction_solidifier, &core->consensus.milestone_tracker)) !=
      RC_OK) {
    log_critical(logger_id, "Initializing processor stage failed\n");
    return ret;
  }

  log_info(logger_id, "Initializing responder stage\n");
  if ((ret = responder_stage_init(&node->responder, node)) != RC_OK) {
    log_critical(logger_id, "Initializing responder stage failed\n");
    return ret;
  }

  log_info(logger_id, "Initializing tips requester\n");
  if ((ret = tips_requester_init(&node->tips_requester, node)) != RC_OK) {
    log_critical(logger_id, "Initializing tips requester failed\n");
    return ret;
  }

  log_info(logger_id, "Initializing transaction requester\n");
  if ((ret = requester_init(&node->transaction_requester, node)) != RC_OK) {
    log_critical(logger_id, "Initializing transaction requester failed\n");
    return ret;
  }

  log_info(logger_id, "Initializing tips cache\n");
  if ((ret = tips_cache_init(&node->tips, node->conf.tips_cache_size)) != RC_OK) {
    log_critical(logger_id, "Initializing tips cache failed\n");
    return ret;
  }

  log_info(logger_id, "Initializing recent seen bytes cache\n");
  if ((ret = recent_seen_bytes_cache_init(&node->recent_seen_bytes, node->conf.recent_seen_bytes_cache_size)) !=
      RC_OK) {
    log_critical(logger_id, "Initializing recent seen bytes cache failed\n");
    return ret;
  }

  log_info(logger_id, "Initializing router\n");
  if ((ret = router_init(&node->router, node)) != RC_OK) {
    log_critical(logger_id, "Initializing router failed\n");
    return ret;
  }

  return ret;
}

retcode_t node_start(node_t* const node) {
  retcode_t ret = RC_OK;

  if (node == NULL) {
    return RC_NULL_PARAM;
  }

  log_info(logger_id, "Starting broadcaster stage\n");
  if ((ret = broadcaster_stage_start(&node->broadcaster)) != RC_OK) {
    log_critical(logger_id, "Starting broadcaster stage failed\n");
    return ret;
  }

  log_info(logger_id, "Starting processor stage\n");
  if ((ret = processor_stage_start(&node->processor)) != RC_OK) {
    log_critical(logger_id, "Starting processor stage failed\n");
    return ret;
  }

  log_info(logger_id, "Starting responder stage\n");
  if ((ret = responder_stage_start(&node->responder)) != RC_OK) {
    log_critical(logger_id, "Starting responder stage failed\n");
    return ret;
  }

  log_info(logger_id, "Starting tips requester\n");
  if ((ret = tips_requester_start(&node->tips_requester)) != RC_OK) {
    log_critical(logger_id, "Starting tips requester failed\n");
    return ret;
  }

  log_info(logger_id, "Starting transaction requester\n");
  if ((ret = requester_start(&node->transaction_requester)) != RC_OK) {
    log_critical(logger_id, "Starting transaction requester failed\n");
    return ret;
  }

  log_info(logger_id, "Starting router\n");
  if ((ret = router_start(&node->router)) != RC_OK) {
    log_critical(logger_id, "Starting router failed\n");
    return ret;
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

  log_info(logger_id, "Stopping broadcaster stage\n");
  if ((ret = broadcaster_stage_stop(&node->broadcaster)) != RC_OK) {
    log_error(logger_id, "Stopping broadcaster stage failed\n");
  }

  log_info(logger_id, "Stopping processor stage\n");
  if ((ret = processor_stage_stop(&node->processor)) != RC_OK) {
    log_error(logger_id, "Stopping processor stage failed\n");
  }

  log_info(logger_id, "Stopping responder stage\n");
  if ((ret = responder_stage_stop(&node->responder)) != RC_OK) {
    log_error(logger_id, "Stopping responder stage failed\n");
  }

  log_info(logger_id, "Stopping tips requester\n");
  if ((ret = tips_requester_stop(&node->tips_requester)) != RC_OK) {
    log_error(logger_id, "Stopping tips requester failed\n");
  }

  log_info(logger_id, "Stopping transaction requester\n");
  if ((ret = requester_stop(&node->transaction_requester)) != RC_OK) {
    log_error(logger_id, "Stopping transaction requester failed\n");
  }

  log_info(logger_id, "Stopping router\n");
  if ((ret = router_stop(&node->router)) != RC_OK) {
    log_error(logger_id, "Stopping router failed\n");
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

  log_info(logger_id, "Destroying router\n");
  if ((ret = router_destroy(&node->router)) != RC_OK) {
    log_error(logger_id, "Destroying router failed\n");
  }

  log_info(logger_id, "Destroying transaction requester\n");
  if ((ret = requester_destroy(&node->transaction_requester)) != RC_OK) {
    log_error(logger_id, "Destroying transaction requester failed\n");
  }

  log_info(logger_id, "Destroying tips requester\n");
  if ((ret = tips_requester_destroy(&node->tips_requester)) != RC_OK) {
    log_error(logger_id, "Destroying tips requester failed\n");
  }

  log_info(logger_id, "Destroying broadcaster stage\n");
  if ((ret = broadcaster_stage_destroy(&node->broadcaster)) != RC_OK) {
    log_error(logger_id, "Destroying broadcaster stage failed\n");
  }

  log_info(logger_id, "Destroying processor stage\n");
  if ((ret = processor_stage_destroy(&node->processor)) != RC_OK) {
    log_error(logger_id, "Destroying processor stage failed\n");
  }

  log_info(logger_id, "Destroying responder stage\n");
  if ((ret = responder_stage_destroy(&node->responder)) != RC_OK) {
    log_error(logger_id, "Destroying responder stage failed\n");
  }

  tips_cache_destroy(&node->tips);
  recent_seen_bytes_cache_destroy(&node->recent_seen_bytes);
  free(node->conf.neighbors);

  logger_helper_release(logger_id);

  return ret;
}
