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
  CONF_START = 1000,

  // cIRI configuration

  CONF_SPENT_ADDRESSES_DB_PATH,
  CONF_TANGLE_DB_PATH,
  CONF_TANGLE_DB_REVALIDATE,

  // Node configuration

  CONF_AUTO_TETHERING_ENABLED,
  CONF_MAX_NEIGHBORS,
  CONF_MWM,
  CONF_NEIGHBORING_ADDRESS,
  CONF_P_PROPAGATE_REQUEST,
  CONF_P_REMOVE_REQUEST,
  CONF_P_REPLY_RANDOM_TIP,
  CONF_P_SELECT_MILESTONE,
  CONF_P_SEND_MILESTONE,
  CONF_RECENT_SEEN_BYTES_CACHE_SIZE,
  CONF_RECONNECT_ATTEMPT_INTERVAL,
  CONF_REQUESTER_QUEUE_SIZE,
  CONF_TIPS_CACHE_SIZE,
  CONF_TIPS_SOLIDIFIER_ENABLED,

  // API configuration

  CONF_MAX_FIND_TRANSACTIONS,
  CONF_MAX_GET_TRYTES,
  CONF_REMOTE_LIMIT_API,

  // Consensus configuration

  CONF_ALPHA,
  CONF_BELOW_MAX_DEPTH,
  CONF_COORDINATOR_ADDRESS,
  CONF_COORDINATOR_DEPTH,
  CONF_COORDINATOR_SECURITY_LEVEL,
  CONF_COORDINATOR_SIGNATURE_TYPE,
  CONF_LAST_MILESTONE,
  CONF_MAX_DEPTH,
  CONF_SNAPSHOT_FILE,
  CONF_SNAPSHOT_SIGNATURE_DEPTH,
  CONF_SNAPSHOT_SIGNATURE_FILE,
  CONF_SNAPSHOT_SIGNATURE_INDEX,
  CONF_SNAPSHOT_SIGNATURE_PUBKEY,
  CONF_SNAPSHOT_SIGNATURE_SKIP_VALIDATION,
  CONF_SNAPSHOT_TIMESTAMP,
  CONF_SPENT_ADDRESSES_FILES,

  // Local snapshots

  CONF_LOCAL_SNAPSHOTS_ENABLED,
  CONF_LOCAL_SNAPSHOTS_PRUNNING_ENABLED,
  CONF_LOCAL_SNAPSHOTS_TRANSACTIONS_GROWTH_THRESHOLD,
  CONF_LOCAL_SNAPSHOTS_MIN_DEPTH,
  CONF_LOCAL_SNAPSHOTS_PATH_BASE

} cli_arg_value_t;

typedef enum cli_arg_requirement_e { NO_ARG, REQUIRED_ARG, OPTIONAL_ARG } cli_arg_requirement_t;

