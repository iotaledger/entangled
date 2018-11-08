/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "consensus/conf.h"

retcode_t iota_consensus_conf_init(iota_consensus_conf_t* const conf) {
  if (conf == NULL) {
    return RC_NULL_PARAM;
  }

  memset(conf->genesis_hash, FLEX_TRIT_NULL_VALUE, FLEX_TRIT_SIZE_243);
  conf->max_depth = DEFAULT_TIP_SELECTION_MAX_DEPTH;
  conf->alpha = DEFAULT_TIP_SELECTION_ALPHA;
  conf->below_max_depth = DEFAULT_TIP_SELECTION_BELOW_MAX_DEPTH;
  strcpy(conf->snapshot_conf_file, DEFAULT_SNAPSHOT_CONF_FILE);
  strcpy(conf->snapshot_sig_file, DEFAULT_SNAPSHOT_SIG_FILE);
  strcpy(conf->snapshot_file, DEFAULT_SNAPSHOT_FILE);
  conf->num_keys_in_milestone = DEFAULT_NUM_KEYS_IN_MILESTONE;
  conf->mwm = DEFAULT_MWN;

  return RC_OK;
}
