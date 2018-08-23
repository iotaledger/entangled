/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __CIRI_CONF_PROVIDER_H__
#define __CIRI_CONF_PROVIDER_H__

#include <getopt.h>

#include "common/errors.h"
#include "ciri/conf/config.h"

retcode_t ciri_conf_init(ciri_config_t* config);
retcode_t ciri_conf_parse(ciri_config_t* out, size_t argc, char** argv);

#endif // __CIRI_CONF_PROVIDER_H__
