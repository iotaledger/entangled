/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <getopt.h>
#include <stdlib.h>
#include <string.h>

#include "yaml.h"

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
  struct option* options =
      (struct option*)malloc((nbr + 1) * sizeof(struct option));
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

static int get_conf_key(char const* const key) {
  int i = 0;
  while (cli_arguments_g[i].name != NULL &&
         strcmp(cli_arguments_g[i].name, key) != 0) {
    i++;
  }
  return cli_arguments_g[i].val;
}

static retcode_t set_conf_value(iota_ciri_conf_t* const ciri_conf,
                                iota_consensus_conf_t* const consensus_conf,
                                iota_gossip_conf_t* const gossip_conf,
                                iota_api_conf_t* const api_conf, int const key,
                                char const* const value) {
  retcode_t ret = RC_OK;

  switch (key) {
    // cIRI configuration
    case 'd':  // --db-path
      strncpy(ciri_conf->db_path, value, sizeof(ciri_conf->db_path));
      strncpy(consensus_conf->db_path, value, sizeof(consensus_conf->db_path));
      strncpy(gossip_conf->db_path, value, sizeof(gossip_conf->db_path));
      strncpy(api_conf->db_path, value, sizeof(api_conf->db_path));
      break;
    case 'h':  // --help
      iota_usage();
      exit(EXIT_SUCCESS);
      break;
    case 'l':  // --log-level
      ciri_conf->log_level = get_log_level(value);
      break;

    // Gossip configuration
    case CONF_MWM:  // --mwm
      gossip_conf->mwm = atoi(value);
      gossip_conf->request_hash_size_trit = HASH_LENGTH_TRIT - gossip_conf->mwm;
      consensus_conf->mwm = atoi(value);
      break;
    case 'n':  // --neighbors
      gossip_conf->neighbors = strdup(value);
      break;
    case CONF_P_PROPAGATE_REQUEST:  // --p-propagate-request
      gossip_conf->p_propagate_request = atof(value);
      break;
    case CONF_P_REMOVE_REQUEST:  // --p-remove-request
      gossip_conf->p_remove_request = atof(value);
      break;
    case CONF_P_REPLY_RANDOM_TIP:  // --p-reply-random-tip
      gossip_conf->p_reply_random_tip = atof(value);
      break;
    case CONF_P_SELECT_MILESTONE:  // --p-select-milestone
      gossip_conf->p_select_milestone = atof(value);
      break;
    case CONF_P_SEND_MILESTONE:  // --p-send-milestone
      gossip_conf->p_send_milestone = atof(value);
      break;
    case CONF_REQUESTER_QUEUE_SIZE:  // --requester-queue-size
      gossip_conf->requester_queue_size = atoi(value);
      break;
    case 't':  // --tcp-receiver-port
      gossip_conf->tcp_receiver_port = atoi(value);
      break;
    case CONF_TIPS_CACHE_SIZE:  // --tips-cache-size
      gossip_conf->tips_cache_size = atoi(value);
      break;
    case 'u':  // --udp-receiver-port
      gossip_conf->udp_receiver_port = atoi(value);
      break;

    // API configuration
    case CONF_MAX_FIND_TRANSACTIONS:  // --max-find-transactions
      api_conf->max_find_transactions = atoi(value);
      break;
    case CONF_MAX_GET_TRYTES:  // --max-get-trytes
      api_conf->max_get_trytes = atoi(value);
      break;
    case 'p':  // --port
      api_conf->port = atoi(value);
      break;

    // Consensus configuration
    case CONF_ALPHA:  // --alpha
      consensus_conf->alpha = atof(value);
      break;
    case CONF_BELOW_MAX_DEPTH:  // --below-max-depth
      consensus_conf->below_max_depth = atoi(value);
      break;
    case CONF_COORDINATOR:  // --coordinator
      if (strlen(value) != HASH_LENGTH_TRYTE) {
        return RC_CIRI_CONF_INVALID_ARGUMENTS;
      }
      flex_trits_from_trytes(consensus_conf->coordinator, HASH_LENGTH_TRIT,
                             (tryte_t*)value, HASH_LENGTH_TRYTE,
                             HASH_LENGTH_TRYTE);
      break;
    case CONF_LAST_MILESTONE:  // --last-milestone
      consensus_conf->last_milestone = atoi(value);
      break;
    case CONF_MAX_DEPTH:  // --max-depth
      consensus_conf->max_depth = atoi(value);
      break;
    case CONF_NUM_KEYS_IN_MILESTONE:  // --num-keys-in-milestone
      consensus_conf->num_keys_in_milestone = atoi(value);
      break;
    case CONF_SNAPSHOT_FILE:  // --snapshot-file
      strcpy(consensus_conf->snapshot_file, value);
      break;
    case CONF_SNAPSHOT_SIGNATURE_DEPTH:  // --snapshot-signature-depth
      consensus_conf->snapshot_signature_depth = atoi(value);
      break;
    case CONF_SNAPSHOT_SIGNATURE_FILE:  // --snapshot-signature-file
      strcpy(consensus_conf->snapshot_signature_file, value);
      break;
    case CONF_SNAPSHOT_SIGNATURE_INDEX:  // --snapshot-signature-index
      consensus_conf->snapshot_signature_index = atoi(value);
      break;
    case CONF_SNAPSHOT_SIGNATURE_PUBKEY:  // --snapshot-signature-pubkey
      if (strlen(value) != HASH_LENGTH_TRYTE) {
        return RC_CIRI_CONF_INVALID_ARGUMENTS;
      }
      flex_trits_from_trytes(consensus_conf->snapshot_signature_pubkey,
                             HASH_LENGTH_TRIT, (tryte_t*)value,
                             HASH_LENGTH_TRYTE, HASH_LENGTH_TRYTE);
      break;
    case CONF_SNAPSHOT_TIMESTAMP:  // --snapshot-timestamp
      consensus_conf->snapshot_timestamp_sec = atoi(value);
      break;

    default:
      iota_usage();
      return RC_CIRI_CONF_INVALID_ARGUMENTS;
  }

  return ret;
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
  strncpy(ciri_conf->db_path, DEFAULT_DB_PATH, sizeof(ciri_conf->db_path));
  strncpy(consensus_conf->db_path, DEFAULT_DB_PATH,
          sizeof(consensus_conf->db_path));
  strncpy(gossip_conf->db_path, DEFAULT_DB_PATH, sizeof(gossip_conf->db_path));
  strncpy(api_conf->db_path, DEFAULT_DB_PATH, sizeof(api_conf->db_path));

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

retcode_t iota_ciri_conf_file(iota_ciri_conf_t* const ciri_conf,
                              iota_consensus_conf_t* const consensus_conf,
                              iota_gossip_conf_t* const gossip_conf,
                              iota_api_conf_t* const api_conf) {
  retcode_t ret = RC_OK;
  yaml_parser_t parser;
  yaml_token_t token;
  FILE* file = NULL;
  int key = 0;
  char* arg = NULL;
  int state = 0;

  if (ciri_conf == NULL || gossip_conf == NULL || consensus_conf == NULL) {
    return RC_NULL_PARAM;
  }

  if ((file = fopen("ciri/conf.yml", "r")) == NULL) {
    return RC_CIRI_CONF_FILE_NOT_FOUND;
  }

  if (!yaml_parser_initialize(&parser)) {
    return RC_CIRI_CONF_PARSER_ERROR;
  }
  yaml_parser_set_input_file(&parser, file);

  do {
    if (!yaml_parser_scan(&parser, &token)) {
      return RC_CIRI_CONF_PARSER_ERROR;
    }
    switch (token.type) {
      case YAML_KEY_TOKEN:
        state = 0;
        break;
      case YAML_VALUE_TOKEN:
        state = 1;
        break;
      case YAML_SCALAR_TOKEN:
        arg = (char*)token.data.scalar.value;
        if (state == 0) {  // Key
          key = get_conf_key(arg);
        } else {  // Value
          if ((ret = set_conf_value(ciri_conf, consensus_conf, gossip_conf,
                                    api_conf, key, arg) != RC_OK)) {
            goto done;
          }
        }
        break;
      default:
        break;
    }
    if (token.type != YAML_STREAM_END_TOKEN) {
      yaml_token_delete(&token);
    }
  } while (token.type != YAML_STREAM_END_TOKEN);

done:
  yaml_token_delete(&token);
  yaml_parser_delete(&parser);
  fclose(file);
  return ret;
}

retcode_t iota_ciri_conf_cli(iota_ciri_conf_t* const ciri_conf,
                             iota_consensus_conf_t* const consensus_conf,
                             iota_gossip_conf_t* const gossip_conf,
                             iota_api_conf_t* const api_conf, int argc,
                             char** argv) {
  int key;
  retcode_t ret = RC_OK;
  struct option* long_options = build_options();

  while ((key = getopt_long(argc, argv, short_options, long_options, NULL)) !=
         -1) {
    if ((ret = set_conf_value(ciri_conf, consensus_conf, gossip_conf, api_conf,
                              key, optarg) != RC_OK)) {
      break;
    }
  }

  free(long_options);
  return ret;
};
