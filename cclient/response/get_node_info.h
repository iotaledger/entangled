/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef CCLIENT_RESPONSE_GET_NODE_INFO_H
#define CCLIENT_RESPONSE_GET_NODE_INFO_H

#include "types/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  /**
   * Name of the IOTA software you're currently using (IRI stands for Initial
   * Reference Implementation).
   */
  char_buffer_t* app_name;
  /**
   * The version of the IOTA software you're currently running.
   */
  char_buffer_t* app_version;
  /**
   * Available cores on your machine for JRE.
   */
  int jre_available_processors;
  /**
   * Returns the amount of free memory in the Java Virtual Machine.
   */
  size_t jre_free_memory;
  /**
   * Returns the maximum amount of memory that the Java virtual machine will
   * attempt to use.
   */
  size_t jre_max_memory;
  /**
   * Returns the total amount of memory in the Java virtual machine.
   */
  size_t jre_total_memory;
  /**
   * Latest milestone that was signed off by the coordinator.
   */
  flex_trit_t latest_milestone[FLEX_TRIT_SIZE_243];
  /**
   * Index of the latest milestone.
   */
  size_t latest_milestone_index;
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
  size_t latest_solid_subtangle_milestone_index;
  /**
   * Number of neighbors you are directly connected with.
   */
  int neighbors;
  /**
   * Packets which are currently queued up.
   */
  int packets_queue_size;
  /**
   * Current UNIX timestamp.
   */
  size_t time;
  /**
   * Number of tips in the network.
   */
  int tips;
  /**
   * Transactions to request during syncing process.
   */
  int trans_to_request;
} get_node_info_res_t;

get_node_info_res_t* get_node_info_res_new();
void get_node_info_res_free(get_node_info_res_t** res);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_RESPONSE_GET_NODE_INFO_H
