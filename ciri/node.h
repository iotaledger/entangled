/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __CIRI_NODE_H__
#define __CIRI_NODE_H__

#include "common/network/components/broadcaster.h"
#include "common/network/components/processor.h"
#include "common/network/components/receiver.h"
#include "common/network/components/requester.h"
#include "common/network/components/responder.h"
#include "common/network/lists/concurrent_list_neighbor.h"

typedef concurrent_list_of_neighbor_t neighbors_list_t;

typedef struct node_s {
  broadcaster_state_t broadcaster;
  processor_state_t processor;
  receiver_state_t receiver;
  requester_state_t requester;
  responder_state_t responder;
  neighbors_list_t* neighbors;
} node_t;

bool node_init(node_t* const node);
bool node_start(node_t* const node);
bool node_stop(node_t* const node);
bool node_destroy(node_t* const node);

#endif  // __CIRI_NODE_H__
