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
  node_t node;
  connection_t db_conn;
  connection_config_t db_conf;
} core_t;

bool core_init(core_t *const core);
bool core_start(core_t *const core);
bool core_stop(core_t *const core);
bool core_destroy(core_t *const core);

#endif  // __CIRI_CORE_H__
