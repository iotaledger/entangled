/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "ciri/perceptive_node/conf.h"
#include <string.h>

#define PN_DEFAULT_TEST_SAMPLE_SIZE 200
#define PN_DEFAULT_SEQUENCE_SIZE 50
#define PN_MIN_INTERVAL_SECONDS 60
#define PN_RANDOM_WALK_DEPTH 3
#define PN_MIN_HASH_POWER_RATIO 0.4

retcode_t iota_perceptive_node_conf_init(
    iota_perceptive_node_conf_t* const conf) {
  if (conf == NULL) {
    return RC_NULL_PARAM;
  }

  conf->test_sample_size = PN_DEFAULT_TEST_SAMPLE_SIZE;
  conf->monitored_transactions_sequence_size = PN_DEFAULT_SEQUENCE_SIZE;
  conf->monitoring_interval_seconds = PN_MIN_INTERVAL_SECONDS;
  conf->is_enabled = false;
  conf->random_walk_depth = PN_RANDOM_WALK_DEPTH;
  conf->min_hash_power_ratio = PN_MIN_HASH_POWER_RATIO;

  return RC_OK;
}
