/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __CIRI_NODE_H__
#define __CIRI_NODE_H__

#include "common/errors.h"
#include "common/network/components/broadcaster.h"
#include "common/network/components/processor.h"
#include "common/network/components/receiver.h"
#include "common/network/components/requester.h"
#include "common/network/components/responder.h"
#include "utils/containers/lists/concurrent_list_neighbor.h"

typedef struct core_s core_t;
typedef concurrent_list_neighbor_t neighbors_list_t;

typedef struct node_s {
  bool running;
  core_t* core;
  broadcaster_state_t broadcaster;
  processor_state_t processor;
  receiver_state_t receiver;
  requester_state_t requester;
  responder_state_t responder;
  neighbors_list_t* neighbors;
} node_t;

retcode_t node_init(node_t* const node, core_t* const core);
retcode_t node_start(node_t* const node);
retcode_t node_stop(node_t* const node);
retcode_t node_destroy(node_t* const node);

#endif  // __CIRI_NODE_H__
