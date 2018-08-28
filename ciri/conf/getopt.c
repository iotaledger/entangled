/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <getopt.h>
#include <stdlib.h>

#include "ciri/conf/config_args.h"
#include "ciri/conf/config_default.h"
#include "ciri/conf/provider.h"

retcode_t ciri_conf_init(ciri_config_t* config) {
  if (config == NULL) {
    return RC_CONF_NULL_CONF;
  }
  config->tcp_port = TCP_PORT;
  config->udp_port = UDP_PORT;

  return RC_OK;
}

retcode_t ciri_conf_parse(ciri_config_t* out, size_t argc, char** argv) {
  int arg;

  while ((arg = getopt_long(argc, argv, short_options, long_options, NULL)) !=
         -1) {
    switch (arg) {
      case 't':
        out->tcp_port = atoi(optarg);
        break;
      case 'u':
        out->udp_port = atoi(optarg);
        break;
      default:
        break;
    }
  }
  return RC_OK;
};
