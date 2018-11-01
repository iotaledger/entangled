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
  CLI_ARG_P_REMOVE_REQUEST,
  CLI_ARG_P_SELECT_MILESTONE,

  // API configuration

  CLI_ARG_MAX_GET_TRYTES,

  // Consensus configuration

  CLI_ARG_ALPHA,
  CLI_ARG_BELOW_MAX_DEPTH,
  CLI_ARG_MAX_DEPTH,

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

    {"help", 'h', "Displays this usage.", NO_ARG},
    {"log-level", 'l',
     "Log level amongst: \"debug\", \"info\", \"notice\", \"warning\", "
     "\"error\", \"critical\", \"alert\" "
     "and \"emergency\".",
     REQUIRED_ARG},

    // Gossip configuration

    {"mwm", CLI_ARG_MWM,
     "The minimum weight magnitude is the number of trailing 0s that must "
     "appear in the end of a transaction hash. Increasing this number by 1 "
     "will result in proof of work that is 3 times as hard.",
     REQUIRED_ARG},
    {"neighbors", 'n', "URIs of peer iota nodes.", REQUIRED_ARG},
    {"p-remove-request", CLI_ARG_P_REMOVE_REQUEST,
     "A number between 0 and 1 that represents the probability of stopping to "
     "request a transaction.This number should be closer to 0 so non-existing "
     "transaction hashes will eventually be removed.",
     REQUIRED_ARG},
    {"p-select-milestone", CLI_ARG_P_SELECT_MILESTONE,
     "A number between 0 and 1 that represents the probability of requesting a "
     "milestone transaction from a neighbor. This should be large since it is "
     "imperative that we find milestones to get transactions confirmed.",
     REQUIRED_ARG},
    {"tcp-receiver-port", 't', "TCP receiver port.", REQUIRED_ARG},
    {"udp-receiver-port", 'u', "UDP receiver port.", REQUIRED_ARG},

    // API configuration

    {"max-get-trytes", CLI_ARG_MAX_GET_TRYTES,
     "The maximal number of trytes that may be returned by the \"getTrytes\" "
     "API call. If the number of transactions found exceeds this number an "
     "error will be returned.",
     REQUIRED_ARG},
    {"port", 'p', "The port that will be used by the API.", REQUIRED_ARG},

    // Consensus configuration

    {"alpha", CLI_ARG_ALPHA,
     "Parameter that defines the randomness of the tip selection. Should be a "
     "number between 0 to infinity, where 0 is most random and infinity is "
     "most deterministic.",
     REQUIRED_ARG},
    {"below-max-depth", CLI_ARG_BELOW_MAX_DEPTH,
     "The maximal number of unconfirmed transactions that may be analyzed in "
     "order to find the latest milestone the transaction that we are "
     "stepping on during the walk approves.",
     REQUIRED_ARG},
    {"max-depth", CLI_ARG_MAX_DEPTH,
     "The maximal number of previous milestones from where you can perform the "
     "random walk.",
     REQUIRED_ARG},

    {NULL, 0, NULL, NO_ARG}};

static char* short_options = "";

#ifdef __cplusplus
extern "C" {
#endif

void iota_usage();

#ifdef __cplusplus
}
#endif

#endif  // __CIRI_USAGE_H__
