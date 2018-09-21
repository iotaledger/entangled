/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "consensus/ledger_validator/ledger_validator.h"
#include "common/model/milestone.h"
#include "common/storage/pack.h"
#include "consensus/milestone_tracker/milestone_tracker.h"
#include "consensus/snapshot/snapshot.h"
#include "consensus/tangle/tangle.h"
#include "utils/logger_helper.h"

#define LEDGER_VALIDATOR_LOGGER_ID "consensus_ledger_validator"

static retcode_t build_snapshot(ledger_validator_t *const lv,
                                iota_milestone_t *const consistent_milestone) {
  retcode_t ret = RC_OK;
  iota_milestone_t milestone;
  iota_milestone_t *milestone_ptr = &milestone;
  iota_stor_pack_t pack = {(void **)&milestone_ptr, 1, 0, false};

  rw_lock_handle_wrlock(&lv->mt->latest_snapshot->rw_lock);

  if ((ret = iota_tangle_milestone_load_first(lv->tangle, &pack)) != RC_OK) {
    goto done;
  }

  while (pack.num_loaded != 0) {
    pack.num_loaded = 0;
    if ((ret = iota_tangle_milestone_load_next(lv->tangle, milestone.index,
                                               &pack)) != RC_OK) {
      goto done;
    }
  }

done:
  rw_lock_handle_unlock(&lv->mt->latest_snapshot->rw_lock);
  return RC_OK;
}

retcode_t iota_consensus_ledger_validator_init(ledger_validator_t *const lv,
                                               tangle_t *const tangle,
                                               milestone_tracker_t *const mt) {
  retcode_t ret = RC_OK;
  iota_milestone_t consistent_milestone;

  logger_helper_init(LEDGER_VALIDATOR_LOGGER_ID, LOGGER_DEBUG, true);
  lv->tangle = tangle;
  lv->mt = mt;

  if ((ret = build_snapshot(lv, &consistent_milestone)) != RC_OK) {
    log_critical(LEDGER_VALIDATOR_LOGGER_ID, "Building snapshot failed\n");
    return ret;
  }

  return ret;
}

retcode_t iota_consensus_ledger_validator_destroy(
    ledger_validator_t *const lv) {
  logger_helper_destroy(LEDGER_VALIDATOR_LOGGER_ID);
  lv->tangle = NULL;
  lv->mt = NULL;
  return RC_OK;
}
