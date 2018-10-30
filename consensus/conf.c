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

  // Miscellanous conf

  memset(conf->genesis_hash, FLEX_TRIT_NULL_VALUE, FLEX_TRIT_SIZE_243);

  // Tip selection conf

  conf->max_depth = DEFAULT_TIP_SELECTION_MAX_DEPTH;
  conf->alpha = DEFAULT_TIP_SELECTION_ALPHA;
  conf->below_max_depth = DEFAULT_TIP_SELECTION_BELOW_MAX_DEPTH;

  return RC_OK;
}
