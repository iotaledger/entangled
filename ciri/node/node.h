/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __CIRI_NODE_NODE_H__
#define __CIRI_NODE_NODE_H__

#include "ciri/node/conf.h"
#include "ciri/node/network/router.h"
#include "ciri/node/pipeline/broadcaster.h"
#include "ciri/node/pipeline/hasher.h"
#include "ciri/node/pipeline/processor.h"
#include "ciri/node/pipeline/responder.h"
#include "ciri/node/pipeline/tips_requester.h"
#include "ciri/node/pipeline/transaction_requester.h"
#include "ciri/node/pipeline/validator.h"
#include "ciri/node/recent_seen_bytes_cache.h"
#include "ciri/node/tips_cache.h"
#include "common/errors.h"

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
  hasher_stage_t hasher;
  processor_stage_t processor;
  responder_stage_t responder;
  tips_requester_t tips_requester;
  transaction_requester_t transaction_requester;
  validator_stage_t validator;
  router_t router;
  tips_cache_t tips;
  recent_seen_bytes_cache_t recent_seen_bytes;
} iota_node_t;

/**
 * Initializes a node
 *
 * @param[in, out]  node    The node
 * @param[in]       core    A core
 * @param[in]       tangle  A tangle
 *
 * @return a status code
 */
retcode_t node_init(iota_node_t* const node, core_t* const core, tangle_t* const tangle);

/**
 * Starts a node
 *
 * @param[in, out] node The node
 *
 * @return a status code
 */
retcode_t node_start(iota_node_t* const node);

/**
 * Stops a node
 *
 * @param[in, out] node The node
 *
 * @return a status code
 */
retcode_t node_stop(iota_node_t* const node);

/**
 * Destroys a node
 *
 * @param[in, out] node The node
 *
 * @return a status code
 */
retcode_t node_destroy(iota_node_t* const node);

/**
 * Checks whether the node is synchronized by comparing the latest known milestone index against the current latest
 * solid milestone index. We allow for max. one milestone delta as a headroom, as a milestone is only applied in a set
 * interval to the current ledger state.
 *
 * @param[in] node The node
 *
 * @return true if synced, false otherwise
 */
bool node_is_synced(node_t const* const node);

#ifdef __cplusplus
}
#endif

#endif  // __CIRI_NODE_NODE_H__
