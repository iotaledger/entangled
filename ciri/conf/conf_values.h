/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __CIRI_CONF_CONF_VALUES_H__
#define __CIRI_CONF_CONF_VALUES_H__

// Command line arguments default values

#define CONF_DEFAULT_API_PORT 14265
#define CONF_DEFAULT_NEIGHBORS NULL
#define CONF_DEFAULT_CONF_FILE NULL
#define CONF_DEFAULT_TCP_RECEIVER_PORT 15600
#define CONF_DEFAULT_UDP_RECEIVER_PORT 14600
#define CONF_DEFAULT_TESTNET false
#define CONF_DEFAULT_REMOTE false
#define CONF_DEFAULT_REMOTE_AUTH_TOKEN NULL
#define CONF_DEFAULT_REMOTE_LIMIT_API NULL
#define CONF_DEFAULT_SEND_LIMIT 0
#define CONF_DEFAULT_MAX_PEERS 0
#define CONF_DEFAULT_DNS_RESOLUTION true

// Mainnet config

#define MAINNET_COORDINATOR_ADDRESS                                          \
  "KPWCHICGJZXKE9GSUDXZYUAPLHAKAHYHDXNPHENTERYMMBQOPSQIDENXKLKCEYCPVTZQLEEJ" \
  "VYJZV9BWU"
#define MAINNET_REQUEST_HASH_SIZE 46
#define MAINNET_MWM 14
#define MAINNET_MILESTONE_START_INDEX 590000
#define MAINNET_PACKET_SIZE 1650

// Testnet config

#define TESTNET_COORDINATOR_ADDRESS                                            \
  "EQQFCZBIHRHWPXKMTOLMYUYPCN9XLMJPYZVFJSAY9FQHCCLWTOLLUGKKMXYFDBOOYFBLBI9WUE" \
  "ILGECYM"
#define TESTNET_REQUEST_HASH_SIZE 49
#define TESTNET_MWM 9
#define TESTNET_MILESTONE_START_INDEX 434525
#define TESTNET_PACKET_SIZE 1653

// Milestone tracker config

#define LMT_RESCAN_INTERVAL 5000
#define SMT_RESCAN_INTERVAL 5000

#endif  // __CIRI_CONF_CONF_VALUES_H__
