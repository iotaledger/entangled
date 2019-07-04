/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __NODE_NODE_H__
#define __NODE_NODE_H__

#include "ciri/node/neighbor.h"
#include "ciri/node/network/tcp_server.h"
#include "ciri/node/pipeline/broadcaster.h"
#include "ciri/node/pipeline/processor.h"
#include "ciri/node/pipeline/responder.h"
#include "ciri/node/pipeline/tips_requester.h"
#include "ciri/node/pipeline/tips_solidifier.h"
#include "ciri/node/pipeline/transaction_requester.h"
#include "ciri/node/pipeline/transaction_requester_worker.h"
#include "ciri/node/recent_seen_bytes_cache.h"
#include "ciri/node/tips_cache.h"
#include "common/errors.h"
#include "utils/handles/rw_lock.h"

#ifdef __cplusplus
extern "C" {
#endif

// Forward declarations
typedef struct core_s core_t;

typedef struct node_s {
  iota_node_conf_t conf;
  bool running;
  core_t* core;
  broadcaster_stage_t broadcaster;
  processor_stage_t processor;
  responder_stage_t responder;
  transaction_requester_t transaction_requester;
  tips_requester_t tips_requester;
  tips_solidifier_t tips_solidifier;
  tcp_server_t tcp_server;
  neighbor_t* neighbors;
  rw_lock_handle_t neighbors_lock;
  tips_cache_t tips;
  recent_seen_bytes_cache_t recent_seen_bytes;
} iota_node_t;

/**
 * Initializes a node
 *
 * @param node The node
 * @param core A core
 * @param tangle A tangle
 *
 * @return a status code
 */
retcode_t node_init(iota_node_t* const node, core_t* const core, tangle_t* const tangle);

/**
 * Starts a node
 *
 * @param node The node
 *
 * @return a status code
 */
retcode_t node_start(iota_node_t* const node);

/**
 * Stops a node
 *
 * @param node The node
 *
 * @return a status code
 */
retcode_t node_stop(iota_node_t* const node);

/**
 * Destroys a node
 *
 * @param node The node
 *
 * @return a status code
 */
retcode_t node_destroy(iota_node_t* const node);

#ifdef __cplusplus
}
#endif

#endif  // __NODE_NODE_H__
