/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <getopt.h>
#include <stdlib.h>

#include "ciri/conf/conf.h"
#include "ciri/conf/usage.h"

static struct option* build_options() {
  size_t nbr = 0;
  while (ciri_arguments_g[nbr].desc) {
    nbr++;
  }
  struct option* options = malloc((nbr + 1) * sizeof(struct option));
  size_t i;
  for (i = 0; i < nbr; i++) {
    options[i].name = ciri_arguments_g[i].name;
    if (ciri_arguments_g[i].has_arg == NO_ARG) {
      options[i].has_arg = no_argument;
    } else if (ciri_arguments_g[i].has_arg == REQUIRED_ARG) {
      options[i].has_arg = required_argument;
    } else if (ciri_arguments_g[i].has_arg == OPTIONAL_ARG) {
      options[i].has_arg = optional_argument;
    }
    options[i].flag = NULL;
    options[i].val = ciri_arguments_g[i].val;
  }
  options[i].name = NULL;
  options[i].has_arg = 0;
  options[i].flag = NULL;
  options[i].val = 0;
  return options;
}

retcode_t ciri_conf_parse(ciri_conf_t* conf, int argc, char** argv) {
  int arg;

  struct option* long_options = build_options();
  while ((arg = getopt_long(argc, argv, short_options, long_options, NULL)) !=
         -1) {
    switch (arg) {
      case 'h':  // --help
        iota_usage();
        free(long_options);
        exit(EXIT_SUCCESS);
        break;
      case 'p':  // --port
        conf->api_port = atoi(optarg);
        break;
      case 'n':  // --neighbors
        conf->neighbors = optarg;
        break;
      case 'c':  // --config
        conf->conf_file = optarg;
        break;
      case 'u':  // --udp-receiver-port
        conf->udp_receiver_port = atoi(optarg);
        break;
      case 't':  // --tcp-receiver-port
        conf->tcp_receiver_port = atoi(optarg);
        break;
      case 'e':  // --testnet
        conf->testnet = true;
        break;
      case 'r':  // --remote
        conf->remote = true;
        break;
      case 'a':  // --remote-auth
        conf->remote_auth_token = optarg;
        break;
      case 'l':  // --remote-limit-api
        conf->remote_limit_api = optarg;
        break;
      case 's':  // --send-limit
        conf->send_limit = atof(optarg);
        break;
      case 'm':  // --max-peers
        conf->max_peers = atoi(optarg);
        break;
      case 'd':  // --dns-resolution-false
        conf->dns_resolution = false;
        break;
      default:
        iota_usage();
        free(long_options);
        return RC_CIRI_CONF_INVALID_ARGUMENTS;
    }
  }
  free(long_options);
  return RC_OK;
};
