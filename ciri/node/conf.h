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

#define DEFAULT_AUTO_TETHERING_ENABLED false
#define DEFAULT_COORDINATOR_ADDRESS COORDINATOR_ADDRESS
#define DEFAULT_MAX_NEIGHBORS 5
#define DEFAULT_MWN MWM
#define DEFAULT_NEIGHBORING_ADDRESS "0.0.0.0"
#define DEFAULT_NEIGHBORING_PORT 15600
#define DEFAULT_NEIGHBORS NULL
#define DEFAULT_PROBABILITY_REPLY_RANDOM_TIP 0.66
#define DEFAULT_PROBABILITY_SEND_MILESTONE 0.02
#define DEFAULT_RECENT_SEEN_BYTES_CACHE_SIZE 1500
#define DEFAULT_RECONNECT_ATTEMPT_INTERVAL 60
#define DEFAULT_REQUESTER_QUEUE_SIZE 10000
#define DEFAULT_TIPS_CACHE_SIZE 5000

#ifdef __cplusplus
extern "C" {
#endif

// This structure contains all configuration variables needed to operate a IOTA node
typedef struct iota_node_conf_s {
  // Number of trailing ternary 0s that must appear at the end of a transaction
  // hash. Difficulty can be described as 3^mwm
  uint8_t mwm;
  // URIs of neighbouring nodes, separated by a space
  char* neighbors;
  // Probability of replying to a random transaction request, even though your
  // node doesn't have anything to request. Value must be in [0,1]
  double p_reply_random_tip;
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
  // The address of the coordinator encoded in bytes
  byte_t coordinator_address[HASH_LENGTH_BYTE];
  // The address to bind the TCP server socket to
  char neighboring_address[16];
  // The TCP receiver port
  uint16_t neighboring_port;
  // Whether to accept new connections from unknown neighbors (which are not defined in the config and were not added
  // via addNeighbors)
  bool auto_tethering_enabled;
  // The maximum number of neighbors allowed to be connected
  size_t max_neighbors;
  // The interval (in seconds) at which to reconnect to neighbors
  size_t reconnect_attempt_interval;
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
