/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __CIRI_CONF_CONF_H__
#define __CIRI_CONF_CONF_H__

#include <stdbool.h>

#include "ciri/api/conf.h"
#include "common/errors.h"
#include "consensus/conf.h"
#include "gossip/conf.h"
#include "utils/logger_helper.h"

#define DEFAULT_LOG_LEVEL LOGGER_INFO

#ifdef __cplusplus
extern "C" {
#endif

typedef struct iota_ciri_conf_s {
  logger_level_t log_level;
} iota_ciri_conf_t;

retcode_t iota_ciri_conf_default(iota_ciri_conf_t *const ciri_conf,
                                 iota_consensus_conf_t *const consensus_conf,
                                 iota_gossip_conf_t *const gossip_conf,
                                 iota_api_conf_t *const api_conf);
retcode_t iota_ciri_conf_cli(iota_ciri_conf_t *const ciri_conf,
                             iota_consensus_conf_t *const consensus_conf,
                             iota_gossip_conf_t *const gossip_conf,
                             iota_api_conf_t *const api_conf, int argc,
                             char **argv);

#ifdef __cplusplus
}
#endif

#endif  // __CIRI_CONF_CONF_H__
