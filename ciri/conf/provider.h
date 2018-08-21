/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __COMMON_CONFIG_PROVIDER__
#define __COMMON_CONFIG_PROVIDER__
#include <getopt.h>

#include "common/errors.h"
#include "ciri/conf/config.h"

ciri_config_t* ciri_conf_init();
retcode_t ciri_conf_parse(ciri_config_t* out, size_t argc, char** argv);

#endif // __COMMON_CONFIGUR_PROVIDER__