static struct cli_argument_s {
  char* name;
  int val;
  char* desc;
  cli_arg_requirement_t has_arg;
} cli_arguments_g[] = {

    // cIRI configuration

    {"config", 'c', "Path to the configuration file.", REQUIRED_ARG},
    {"help", 'h', "Displays this usage.", NO_ARG},
    {"log-level", 'l',
     "Valid log levels: \"debug\", \"info\", \"notice\", \"warning\", "
     "\"error\", \"critical\", \"alert\" "
     "and \"emergency\".",
     REQUIRED_ARG},
    {"spent-addresses-db-path", CONF_SPENT_ADDRESSES_DB_PATH, "Path to the spent addresses database file.",
     REQUIRED_ARG},
    {"tangle-db-path", CONF_TANGLE_DB_PATH, "Path to the tangle database file.", REQUIRED_ARG},
    {"tangle-db-revalidate", CONF_TANGLE_DB_REVALIDATE,
     "Reloads milestones, state of the ledger and transactions metadata from the tangle database.", REQUIRED_ARG},

    // Node configuration

    {"auto-tethering-enabled", CONF_AUTO_TETHERING_ENABLED,
     "Whether to accept new connections from unknown neighbors (which are not defined in the config and were not added "
     "via addNeighbors).",
     REQUIRED_ARG},
    {"max-neighbors", CONF_MAX_NEIGHBORS, "The maximum number of neighbors allowed to be connected.", REQUIRED_ARG},
    {"mwm", CONF_MWM,
     "Number of trailing ternary 0s that must appear at the end of a "
     "transaction hash. Difficulty can be described as 3^mwm.",
     REQUIRED_ARG},
    {"neighboring-address", CONF_NEIGHBORING_ADDRESS, "The address to bind the TCP server socket to.", REQUIRED_ARG},
    {"neighboring-port", 't', "The TCP receiver port.", REQUIRED_ARG},
    {"neighbors", 'n', "URIs of neighbouring nodes, separated by a space.", REQUIRED_ARG},
    {"p-propagate-request", CONF_P_PROPAGATE_REQUEST,
     "Probability of propagating the request of a transaction to a neighbor "
     "node if it can't be found. This should be low since we don't want to "
     "propagate non-existing transactions that spam the network. Value must be "
     "in [0,1].",
     REQUIRED_ARG},
    {"p-remove-request", CONF_P_REMOVE_REQUEST,
     "Probability of removing a transaction from the request queue without "
     "requesting it. Value must be in [0,1].",
     REQUIRED_ARG},
    {"p-reply-random-tip", CONF_P_REPLY_RANDOM_TIP,
     "Probability of replying to a random transaction request, even though "
     "your node doesn't have anything to request. Value must be in [0,1].",
     REQUIRED_ARG},
    {"p-select-milestone", CONF_P_SELECT_MILESTONE,
     "Probability of sending a current milestone request to a neighbour. "
     "Value must be in [0,1].",
     REQUIRED_ARG},
    {"p-send-milestone", CONF_P_SEND_MILESTONE,
     "Probability of sending a milestone transaction when the node looks for a "
     "random transaction to send to a neighbor. Value must be in [0,1].",
     REQUIRED_ARG},
    {"requester-queue-size", CONF_REQUESTER_QUEUE_SIZE, "Size of the transaction requester queue.", REQUIRED_ARG},
    {"recent-seen-bytes-cache-size", CONF_RECENT_SEEN_BYTES_CACHE_SIZE,
     "The number of entries to keep in the network cache.", REQUIRED_ARG},
    {"reconnect-attempt-interval", CONF_RECONNECT_ATTEMPT_INTERVAL,
     "The interval (in seconds) at which to reconnect to neighbors.", REQUIRED_ARG},
    {"tips-cache-size", CONF_TIPS_CACHE_SIZE,
     "Size of the tips cache. Also bounds the number of tips returned by "
     "getTips API call.",
     REQUIRED_ARG},
    {"tips-solidifier-enabled", CONF_TIPS_SOLIDIFIER_ENABLED,
     "Scan the current tips and attempt to mark them as solid.", REQUIRED_ARG},

    // API configuration

    {"http-port", 'p', "HTTP API listen port.", REQUIRED_ARG},
    {"max-find-transactions", CONF_MAX_FIND_TRANSACTIONS,
     "The maximal number of transactions that may be returned by the "
     "'findTransactions' API call. If the number of transactions found exceeds "
     "this number an error will be returned",
     REQUIRED_ARG},
    {"max-get-trytes", CONF_MAX_GET_TRYTES,
     "Maximum number of transactions that will be returned by the 'getTrytes' "
     "API call.",
     REQUIRED_ARG},
    {"remote-limit-api", CONF_REMOTE_LIMIT_API, "Commands that should be ignored by API.", REQUIRED_ARG},

    // Consensus configuration

    {"alpha", CONF_ALPHA,
     "Randomness of the tip selection. Value must be in [0, inf] where 0 is "
     "most random and inf is most deterministic.",
     REQUIRED_ARG},
    {"below-max-depth", CONF_BELOW_MAX_DEPTH,
     "The maximal number of unconfirmed transactions that may be analyzed in order to find the latest milestone the "
     "transaction that we are stepping on during the walk approves.",
     REQUIRED_ARG},
    {"coordinator-address", CONF_COORDINATOR_ADDRESS, "The address of the coordinator.", REQUIRED_ARG},
    {"coordinator-depth", CONF_COORDINATOR_DEPTH,
     "The depth of the Merkle tree which in turn determines the number of leaves (private keys) that the coordinator "
     "can use to sign a message.",
     REQUIRED_ARG},
    {"coordinator-security-level", CONF_COORDINATOR_SECURITY_LEVEL,
     "The security level used in coordinator signatures.", REQUIRED_ARG},
    {"coordinator-signature-type", CONF_COORDINATOR_SIGNATURE_TYPE,
     "The signature type used in coordinator signatures. Valid types: \"CURL_P27\", \"CURL_P81\" and \"KERL\".",
     REQUIRED_ARG},
    {"last-milestone", CONF_LAST_MILESTONE,
     "The index of the last milestone issued by the corrdinator before the "
     "last snapshot.",
     REQUIRED_ARG},
    {"max-depth", CONF_MAX_DEPTH,
     "The maximal number of previous milestones from where you can perform the random walk.", REQUIRED_ARG},
    {"snapshot-file", CONF_SNAPSHOT_FILE,
     "Path to the file that contains the state of the ledger at the last "
     "snapshot.",
     REQUIRED_ARG},
    {"snapshot-signature-depth", CONF_SNAPSHOT_SIGNATURE_DEPTH, "Depth of the snapshot signature.", REQUIRED_ARG},
    {"snapshot-signature-file", CONF_SNAPSHOT_SIGNATURE_FILE,
     "Path to the file that contains a signature for the snapshot file.", REQUIRED_ARG},
    {"snapshot-signature-index", CONF_SNAPSHOT_SIGNATURE_INDEX, "Index of the snapshot signature.", REQUIRED_ARG},
    {"snapshot-signature-pubkey", CONF_SNAPSHOT_SIGNATURE_PUBKEY, "Public key of the snapshot signature.",
     REQUIRED_ARG},
    {"snapshot-signature-skip-validation", CONF_SNAPSHOT_SIGNATURE_SKIP_VALIDATION,
     "Skip validation of snapshot signature. Must be \"true\" or \"false\".", REQUIRED_ARG},
    {"snapshot-timestamp", CONF_SNAPSHOT_TIMESTAMP, "Epoch time of the last snapshot.", REQUIRED_ARG},
    {"spent-addresses-files", CONF_SPENT_ADDRESSES_FILES,
     "List of whitespace separated files that contains spent addresses to be merged into the database.", REQUIRED_ARG},

    // Local snapshots configuration

    {"local-snapshots-enabled", CONF_LOCAL_SNAPSHOTS_ENABLED, "Whether or not local snapshots should be enabled.",
     REQUIRED_ARG},
    {"local-snapshots-prunning-enabled", CONF_LOCAL_SNAPSHOTS_PRUNNING_ENABLED,
     "Whether or not prunning should be enabled.", REQUIRED_ARG},
    {"local-snapshots-transactions-growth-threshold", CONF_LOCAL_SNAPSHOTS_TRANSACTIONS_GROWTH_THRESHOLD,
     "Minimal number of new transactions from last local snapshot for triggering a new local snapshot.", REQUIRED_ARG},
    {"local-snapshots-min-depth", CONF_LOCAL_SNAPSHOTS_MIN_DEPTH,
     "Minimal milestones depth for new local snapshot entry point.", REQUIRED_ARG},
    {"local-snapshots-path-base", CONF_LOCAL_SNAPSHOTS_PATH_BASE,
     "The base path for both local snapshot addresses/balances data and metadata file.", REQUIRED_ARG},

    {NULL, 0, NULL, NO_ARG},

};

#ifdef __cplusplus
extern "C" {
#endif

void iota_usage();

#ifdef __cplusplus
}
#endif

#endif  // __CIRI_USAGE_H__
