/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <getopt.h>
#include <stdlib.h>
#include <string.h>

#include "ciri/conf.h"
#include "ciri/usage.h"

/*
 * Private functions
 */

static struct option* build_options() {
  size_t nbr = 0;
  while (cli_arguments_g[nbr].desc) {
    nbr++;
  }
  struct option* options = malloc((nbr + 1) * sizeof(struct option));
  size_t i;
  for (i = 0; i < nbr; i++) {
    options[i].name = cli_arguments_g[i].name;
    if (cli_arguments_g[i].has_arg == NO_ARG) {
      options[i].has_arg = no_argument;
    } else if (cli_arguments_g[i].has_arg == REQUIRED_ARG) {
      options[i].has_arg = required_argument;
    } else if (cli_arguments_g[i].has_arg == OPTIONAL_ARG) {
      options[i].has_arg = optional_argument;
    }
    options[i].flag = NULL;
    options[i].val = cli_arguments_g[i].val;
  }
  options[i].name = NULL;
  options[i].has_arg = 0;
  options[i].flag = NULL;
  options[i].val = 0;
  return options;
}

static logger_level_t get_log_level(char const* const log_level) {
  static struct log_level_map {
    char* str;
    logger_level_t level;
  } map[] = {{"debug", LOGGER_DEBUG},   {"info", LOGGER_INFO},
             {"notice", LOGGER_NOTICE}, {"warning", LOGGER_WARNING},
             {"error", LOGGER_ERR},     {"critical", LOGGER_CRIT},
             {"alert", LOGGER_ALERT},   {"emergency", LOGGER_EMERG},
             {NULL, LOGGER_INFO}};
  size_t i;
  for (i = 0; map[i].str != NULL && strcmp(map[i].str, log_level) != 0; i++)
    ;
  return map[i].level;
}

/*
 * Public functions
 */

retcode_t iota_ciri_conf_default(iota_ciri_conf_t* const ciri_conf,
                                 iota_consensus_conf_t* const consensus_conf,
                                 iota_gossip_conf_t* const gossip_conf,
                                 iota_api_conf_t* const api_conf) {
  retcode_t ret = RC_OK;

  if (ciri_conf == NULL || gossip_conf == NULL || consensus_conf == NULL) {
    return RC_NULL_PARAM;
  }

  ciri_conf->log_level = DEFAULT_LOG_LEVEL;
  strcpy(ciri_conf->db_path, DEFAULT_DB_PATH);

  if ((ret = iota_consensus_conf_init(consensus_conf)) != RC_OK) {
    return ret;
  }

  if ((ret = iota_gossip_conf_init(gossip_conf)) != RC_OK) {
    return ret;
  }

  if ((ret = iota_api_conf_init(api_conf)) != RC_OK) {
    return ret;
  }

  return ret;
}

retcode_t iota_ciri_conf_cli(iota_ciri_conf_t* const ciri_conf,
                             iota_consensus_conf_t* const consensus_conf,
                             iota_gossip_conf_t* const gossip_conf,
                             iota_api_conf_t* const api_conf, int argc,
                             char** argv) {
  int arg;
  retcode_t ret = RC_OK;
  struct option* long_options = build_options();

  while ((arg = getopt_long(argc, argv, short_options, long_options, NULL)) !=
         -1) {
    switch (arg) {
      // cIRI configuration
      case 'h':  // --help
        iota_usage();
        free(long_options);
        exit(EXIT_SUCCESS);
        break;
      case 'l':  // --log-level
        ciri_conf->log_level = get_log_level(optarg);
        break;
      case 'd':  // --db-path
        strcpy(ciri_conf->db_path, optarg);
        break;

      // Gossip configuration
      case CLI_ARG_MWM:  // --mwm
        gossip_conf->mwm = atoi(optarg);
        gossip_conf->request_hash_size_trit =
            HASH_LENGTH_TRIT - gossip_conf->mwm;
        consensus_conf->mwm = atoi(optarg);
        break;
      case 'n':  // --neighbors
        gossip_conf->neighbors = optarg;
        break;
      case CLI_ARG_P_PROPAGATE_REQUEST:  // --p-propagate-request
        gossip_conf->p_propagate_request = atof(optarg);
        break;
      case CLI_ARG_P_REPLY_RANDOM_TIP:  // --p-reply-random-tip
        gossip_conf->p_reply_random_tip = atof(optarg);
        break;
      case CLI_ARG_P_REMOVE_REQUEST:  // --p-remove-request
        gossip_conf->p_remove_request = atof(optarg);
        break;
      case CLI_ARG_P_SELECT_MILESTONE:  // --p-select-milestone
        gossip_conf->p_select_milestone = atof(optarg);
        break;
      case CLI_ARG_P_SEND_MILESTONE:  // --p-send-milestone
        gossip_conf->p_send_milestone = atof(optarg);
        break;
      case 't':  // --tcp-receiver-port
        gossip_conf->tcp_receiver_port = atoi(optarg);
        break;
      case CLI_ARG_TIPS_CACHE_SIZE:  // --tips-cache-size
        gossip_conf->tips_cache_size = atoi(optarg);
        break;
      case 'u':  // --udp-receiver-port
        gossip_conf->udp_receiver_port = atoi(optarg);
        break;

      // API configuration
      case CLI_ARG_MAX_GET_TRYTES:  // --max-get-trytes
        api_conf->max_get_trytes = atoi(optarg);
        break;
      case 'p':  // --port
        api_conf->port = atoi(optarg);
        break;

      // Consensus configuration
      case CLI_ARG_MAX_DEPTH:  // --max-depth
        consensus_conf->max_depth = atoi(optarg);
        break;
      case CLI_ARG_ALPHA:  // --alpha
        consensus_conf->alpha = atof(optarg);
        break;
      case CLI_ARG_BELOW_MAX_DEPTH:  // --below-max-depth
        consensus_conf->below_max_depth = atoi(optarg);
        break;

      default:
        iota_usage();
        ret = RC_CIRI_CONF_INVALID_ARGUMENTS;
        goto done;
    }
  }

done:
  free(long_options);
  return ret;
};
