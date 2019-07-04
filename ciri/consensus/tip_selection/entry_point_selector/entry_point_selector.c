/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <inttypes.h>

#include "ciri/consensus/milestone/milestone_tracker.h"
#include "ciri/consensus/tangle/tangle.h"
#include "ciri/consensus/tip_selection/entry_point_selector/entry_point_selector.h"
#include "common/model/milestone.h"
#include "utils/logger_helper.h"
#include "utils/macros.h"

#define ENTRY_POINT_SELECTOR_LOGGER_ID "entry_point_selector"

static logger_id_t logger_id;

retcode_t iota_consensus_entry_point_selector_init(entry_point_selector_t *const eps, milestone_tracker_t *const mt) {
  logger_id = logger_helper_enable(ENTRY_POINT_SELECTOR_LOGGER_ID, LOGGER_DEBUG, true);
  eps->mt = mt;
  return RC_OK;
}

retcode_t iota_consensus_entry_point_selector_get_entry_point(entry_point_selector_t *const eps, tangle_t *const tangle,
                                                              uint32_t const depth, flex_trit_t *const ep) {
  retcode_t ret = RC_OK;
  uint64_t milestone_index = MAX((int64_t)eps->mt->latest_solid_subtangle_milestone_index - depth - 1, 0);
  DECLARE_PACK_SINGLE_MILESTONE(milestone, milestone_ptr, pack);

  if ((ret = iota_tangle_milestone_load_by_index(tangle, milestone_index + 1, &pack))) {
    log_error(logger_id, "Finding closest next milestone failed with error %" PRIu64 "\n", ret);
    return ret;
  }

  if (pack.num_loaded != 0) {
    memcpy(ep, milestone.hash, FLEX_TRIT_SIZE_243);
  } else {
    memcpy(ep, eps->mt->latest_solid_subtangle_milestone, FLEX_TRIT_SIZE_243);
  }

  return RC_OK;
}

retcode_t iota_consensus_entry_point_selector_destroy(entry_point_selector_t *const eps) {
  logger_helper_release(logger_id);
  eps->mt = NULL;
  return RC_OK;
}
