#include <stdlib.h>
#include <string.h>

#include "ciri/conf/provider.h"
#include "ciri/conf/config_args.h"

ciri_config_t* ciri_conf_init() {
  ciri_config_t *config = (ciri_config_t* )malloc(sizeof(ciri_config_t));
  if (config == NULL) {
      return NULL;
  }
  memset(config, 0, sizeof(ciri_config_t));
  config->tcp_port = 14260;
  config->udp_port = 14261;

  return config;
}

retcode_t ciri_conf_parse(ciri_config_t* out, size_t argc, char** argv) {
  int arg;
  while ((arg = getopt_long(argc, argv, short_options, long_options, NULL)) != -1) {
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
