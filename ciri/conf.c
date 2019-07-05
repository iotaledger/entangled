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

static char* short_options = "c:d:hl:n:p:t:u:";

/*
 * Private functions
 */

static struct option* build_options() {
  struct option* options = NULL;
  size_t nbr = 0;
  size_t i = 0;

  while (cli_arguments_g[nbr].desc) {
    nbr++;
  }
  options = (struct option*)malloc((nbr + 1) * sizeof(struct option));

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

static int get_conf_key(char const* const key) {
  int i = 0;

  while (cli_arguments_g[i].name != NULL && strcmp(cli_arguments_g[i].name, key) != 0) {
    i++;
  }

  return cli_arguments_g[i].val;
}

static char const* get_conf_name(int const key) {
  int i = 0;

  while (cli_arguments_g[i].name != NULL && cli_arguments_g[i].val != key) {
    i++;
  }

  return cli_arguments_g[i].name;
}

static char const* get_conf_desc(int const key) {
  int i = 0;

  while (cli_arguments_g[i].name != NULL && cli_arguments_g[i].val != key) {
    i++;
  }
  return cli_arguments_g[i].desc;
}

static void print_invalid_argument(int const key) {
  fprintf(stderr, "Invalid argument for parameter \'%s\'.\n", get_conf_name(key));
  fprintf(stderr, "Usage: %s\n", get_conf_desc(key));
}

static retcode_t get_log_level(char const* const input, logger_level_t* const output) {
  static struct log_level_map {
    char* str;
    logger_level_t level;
  } map[] = {{"debug", LOGGER_DEBUG},     {"info", LOGGER_INFO},       {"notice", LOGGER_NOTICE},
             {"warning", LOGGER_WARNING}, {"error", LOGGER_ERR},       {"critical", LOGGER_CRIT},
             {"alert", LOGGER_ALERT},     {"emergency", LOGGER_EMERG}, {NULL, LOGGER_UNKNOWN}};
  size_t i;

  for (i = 0; map[i].str != NULL && strcmp(map[i].str, input) != 0; i++) {
  }

  if ((*output = map[i].level) == LOGGER_UNKNOWN) {
    return RC_CONF_INVALID_ARGUMENT;
  }

  return RC_OK;
}

static retcode_t get_sponge_type(char const* const input, sponge_type_t* const output) {
  static struct sponge_type_map {
    char* str;
    sponge_type_t type;
  } map[] = {{"CURL_P27", SPONGE_CURLP27}, {"CURL_P81", SPONGE_CURLP81}, {"KERL", SPONGE_KERL}, {NULL, SPONGE_UNKNOWN}};
  size_t i;

  for (i = 0; map[i].str != NULL && strcmp(map[i].str, input) != 0; i++) {
  }

  if ((*output = map[i].type) == SPONGE_UNKNOWN) {
    return RC_CONF_INVALID_ARGUMENT;
  }

  return RC_OK;
}

static retcode_t get_probability(char const* const input, double* const output) {
  *output = atof(input);
  if (*output < 0 || *output > 1) {
    return RC_CONF_INVALID_ARGUMENT;
  }

  return RC_OK;
}

static retcode_t get_trytes(char const* const input, flex_trit_t* const output, size_t const length) {
  if (strlen(input) != length) {
    return RC_CONF_INVALID_ARGUMENT;
  }
  for (size_t i = 0; i < length; i++) {
    if (INDEX_OF_TRYTE(input[i]) < 0 || INDEX_OF_TRYTE(input[i]) > 26) {
      return RC_CONF_INVALID_ARGUMENT;
    }
  }
  flex_trits_from_trytes(output, length * 3, (tryte_t*)input, length, length);

  return RC_OK;
}

static retcode_t get_true_false(char const* const input, bool* const output) {
  if (strcmp(input, "true") == 0) {
    *output = true;
  } else if (strcmp(input, "false") == 0) {
    *output = false;
  } else {
    return RC_CONF_INVALID_ARGUMENT;
  }
  return RC_OK;
}

static retcode_t set_conf_value(iota_ciri_conf_t* const ciri_conf, iota_consensus_conf_t* const consensus_conf,
                                iota_node_conf_t* const node_conf, iota_api_conf_t* const api_conf, int const key,
                                char const* const value) {
  retcode_t ret = RC_OK;

  switch (key) {
    // cIRI configuration
    case 'c':  // --config
      if (strlen(optarg) == 0) {
        return RC_CONF_INVALID_ARGUMENT;
      }
      strncpy(ciri_conf->conf_path, optarg, sizeof(ciri_conf->conf_path));
      break;
    case 'h':  // --help
      iota_usage();
      exit(EXIT_SUCCESS);
      break;
    case 'l':  // --log-level
      ret = get_log_level(value, &ciri_conf->log_level);
      break;
    case CONF_SPENT_ADDRESSES_DB_PATH:  // --spent-addresses-db-path
      if (strlen(value) == 0) {
        return RC_CONF_INVALID_ARGUMENT;
      }
      strncpy(ciri_conf->spent_addresses_db_path, value, sizeof(ciri_conf->spent_addresses_db_path));
      strncpy(consensus_conf->spent_addresses_db_path, value, sizeof(consensus_conf->spent_addresses_db_path));
      strncpy(api_conf->spent_addresses_db_path, value, sizeof(api_conf->spent_addresses_db_path));
      break;
    case CONF_TANGLE_DB_PATH:  // --tangle-db-path
      if (strlen(value) == 0) {
        return RC_CONF_INVALID_ARGUMENT;
      }
      strncpy(ciri_conf->tangle_db_path, value, sizeof(ciri_conf->tangle_db_path));
      strncpy(consensus_conf->tangle_db_path, value, sizeof(consensus_conf->tangle_db_path));
      strncpy(node_conf->tangle_db_path, value, sizeof(node_conf->tangle_db_path));
      strncpy(api_conf->tangle_db_path, value, sizeof(api_conf->tangle_db_path));
      break;
    case CONF_TANGLE_DB_REVALIDATE:  // --tangle-db-revalidate
      ret = get_true_false(value, &ciri_conf->tangle_db_revalidate);
      break;

    // Node configuration
    case CONF_MWM:  // --mwm
      node_conf->mwm = atoi(value);
      node_conf->request_hash_size_trit = HASH_LENGTH_TRIT - node_conf->mwm;
      consensus_conf->mwm = atoi(value);
      break;
    case 'n':  // --neighbors
      if (strlen(value) == 0) {
        return RC_CONF_INVALID_ARGUMENT;
      }
      node_conf->neighbors = strdup(value);
      break;
    case CONF_P_PROPAGATE_REQUEST:  // --p-propagate-request
      ret = get_probability(value, &node_conf->p_propagate_request);
      break;
    case CONF_P_REMOVE_REQUEST:  // --p-remove-request
      ret = get_probability(value, &node_conf->p_remove_request);
      break;
    case CONF_P_REPLY_RANDOM_TIP:  // --p-reply-random-tip
      ret = get_probability(value, &node_conf->p_reply_random_tip);
      break;
    case CONF_P_SELECT_MILESTONE:  // --p-select-milestone
      ret = get_probability(value, &node_conf->p_select_milestone);
      break;
    case CONF_P_SEND_MILESTONE:  // --p-send-milestone
      ret = get_probability(value, &node_conf->p_send_milestone);
      break;
    case CONF_RECENT_SEEN_BYTES_CACHE_SIZE:  // --recent-seen-bytes-cache-size
      node_conf->recent_seen_bytes_cache_size = atoi(value);
      break;
    case CONF_REQUESTER_QUEUE_SIZE:  // --requester-queue-size
      node_conf->requester_queue_size = atoi(value);
      break;
    case 't':  // --tcp-receiver-port
      node_conf->tcp_receiver_port = atoi(value);
      break;
    case CONF_TIPS_CACHE_SIZE:  // --tips-cache-size
      node_conf->tips_cache_size = atoi(value);
      break;
    case 'u':  // --udp-receiver-port
      node_conf->udp_receiver_port = atoi(value);
      break;
    case CONF_TIPS_SOLIDIFIER_ENABLED:  // --tips-solidifier-enabled
      ret = get_true_false(value, &node_conf->tips_solidifier_enabled);
      break;

    // API configuration
    case 'p':  // --http_port
      api_conf->http_port = atoi(value);
      break;
    case CONF_MAX_FIND_TRANSACTIONS:  // --max-find-transactions
      api_conf->max_find_transactions = atoi(value);
      break;
    case CONF_MAX_GET_TRYTES:  // --max-get-trytes
      api_conf->max_get_trytes = atoi(value);
      break;
    case CONF_REMOTE_LIMIT_API:  // --remote-limit-api
    {
      char *token = NULL, *str = NULL, *free_str = NULL;
      size_t i = 0;

      if ((free_str = str = strdup(value)) == NULL) {
        return RC_OOM;
      }

      while ((token = strsep(&str, ", \t")) != NULL) {
        if (token[0]) {
          if (i >= API_ENDPOINTS_NUM) {
            free(free_str);
            return RC_CONF_INVALID_ARGUMENT;
          }
          api_conf->remote_limit_api[i++] = strdup(token);
        }
      }

      free(free_str);
      break;
    }

    // Consensus configuration
    case CONF_ALPHA:  // --alpha
      consensus_conf->alpha = atof(value);
      break;
    case CONF_BELOW_MAX_DEPTH:  // --below-max-depth
      consensus_conf->below_max_depth = atoi(value);
      break;
    case CONF_COORDINATOR_ADDRESS:  // --coordinator-address
      ret = get_trytes(value, consensus_conf->coordinator_address, HASH_LENGTH_TRYTE);
      if (ret == RC_OK) {
        ret = get_trytes(value, node_conf->coordinator_address, HASH_LENGTH_TRYTE);
      }
      break;
    case CONF_COORDINATOR_DEPTH:  // --coordinator-depth
      consensus_conf->coordinator_depth = atoi(value);
      consensus_conf->coordinator_max_milestone_index = 1 << consensus_conf->coordinator_depth;
      break;
    case CONF_COORDINATOR_SECURITY_LEVEL:  // --coordinator-security-level
    {
      int security_level = atoi(value);

      if (security_level < 0 || security_level > 3) {
        return RC_CONF_INVALID_ARGUMENT;
      }
      consensus_conf->coordinator_security_level = (uint8_t)security_level;
      break;
    }
    case CONF_COORDINATOR_SIGNATURE_TYPE:  // --coordinator-signature-type
      ret = get_sponge_type(value, &consensus_conf->coordinator_signature_type);
      break;
    case CONF_LAST_MILESTONE:  // --last-milestone
      consensus_conf->last_milestone = atoi(value);
      break;
    case CONF_MAX_DEPTH:  // --max-depth
      consensus_conf->max_depth = atoi(value);
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
      ret = get_trytes(value, consensus_conf->snapshot_signature_pubkey, HASH_LENGTH_TRYTE);
      break;
    case CONF_SNAPSHOT_SIGNATURE_SKIP_VALIDATION:  // --snapshot-signature-skip-validation
      ret = get_true_false(value, &consensus_conf->snapshot_signature_skip_validation);
      break;
    case CONF_SNAPSHOT_TIMESTAMP:  // --snapshot-timestamp
      consensus_conf->snapshot_timestamp_sec = atoi(value);
      break;
    case CONF_SPENT_ADDRESSES_FILES:  // --spent-addresses-files
      consensus_conf->spent_addresses_files = (char*)value;
      break;

      // Local snapshots configuration
    case CONF_LOCAL_SNAPSHOTS_ENABLED:
      ret = get_true_false(value, &consensus_conf->local_snapshots.local_snapshots_is_enabled);
      break;
    case CONF_LOCAL_SNAPSHOTS_PRUNNING_ENABLED:
      ret = get_true_false(value, &consensus_conf->local_snapshots.prunning_is_enabled);
      break;
    case CONF_LOCAL_SNAPSHOTS_TRANSACTIONS_GROWTH_THRESHOLD:
      consensus_conf->local_snapshots.transactions_growth_threshold = atoi(value);
      break;
    case CONF_LOCAL_SNAPSHOTS_MIN_DEPTH:
      consensus_conf->local_snapshots.min_depth = atoi(value);
      break;
    case CONF_LOCAL_SNAPSHOTS_PATH_BASE:
      strncpy(consensus_conf->local_snapshots.local_snapshots_path_base, value,
              sizeof(consensus_conf->local_snapshots.local_snapshots_path_base));
      break;

    default:
      iota_usage();
      return RC_CONF_INVALID_ARGUMENT;
  }

  return ret;
}

/*
 * Public functions
 */

retcode_t iota_ciri_conf_default_init(iota_ciri_conf_t* const ciri_conf, iota_consensus_conf_t* const consensus_conf,
                                      iota_node_conf_t* const node_conf, iota_api_conf_t* const api_conf) {
  retcode_t ret = RC_OK;

  if (ciri_conf == NULL || consensus_conf == NULL || node_conf == NULL || api_conf == NULL) {
    return RC_NULL_PARAM;
  }

  ciri_conf->log_level = DEFAULT_LOG_LEVEL;
  strncpy(ciri_conf->conf_path, DEFAULT_CONF_PATH, sizeof(ciri_conf->conf_path));
  strncpy(ciri_conf->tangle_db_path, DEFAULT_TANGLE_DB_PATH, sizeof(ciri_conf->tangle_db_path));
  strncpy(consensus_conf->tangle_db_path, DEFAULT_TANGLE_DB_PATH, sizeof(consensus_conf->tangle_db_path));
  strncpy(node_conf->tangle_db_path, DEFAULT_TANGLE_DB_PATH, sizeof(node_conf->tangle_db_path));
  strncpy(api_conf->tangle_db_path, DEFAULT_TANGLE_DB_PATH, sizeof(api_conf->tangle_db_path));
  strncpy(ciri_conf->spent_addresses_db_path, DEFAULT_SPENT_ADDRESSES_DB_PATH,
          sizeof(ciri_conf->spent_addresses_db_path));
  strncpy(consensus_conf->spent_addresses_db_path, DEFAULT_SPENT_ADDRESSES_DB_PATH,
          sizeof(consensus_conf->spent_addresses_db_path));
  strncpy(api_conf->spent_addresses_db_path, DEFAULT_SPENT_ADDRESSES_DB_PATH,
          sizeof(api_conf->spent_addresses_db_path));
  ciri_conf->tangle_db_revalidate = DEFAULT_TANGLE_DB_REVALIDATE;

  if ((ret = iota_consensus_conf_init(consensus_conf)) != RC_OK) {
    return ret;
  }

  if ((ret = iota_node_conf_init(node_conf)) != RC_OK) {
    return ret;
  }

  if ((ret = iota_api_conf_init(api_conf)) != RC_OK) {
    return ret;
  }

  return ret;
}

retcode_t iota_ciri_conf_file_init(iota_ciri_conf_t* const ciri_conf, iota_consensus_conf_t* const consensus_conf,
                                   iota_node_conf_t* const node_conf, iota_api_conf_t* const api_conf, int argc,
                                   char** argv) {
  retcode_t ret = RC_OK;
  yaml_parser_t parser;
  yaml_token_t token;
  FILE* file = NULL;
  int key = 0;
  char* arg = NULL;
  int state = 0;
  struct option* long_options = build_options();

  if (ciri_conf == NULL || consensus_conf == NULL || node_conf == NULL || api_conf == NULL) {
    ret = RC_NULL_PARAM;
    goto done;
  }

  if (!yaml_parser_initialize(&parser)) {
    ret = RC_CONF_PARSER_ERROR;
    goto done;
  }

  // Looping through the CLI arguments a first time to find the configuration file path
  while ((key = getopt_long(argc, argv, short_options, long_options, NULL)) != -1) {
    if (key == ':') {
      ret = RC_CONF_MISSING_ARGUMENT;
      goto done;
    } else if (key == '?') {
      ret = RC_CONF_UNKNOWN_OPTION;
      goto done;
    } else if (key == 'c') {  // configure file path
      if ((ret = set_conf_value(ciri_conf, consensus_conf, node_conf, api_conf, key, optarg)) != RC_OK) {
        goto done;
      }
    }
  }
  // Resetting the CLI arguments for the second loop where they are actually analyzed
  optind = 1;

  if ((file = fopen(ciri_conf->conf_path, "r")) == NULL) {
    ret = RC_OK;
    goto done;
  }

  yaml_parser_set_input_file(&parser, file);

  do {
    if (!yaml_parser_scan(&parser, &token)) {
      ret = RC_CONF_PARSER_ERROR;
      goto done;
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
          if ((ret = set_conf_value(ciri_conf, consensus_conf, node_conf, api_conf, key, arg)) != RC_OK) {
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

  if (ret == RC_CONF_INVALID_ARGUMENT) {
    print_invalid_argument(key);
  }
  yaml_token_delete(&token);
  yaml_parser_delete(&parser);
  if (file) {
    fclose(file);
  }
  free(long_options);

  return ret;
}

retcode_t iota_ciri_conf_cli_init(iota_ciri_conf_t* const ciri_conf, iota_consensus_conf_t* const consensus_conf,
                                  iota_node_conf_t* const node_conf, iota_api_conf_t* const api_conf, int argc,
                                  char** argv) {
  int key = 0;
  retcode_t ret = RC_OK;
  struct option* long_options = build_options();

  if (ciri_conf == NULL || consensus_conf == NULL || node_conf == NULL || api_conf == NULL) {
    ret = RC_NULL_PARAM;
    goto done;
  }

  while ((key = getopt_long(argc, argv, short_options, long_options, NULL)) != -1) {
    if (key == ':') {
      ret = RC_CONF_MISSING_ARGUMENT;
      break;
    } else if (key == '?') {
      ret = RC_CONF_UNKNOWN_OPTION;
      break;
    } else if ((ret = set_conf_value(ciri_conf, consensus_conf, node_conf, api_conf, key, optarg)) != RC_OK) {
      break;
    }
  }

done:

  if (ret == RC_CONF_INVALID_ARGUMENT) {
    print_invalid_argument(key);
  }
  free(long_options);

  return ret;
}

retcode_t iota_ciri_conf_destroy(iota_ciri_conf_t* const ciri_conf, iota_consensus_conf_t* const consensus_conf,
                                 iota_node_conf_t* const node_conf, iota_api_conf_t* const api_conf) {
  if (ciri_conf == NULL || consensus_conf == NULL || node_conf == NULL || api_conf == NULL) {
    return RC_NULL_PARAM;
  }

  iota_api_conf_destroy(api_conf);

  return RC_OK;
}
