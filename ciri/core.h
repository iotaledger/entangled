/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __CIRI_CORE_H__
#define __CIRI_CORE_H__

#include "ciri/node.h"
#include "common/storage/connection.h"

typedef struct core_s {
  bool running;
  node_t node;
  connection_t db_conn;
  connection_config_t db_conf;
} core_t;

retcode_t core_init(core_t *const core);
retcode_t core_start(core_t *const core);
retcode_t core_stop(core_t *const core);
retcode_t core_destroy(core_t *const core);

#endif  // __CIRI_CORE_H__
