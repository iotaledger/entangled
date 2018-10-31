/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __CIRI_CONF_CONF_H__
#define __CIRI_CONF_CONF_H__

#include <stdbool.h>
#include <stdint.h>

#include "common/errors.h"
#include "consensus/conf.h"
#include "gossip/conf.h"
#include "utils/logger_helper.h"

typedef struct iota_ciri_conf_s {
  logger_level_t log_level;
  uint16_t api_port;
  char *conf_file;
  bool remote;
  char *remote_auth_token;
  char *remote_limit_api;
  float send_limit;
  size_t max_peers;
  bool dns_resolution;
} iota_ciri_conf_t;

#ifdef __cplusplus
extern "C" {
#endif

retcode_t iota_ciri_conf_default(iota_ciri_conf_t *const ciri_conf,
                                 iota_gossip_conf_t *const gossip_conf,
                                 iota_consensus_conf_t *const consensus_conf);
retcode_t iota_ciri_conf_cli(iota_ciri_conf_t *const ciri_conf,
                             iota_gossip_conf_t *const gossip_conf,
                             iota_consensus_conf_t *const consensus_conf,
                             int argc, char **argv);

#ifdef __cplusplus
}
#endif

#endif  // __CIRI_CONF_CONF_H__
