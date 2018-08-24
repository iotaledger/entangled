/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "consensus/entry_point_selector/entry_point_selector.h"
#include "common/errors.h"
#include "common/storage/storage.h"
#include "consensus/view_models/milestone.h"

#define ENTRY_POINT_SELECTOR_LOGGER_ID "consensus_entry_point_selector"
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

retcode_t iota_consensus_entry_point_selector_init(
    bool testnet, entry_point_selector_t *ep_selector) {
  logger_helper_init(ENTRY_POINT_SELECTOR_LOGGER_ID, LOGGER_INFO, true);
  return RC_CONSENSUS_NOT_IMPLEMENTED;
}

retcode_t iota_consensus_entry_point_selector_destroy(
    entry_point_selector_t *ep_selector) {
  logger_helper_destroy(ENTRY_POINT_SELECTOR_LOGGER_ID);
  ep_selector->milestone = NULL;
  ep_selector->tangle = NULL;
  return RC_OK;
}

retcode_t iota_consensus_get_entry_point(
    const entry_point_selector_t *ep_selector, size_t depth, trit_array_p ep) {
  size_t idx =
      MAX(ep_selector->milestone->latest_milestone_index - depth - 1, 0);
  milestone_view_t msv = NULL;
  if (iota_consencues_find_closest_next_milestone(
          ep_selector->tangle, idx, ep_selector->testnet,
          ep_selector->start_index, msv) != RC_OK) {
    // TODO
  }
  if (msv == NULL) {
    msv = ep_selector->milestone->latest_solid_milestone_hash;
  }

  return RC_OK;
}
