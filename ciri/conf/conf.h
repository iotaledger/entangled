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
#include "utils/logger_helper.h"

typedef struct ciri_conf_s {
  logger_level_t log_level;
  uint16_t api_port;
  char *neighbors;
  char *conf_file;
  uint16_t tcp_receiver_port;
  uint16_t udp_receiver_port;
  bool testnet;
  bool remote;
  char *remote_auth_token;
  char *remote_limit_api;
  float send_limit;
  size_t max_peers;
  bool dns_resolution;
} ciri_conf_t;

#ifdef __cplusplus
extern "C" {
#endif

retcode_t ciri_conf_init(ciri_conf_t *conf);
retcode_t ciri_conf_parse(ciri_conf_t *conf, int argc, char **argv);

#ifdef __cplusplus
}
#endif

#endif  // __CIRI_CONF_CONF_H__
