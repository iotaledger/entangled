/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __CIRI_USAGE_H__
#define __CIRI_USAGE_H__

#include <stdlib.h>

typedef enum cli_arg_value_e {
  CLI_ARG = 1000,

  // Gossip configuration

  CLI_ARG_MWM,
  CLI_ARG_P_PROPAGATE_REQUEST,
  CLI_ARG_P_REMOVE_REQUEST,
  CLI_ARG_P_REPLY_RANDOM_TIP,
  CLI_ARG_P_SELECT_MILESTONE,
  CLI_ARG_P_SEND_MILESTONE,
  CLI_ARG_REQUESTER_QUEUE_SIZE,
  CLI_ARG_TIPS_CACHE_SIZE,

  // API configuration

  CLI_ARG_MAX_GET_TRYTES,

  // Consensus configuration

  CLI_ARG_ALPHA,
  CLI_ARG_BELOW_MAX_DEPTH,
  CLI_ARG_LAST_MILESTONE,
  CLI_ARG_MAX_DEPTH,
  CLI_ARG_NUM_KEYS_IN_MILESTONE,
  CLI_ARG_SNAPSHOT_FILE,
  CLI_ARG_SNAPSHOT_SIGNATURE_DEPTH,
  CLI_ARG_SNAPSHOT_SIGNATURE_FILE,
  CLI_ARG_SNAPSHOT_SIGNATURE_INDEX,
  CLI_ARG_SNAPSHOT_SIGNATURE_PUBKEY,
  CLI_ARG_SNAPSHOT_TIMESTAMP,

} cli_arg_value_t;

typedef enum cli_arg_requirement_e {
  NO_ARG,
  REQUIRED_ARG,
  OPTIONAL_ARG
} cli_arg_requirement_t;

static struct cli_argument_s {
  char* name;
  int val;
  char* desc;
  cli_arg_requirement_t has_arg;
} cli_arguments_g[] = {

    // cIRI configuration

    {"db-path", 'd', "Path to the database file.", REQUIRED_ARG},
    {"help", 'h', "Displays this usage.", NO_ARG},
    {"log-level", 'l',
     "Valid log levels: \"debug\", \"info\", \"notice\", \"warning\", "
     "\"error\", \"critical\", \"alert\" "
     "and \"emergency\".",
     REQUIRED_ARG},

    // Gossip configuration

    {"mwm", CLI_ARG_MWM,
     "Number of trailing ternary 0s that must appear at the end of a "
     "transaction hash. Difficulty can be described as 3^mwm.",
     REQUIRED_ARG},
    {"neighbors", 'n', "URIs of neighbouring nodes, separated by a space.",
     REQUIRED_ARG},
    {"p-propagate-request", CLI_ARG_P_PROPAGATE_REQUEST,
     "Probability of propagating the request of a transaction to a neighbor "
     "node if it can't be found. This should be low since we don't want to "
     "propagate non-existing transactions that spam the network. Value must be "
     "in [0,1].",
     REQUIRED_ARG},
    {"p-remove-request", CLI_ARG_P_REMOVE_REQUEST,
     "Probability of removing a transaction from the request queue without "
     "requesting it. Value must be in [0,1].",
     REQUIRED_ARG},
    {"p-reply-random-tip", CLI_ARG_P_REPLY_RANDOM_TIP,
     "Probability of replying to a random transaction request, even though "
     "your node doesn't have anything to request. Value must be in [0,1].",
     REQUIRED_ARG},
    {"p-select-milestone", CLI_ARG_P_SELECT_MILESTONE,
     "Probability of sending a current milestone request to a neighbour. "
     "Value must be in [0,1].",
     REQUIRED_ARG},
    {"p-send-milestone", CLI_ARG_P_SEND_MILESTONE,
     "Probability of sending a milestone transaction when the node looks for a "
     "random transaction to send to a neighbor. Value must be in [0,1].",
     REQUIRED_ARG},
    {"requester-queue-size", CLI_ARG_REQUESTER_QUEUE_SIZE,
     "Size of the transaction requester queue.", REQUIRED_ARG},
    {"tcp-receiver-port", 't', "TCP listen port.", REQUIRED_ARG},
    {"tips-cache-size", CLI_ARG_TIPS_CACHE_SIZE,
     "Size of the tips cache. Also bounds the number of tips returned by "
     "getTips API call.",
     REQUIRED_ARG},
    {"udp-receiver-port", 'u', "UDP listen port.", REQUIRED_ARG},

    // API configuration

    {"max-get-trytes", CLI_ARG_MAX_GET_TRYTES,
     "Maximum number of transactions that will be returned by the 'getTrytes' "
     "API call.",
     REQUIRED_ARG},
    {"port", 'p', "HTTP API listen port.", REQUIRED_ARG},

    // Consensus configuration

    {"alpha", CLI_ARG_ALPHA,
     "Randomness of the tip selection. Value must be in [0, inf] where 0 is "
     "most random and inf is most deterministic.",
     REQUIRED_ARG},
    {"below-max-depth", CLI_ARG_BELOW_MAX_DEPTH,
     "Maximum number of unconfirmed transactions that may be analysed to find "
     "the latest referenced milestone by the currently visited transaction "
     "during the random walk.",
     REQUIRED_ARG},
    {"last-milestone", CLI_ARG_LAST_MILESTONE,
     "The index of the last milestone issued by the corrdinator before the "
     "last snapshot.",
     REQUIRED_ARG},
    {"max-depth", CLI_ARG_MAX_DEPTH,
     "Limits how many milestones behind the current one the random walk can "
     "start.",
     REQUIRED_ARG},
    {"num-keys-in-milestone", CLI_ARG_NUM_KEYS_IN_MILESTONE,
     "The depth of the Merkle tree which in turn determines the number of "
     "leaves (private keys) that the coordinator can use to sign a message.",
     REQUIRED_ARG},
    {"snapshot-file", CLI_ARG_SNAPSHOT_FILE,
     "Path to the file that contains the state of the ledger at the last "
     "snapshot.",
     REQUIRED_ARG},
    {"snapshot-signature-depth", CLI_ARG_SNAPSHOT_SIGNATURE_DEPTH,
     "Depth of the snapshot signature.", REQUIRED_ARG},
    {"snapshot-signature-file", CLI_ARG_SNAPSHOT_SIGNATURE_FILE,
     "Path to the file that contains a signature for the snapshot file.",
     REQUIRED_ARG},
    {"snapshot-signature-index", CLI_ARG_SNAPSHOT_SIGNATURE_INDEX,
     "Index of the snapshot signature.", REQUIRED_ARG},
    {"snapshot-signature-pubkey", CLI_ARG_SNAPSHOT_SIGNATURE_PUBKEY,
     "Public key of the snapshot signature.", REQUIRED_ARG},
    {"snapshot-timestamp", CLI_ARG_SNAPSHOT_TIMESTAMP,
     "Epoch time of the last snapshot.", REQUIRED_ARG},
    {NULL, 0, NULL, NO_ARG}};

static char* short_options = "hl:d:n:t:u:p:";

#ifdef __cplusplus
extern "C" {
#endif

void iota_usage();

#ifdef __cplusplus
}
#endif

#endif  // __CIRI_USAGE_H__
