/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __CIRI_CORE_H__
#define __CIRI_CORE_H__

#include "ciri/conf/conf.h"
#include "ciri/node.h"
#include "consensus/tangle/tangle.h"

typedef struct core_s {
  bool running;
  ciri_conf_t config;
  node_t node;
  tangle_t tangle;
  connection_config_t db_conf;
} core_t;

/**
 * Initializes a core
 *
 * @param core The core
 *
 * @return a status code
 */
retcode_t core_init(core_t *const core);

/**
 * Starts a core
 *
 * @param core The core
 *
 * @return a status code
 */
retcode_t core_start(core_t *const core);

/**
 * Stops a core
 *
 * @param core The core
 *
 * @return a status code
 */
retcode_t core_stop(core_t *const core);

/**
 * Destroys a core
 *
 * @param core The core
 *
 * @return a status code
 */
retcode_t core_destroy(core_t *const core);

#endif  // __CIRI_CORE_H__
