/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef CCLIENT_RESPONSE_GET_NODE_INFO_H
#define CCLIENT_RESPONSE_GET_NODE_INFO_H

#include "cclient/types/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct get_node_info_res_s {
  /**
   * Name of the IOTA software you're currently using.
   */
  char_buffer_t* app_name;
  /**
   * The version of the IOTA software you're currently running.
   */
  char_buffer_t* app_version;
  /**
   * Latest milestone that was signed off by the coordinator.
   */
  flex_trit_t latest_milestone[FLEX_TRIT_SIZE_243];
  /**
   * Index of the latest milestone.
   */
  uint32_t latest_milestone_index;
  /**
   * The latest milestone which is solid and is used for sending transactions.
   * For a milestone to become solid your local node must basically approve the
   * subtangle of coordinator-approved transactions, and have a consistent view
   * of all referenced transactions.
   */
  flex_trit_t latest_solid_subtangle_milestone[FLEX_TRIT_SIZE_243];
  /**
   * Index of the latest solid subtangle.
   */
  uint32_t latest_solid_subtangle_milestone_index;
  /**
   * The start index of the milestones.
   * This index is encoded in each milestone transaction by the coordinator
   */
  uint32_t milestone_start_index;
  /**
   * Number of neighbors you are directly connected with.
   */
  uint16_t neighbors;
  /**
   * Packets which are currently queued up.
   */
  uint16_t packets_queue_size;
  /**
   * Current UNIX timestamp.
   */
  uint64_t time;
  /**
   * Number of tips in the network.
   */
  uint32_t tips;
  /**
   * Transactions to request during syncing process.
   */
  uint32_t transactions_to_request;
  /**
   * The address of the coordinator being followed by this node.
   */
  flex_trit_t coordinator_address[FLEX_TRIT_SIZE_243];
} get_node_info_res_t;

get_node_info_res_t* get_node_info_res_new();
void get_node_info_res_free(get_node_info_res_t** res);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_RESPONSE_GET_NODE_INFO_H
