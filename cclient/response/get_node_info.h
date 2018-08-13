// Copyright 2018 IOTA Foundation

#ifndef CCLIENT_RESPONSE_GET_NODE_INFO_H
#define CCLIENT_RESPONSE_GET_NODE_INFO_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  /**
   * Name of the IOTA software you're currently using (IRI stands for Initial
   * Reference Implementation).
   */
  char* appName;
  /**
   * The version of the IOTA software you're currently running.
   */
  char* appVersion;
  /**
   * Available cores on your machine for JRE.
   */
  short jreAvailableProcessors;
  /**
   * Returns the amount of free memory in the Java Virtual Machine.
   */
  int jreFreeMemory;
  /**
   * Returns the maximum amount of memory that the Java virtual machine will
   * attempt to use.
   */
  int jreMaxMemory;
  /**
   * Returns the total amount of memory in the Java virtual machine.
   */
  int jreTotalMemory;
  /**
   * Latest milestone that was signed off by the coordinator.
   */
  flex_trit_p latestMilestone;
  /**
   * Index of the latest milestone.
   */
  int latestMilestoneIndex;
  /**
   * The latest milestone which is solid and is used for sending transactions.
   * For a milestone to become solid your local node must basically approve the
   * subtangle of coordinator-approved transactions, and have a consistent view
   * of all referenced transactions.
   */
  flex_trit_p latestSolidSubtangleMilestone;
  /**
   * Index of the latest solid subtangle.
   */
  int latestSolidSubtangleMilestoneIndex;
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
  int time;
  /**
   * Number of tips in the network.
   */
  int tips;
  /**
   * Transactions to request during syncing process.
   */
  int transactionsToRequest;
} get_node_info_res_t;

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_RESPONSE_GET_NODE_INFO_H
