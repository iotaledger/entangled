/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <inttypes.h>

#include "common/model/milestone.h"
#include "consensus/entry_point_selector/entry_point_selector.h"
#include "consensus/milestone_tracker/milestone_tracker.h"
#include "consensus/tangle/tangle.h"
#include "utils/logger_helper.h"
#include "utils/macros.h"

#define ENTRY_POINT_SELECTOR_LOGGER_ID "consensus_entry_point_selector"

retcode_t iota_consensus_entry_point_selector_init(
    entry_point_selector_t *const eps, milestone_tracker_t *const mt,
    tangle_t *const tangle) {
  logger_helper_init(ENTRY_POINT_SELECTOR_LOGGER_ID, LOGGER_DEBUG, true);
  eps->mt = mt;
  eps->tangle = tangle;
  return RC_OK;
}

retcode_t iota_consensus_entry_point_selector_get_entry_point(
    entry_point_selector_t *const eps, size_t const depth,
    trit_array_p const ep) {
  retcode_t ret = RC_OK;
  uint64_t milestone_index = MAX(
      (int64_t)eps->mt->latest_solid_subtangle_milestone_index - depth - 1, 0);
  DECLARE_PACK_SINGLE_MILESTONE(milestone, milestone_ptr, pack);

  if ((ret = iota_tangle_milestone_load_next(eps->tangle, milestone_index,
                                             &pack))) {
    log_error(ENTRY_POINT_SELECTOR_LOGGER_ID,
              "Finding closest next milestone failed with error %" PRIu64 "\n",
              ret);
    return ret;
  }

  if (pack.num_loaded != 0) {
    memcpy(ep->trits, milestone.hash, FLEX_TRIT_SIZE_243);
  } else {
    memcpy(ep->trits, eps->mt->latest_solid_subtangle_milestone->trits,
           FLEX_TRIT_SIZE_243);
  }

  return RC_OK;
}

retcode_t iota_consensus_entry_point_selector_destroy(
    entry_point_selector_t *const eps) {
  logger_helper_destroy(ENTRY_POINT_SELECTOR_LOGGER_ID);
  eps->mt = NULL;
  eps->tangle = NULL;
  return RC_OK;
}
