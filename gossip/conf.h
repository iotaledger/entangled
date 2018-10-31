/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __GOSSIP_CONF_H__
#define __GOSSIP_CONF_H__

#include <stdint.h>

#include "common/errors.h"

#define PACKET_TX_SIZE 1604
#define PACKET_SIZE PACKET_TX_SIZE + REQUEST_HASH_SIZE

#define DEFAULT_UDP_RECEIVER_PORT 14600
#define DEFAULT_TCP_RECEIVER_PORT 15600
#define DEFAULT_MWN MWM
#define DEFAULT_NEIGHBORS NULL
#define DEFAULT_PROBABILITY_SELECT_MILESTONE 0.7
#define DEFAULT_PROBABILITY_REMOVE_REQUEST 0.01

#ifdef __cplusplus
extern "C" {
#endif

// This structure contains all configuration variables needed to operate the
// IOTA gossip protocol
typedef struct iota_gossip_conf_s {
  // The UDP receiver port
  uint16_t udp_receiver_port;
  // The TCP receiver port
  uint16_t tcp_receiver_port;
  // The minimum weight magnitude is the number of trailing 0s that must appear
  // in the end of a transaction hash. Increasing this number by 1 will result
  // in proof of work that is 3 times as hard
  uint8_t mwm;
  // Node neighbors
  char* neighbors;
  // A number between 0 and 1 that represents the probability of requesting a
  // milestone transaction from a neighbor. This should be large since it is
  // imperative that we find milestones to get transactions confirmed
  double p_select_milestone;
  // A number between 0 and 1 that represents the probability of stopping to
  // request a transaction. This number should be closer to 0 so non-existing
  // transaction hashes will eventually be removed
  double p_remove_request;
} iota_gossip_conf_t;

/**
 * Initializes gossip configuration with default values
 *
 * @param conf Gossip configuration variables
 *
 * @return a status code
 */
retcode_t iota_gossip_conf_init(iota_gossip_conf_t* const conf);

#ifdef __cplusplus
}
#endif

#endif  // __GOSSIP_CONF_H__
