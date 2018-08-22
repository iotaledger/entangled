// Copyright 2018 IOTA Foundation

#ifndef CCLIENT_RESPONSE_GET_NODE_INFO_H
#define CCLIENT_RESPONSE_GET_NODE_INFO_H

#ifdef __cplusplus
extern "C" {
#endif

#include "types/types.h"

typedef struct {
  /**
   * Name of the IOTA software you're currently using (IRI stands for Initial
   * Reference Implementation).
   */
  char_buffer_t* appName;
  /**
   * The version of the IOTA software you're currently running.
   */
  char_buffer_t* appVersion;
  /**
   * Available cores on your machine for JRE.
   */
  int jreAvailableProcessors;
  /**
   * Returns the amount of free memory in the Java Virtual Machine.
   */
  size_t jreFreeMemory;
  /**
   * Returns the maximum amount of memory that the Java virtual machine will
   * attempt to use.
   */
  size_t jreMaxMemory;
  /**
   * Returns the total amount of memory in the Java virtual machine.
   */
  size_t jreTotalMemory;
  /**
   * Latest milestone that was signed off by the coordinator.
   */
  char_buffer_t* latestMilestone;
  /**
   * Index of the latest milestone.
   */
  size_t latestMilestoneIndex;
  /**
   * The latest milestone which is solid and is used for sending transactions.
   * For a milestone to become solid your local node must basically approve the
   * subtangle of coordinator-approved transactions, and have a consistent view
   * of all referenced transactions.
   */
  char_buffer_t* latestSolidSubtangleMilestone;
  /**
   * Index of the latest solid subtangle.
   */
  size_t latestSolidSubtangleMilestoneIndex;
  /**
   * Number of neighbors you are directly connected with.
   */
  int neighbors;
  /**
   * Packets which are currently queued up.
   */
  int packetsQueueSize;
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
  int transactionsToRequest;
} get_node_info_res_t;

get_node_info_res_t* get_node_info_res_new();
void get_node_info_res_free(get_node_info_res_t* res);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_RESPONSE_GET_NODE_INFO_H
