/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __PERCEPTIVE_NODE_CONF_H__
#define __PERCEPTIVE_NODE_CONF_H__

#include <inttypes.h>
#include <stdbool.h>
#include "common/errors.h"

/**
 * The IOTA Perceptive node implementation
 */
typedef struct iota_perceptive_node_conf_s {
  bool is_enabled;
  // Path of the DB file
  char db_path[128];
  int32_t monitoring_interval_seconds;
  int32_t monitored_transactions_sequence_size;
  int32_t test_sample_size;
  uint16_t random_walk_depth;
} iota_perceptive_node_conf_t;

extern retcode_t iota_perceptive_node_conf_init(
    iota_perceptive_node_conf_t* const conf);

#endif  //__PERCEPTIVE_NODE_CONF_H__
