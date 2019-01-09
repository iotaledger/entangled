/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "ciri/perceptive_node/conf.h"
#include <string.h>

#define DEFAULT_TEST_SAMPLE_SIZE 200
#define DEFAULT_SEQUENCE_SIZE 50
#define PERCEPTIVE_NODE_INTERVAL_SECONDS 60
#define PERCEPTIVE_NODE_RANDOM_WALK_DEPTH 3

retcode_t iota_perceptive_node_conf_init(
    iota_perceptive_node_conf_t* const conf) {
  if (conf == NULL) {
    return RC_NULL_PARAM;
  }

  conf->test_sample_size = DEFAULT_TEST_SAMPLE_SIZE;
  conf->monitored_transactions_sequence_size = DEFAULT_SEQUENCE_SIZE;
  conf->monitoring_interval_seconds = PERCEPTIVE_NODE_INTERVAL_SECONDS;
  conf->is_enabled = false;
  conf->random_walk_depth = PERCEPTIVE_NODE_RANDOM_WALK_DEPTH;

  return RC_OK;
}
