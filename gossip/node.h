/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __GOSSIP_NODE_H__
#define __GOSSIP_NODE_H__

#include "common/errors.h"
#include "gossip/components/broadcaster.h"
#include "gossip/components/processor.h"
#include "gossip/components/receiver.h"
#include "gossip/components/responder.h"
#include "gossip/components/transaction_requester.h"
#include "gossip/tips_cache.h"

#ifdef __cplusplus
extern "C" {
#endif

// Forward declarations
typedef struct core_s core_t;
typedef struct concurrent_list_neighbor_t_s neighbors_list_t;

typedef struct node_s {
  iota_gossip_conf_t conf;
  bool running;
  core_t* core;
  broadcaster_t broadcaster;
  processor_t processor;
  receiver_state_t receiver;
  responder_t responder;
  requester_state_t transaction_requester;
  neighbors_list_t* neighbors;
  tips_cache_t tips;
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
retcode_t node_init(iota_node_t* const node, core_t* const core,
                    tangle_t* const tangle);

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

#endif  // __GOSSIP_NODE_H__
