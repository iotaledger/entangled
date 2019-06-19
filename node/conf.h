/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __NODE_CONF_H__
#define __NODE_CONF_H__

#include <stdbool.h>
#include <stdint.h>

#include "common/errors.h"
#include "common/trinary/flex_trit.h"
#include "utils/files.h"

#define DEFAULT_COORDINATOR_ADDRESS COORDINATOR_ADDRESS
#define DEFAULT_MWN MWM
#define DEFAULT_NEIGHBORS NULL
#define DEFAULT_PROBABILITY_PROPAGATE_REQUEST 0.01
#define DEFAULT_PROBABILITY_REMOVE_REQUEST 0.01
#define DEFAULT_PROBABILITY_REPLY_RANDOM_TIP 0.66
#define DEFAULT_PROBABILITY_SELECT_MILESTONE 0.7
#define DEFAULT_PROBABILITY_SEND_MILESTONE 0.02
#define DEFAULT_RECENT_SEEN_BYTES_CACHE_SIZE 1500
#define DEFAULT_REQUESTER_QUEUE_SIZE 10000
#define DEFAULT_TCP_RECEIVER_PORT 15600
#define DEFAULT_TIPS_CACHE_SIZE 5000
#define DEFAULT_TIPS_SOLIDIFIER_ENABLED true
#define DEFAULT_UDP_RECEIVER_PORT 14600

#ifdef __cplusplus
extern "C" {
#endif

// This structure contains all configuration variables needed to operate a IOTA node
typedef struct iota_node_conf_s {
  // UDP listen port
  uint16_t udp_receiver_port;
  // TCP listen port
  uint16_t tcp_receiver_port;
  // Number of trailing ternary 0s that must appear at the end of a transaction
  // hash. Difficulty can be described as 3^mwm
  uint8_t mwm;
  // Size of the request hash in trits, equivalent to HASH_LENGTH_TRIT - mwm
  uint8_t request_hash_size_trit;
  // URIs of neighbouring nodes, separated by a space
  char* neighbors;
  // Probability of propagating the request of a transaction to a neighbor node
  // if it can't be found. This should be low since we don't want to propagate
  // non-existing transactions that spam the network. Value must be in [0,1]
  double p_propagate_request;
  // Probability of removing a transaction from the request queue without
  // requesting it. Value must be in [0,1]
  double p_remove_request;
  // Probability of replying to a random transaction request, even though your
  // node doesn't have anything to request. Value must be in [0,1]
  double p_reply_random_tip;
  // Probability of sending a current milestone request to a neighbour. Value
  // must be in [0,1]
  double p_select_milestone;
  // Probability of sending a milestone transaction when the node looks for a
  // random transaction to send to a neighbor. Value must be in [0,1]
  double p_send_milestone;
  // Size of the tips cache
  size_t tips_cache_size;
  // The number of entries to keep in the network cache
  size_t recent_seen_bytes_cache_size;
  // Size of the requester queue
  size_t requester_queue_size;
  // Path of the tangle database file
  char tangle_db_path[FILE_PATH_SIZE];
  // Scan the current tips and attempt to mark them as solid
  bool tips_solidifier_enabled;
  // The address of the coordinator
  flex_trit_t coordinator_address[FLEX_TRIT_SIZE_243];
} iota_node_conf_t;

/**
 * Initializes node configuration with default values
 *
 * @param conf Node configuration variables
 *
 * @return a status code
 */
retcode_t iota_node_conf_init(iota_node_conf_t* const conf);

#ifdef __cplusplus
}
#endif

#endif  // __NODE_CONF_H__
