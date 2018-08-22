#include <stdlib.h>

#include "ciri/conf/provider.h"
#include "ciri/conf/config_args.h"

retcode_t ciri_conf_init(ciri_config_t* config) {
  if (config == NULL) {
      return 1;
  }
  config->tcp_port = 14260;
  config->udp_port = 14261;

  return RC_OK;
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
