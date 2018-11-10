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
#include "common/errors.h"
#include "consensus/conf.h"
#include "gossip/conf.h"
#include "utils/logger_helper.h"

#define DEFAULT_LOG_LEVEL LOGGER_INFO
#define DEFAULT_DB_PATH DB_PATH

#ifdef __cplusplus
extern "C" {
#endif

// This structure contains all configuration variables needed to operate
// cIRI
typedef struct iota_ciri_conf_s {
  // Valid log levels: LOGGER_DEBUG, LOGGER_INFO, LOGGER_NOTICE,
  // LOGGER_WARNING, LOGGER_ERR, LOGGER_CRIT, LOGGER_ALERT and LOGGER_EMERG
  logger_level_t log_level;
  // Path of the DB file
  char db_path[128];
} iota_ciri_conf_t;

/**
 * Initializes configurations with default values
 * Should be called first
 *
 * @param ciri_conf cIRI configuration variables
 * @param consensus_conf Consensus configuration variables
 * @param gossip_conf Gossip configuration variables
 * @param api_conf API configuration variables
 *
 * @return a status code
 */
retcode_t iota_ciri_conf_default(iota_ciri_conf_t *const ciri_conf,
                                 iota_consensus_conf_t *const consensus_conf,
                                 iota_gossip_conf_t *const gossip_conf,
                                 iota_api_conf_t *const api_conf);

/**
 * Initializes configurations with values from file
 * Should be called second
 *
 * @param ciri_conf cIRI configuration variables
 * @param consensus_conf Consensus configuration variables
 * @param gossip_conf Gossip configuration variables
 * @param api_conf API configuration variables
 *
 * @return a status code
 */
retcode_t iota_ciri_conf_file(iota_ciri_conf_t *const ciri_conf,
                              iota_consensus_conf_t *const consensus_conf,
                              iota_gossip_conf_t *const gossip_conf,
                              iota_api_conf_t *const api_conf);

/**
 * Initializes configurations with values from CLI
 * Should be called third
 *
 * @param ciri_conf cIRI configuration variables
 * @param consensus_conf Consensus configuration variables
 * @param gossip_conf Gossip configuration variables
 * @param argc Number of arguments of the CLI
 * @param argv Arguments of the CLI
 *
 * @return a status code
 */
retcode_t iota_ciri_conf_cli(iota_ciri_conf_t *const ciri_conf,
                             iota_consensus_conf_t *const consensus_conf,
                             iota_gossip_conf_t *const gossip_conf,
                             iota_api_conf_t *const api_conf, int argc,
                             char **argv);

#ifdef __cplusplus
}
#endif

#endif  // __CIRI_CONF_H__
