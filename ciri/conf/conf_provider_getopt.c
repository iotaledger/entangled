/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <getopt.h>
#include <stdlib.h>

#include "ciri/conf/conf.h"
#include "ciri/conf/conf_args.h"

retcode_t ciri_conf_parse(ciri_conf_t* conf, int argc, char** argv) {
  int arg;

  while ((arg = getopt_long(argc, argv, short_options, long_options, NULL)) !=
         -1) {
    switch (arg) {
      case 't':
        conf->tcp_receiver_port = atoi(optarg);
        break;
      case 'u':
        conf->udp_receiver_port = atoi(optarg);
        break;
      default:
        return RC_CONF_INVALID_ARGUMENTS;
    }
  }
  return RC_OK;
};
