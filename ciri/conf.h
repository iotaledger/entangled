/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __CIRI_CONF_H__
#define __CIRI_CONF_H__

#include <stdbool.h>

#include "ciri/api/conf.h"
#include "ciri/consensus/conf.h"
#include "ciri/node/conf.h"
#include "ciri/utils/files.h"
#include "common/errors.h"
#include "utils/logger_helper.h"

#define DEFAULT_CONF_PATH "ciri/conf.yml"
#define DEFAULT_LOG_LEVEL LOGGER_INFO
#define DEFAULT_SPENT_ADDRESSES_DB_PATH SPENT_ADDRESSES_DB_PATH
#define DEFAULT_TANGLE_DB_PATH TANGLE_DB_PATH
#define DEFAULT_TANGLE_DB_REVALIDATE false

#ifdef __cplusplus
extern "C" {
#endif

// This structure contains all configuration variables needed to operate cIRI
typedef struct iota_ciri_conf_s {
  // Path to the configuration file
  char conf_path[FILE_PATH_SIZE];
  // Valid log levels: LOGGER_DEBUG, LOGGER_INFO, LOGGER_NOTICE,
  // LOGGER_WARNING, LOGGER_ERR, LOGGER_CRIT, LOGGER_ALERT and LOGGER_EMERG
  logger_level_t log_level;
  // Path of the spent addresses database file
  char spent_addresses_db_path[FILE_PATH_SIZE];
  // Path of the tangle database file
  char tangle_db_path[FILE_PATH_SIZE];
  // Reloads milestones, state of the ledger and transactions metadata from the tangle database
  bool tangle_db_revalidate;
} iota_ciri_conf_t;

/**
 * Initializes configurations with default values
 * Should be called first
 *
 * @param ciri_conf cIRI configuration variables
 * @param consensus_conf Consensus configuration variables
 * @param node_conf Node configuration variables
 * @param api_conf API configuration variables
 *
 * @return a status code
 */
retcode_t iota_ciri_conf_default_init(iota_ciri_conf_t *const ciri_conf, iota_consensus_conf_t *const consensus_conf,
                                      iota_node_conf_t *const node_conf, iota_api_conf_t *const api_conf);

/**
 * Initializes configurations with values from file
 * Should be called second
 *
 * @param ciri_conf cIRI configuration variables
 * @param consensus_conf Consensus configuration variables
 * @param node_conf Node configuration variables
 * @param api_conf API configuration variables
 * @param argc Number of arguments of the CLI
 * @param argv Arguments of the CLI
 *
 * @return a status code
 */
retcode_t iota_ciri_conf_file_init(iota_ciri_conf_t *const ciri_conf, iota_consensus_conf_t *const consensus_conf,
                                   iota_node_conf_t *const node_conf, iota_api_conf_t *const api_conf, int argc,
                                   char **argv);

/**
 * Initializes configurations with values from CLI
 * Should be called third
 *
 * @param ciri_conf cIRI configuration variables
 * @param consensus_conf Consensus configuration variables
 * @param node_conf Node configuration variables
 * @param api_conf API configuration variables
 * @param argc Number of arguments of the CLI
 * @param argv Arguments of the CLI
 *
 * @return a status code
 */
retcode_t iota_ciri_conf_cli_init(iota_ciri_conf_t *const ciri_conf, iota_consensus_conf_t *const consensus_conf,
                                  iota_node_conf_t *const node_conf, iota_api_conf_t *const api_conf, int argc,
                                  char **argv);

/**
 * Destroys configurations
 *
 * @param ciri_conf cIRI configuration variables
 * @param consensus_conf Consensus configuration variables
 * @param node_conf Node configuration variables
 * @param api_conf API configuration variables
 *
 * @return a status code
 */
retcode_t iota_ciri_conf_destroy(iota_ciri_conf_t *const ciri_conf, iota_consensus_conf_t *const consensus_conf,
                                 iota_node_conf_t *const node_conf, iota_api_conf_t *const api_conf);

#ifdef __cplusplus
}
#endif

#endif  // __CIRI_CONF_H__
