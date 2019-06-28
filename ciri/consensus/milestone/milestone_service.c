/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <inttypes.h>
#include <stdlib.h>

#include "ciri/consensus/milestone/milestone_service.h"
#include "common/model/milestone.h"
#include "utils/logger_helper.h"
#include "utils/macros.h"

#define MILESTONE_SERVICE_LOGGER_ID "milestone_service"

static logger_id_t logger_id;

retcode_t iota_milestone_service_init(milestone_service_t *const milestone_service, iota_consensus_conf_t *conf) {
  milestone_service->conf = conf;
  logger_id = logger_helper_enable(MILESTONE_SERVICE_LOGGER_ID, LOGGER_DEBUG, true);

  return RC_OK;
}

retcode_t iota_milestone_service_destroy(milestone_service_t *const milestone_service) {
  UNUSED(milestone_service);

  logger_helper_release(logger_id);

  return RC_OK;
}

retcode_t iota_milestone_service_replay_milestones(milestone_service_t const *const milestone_service,
                                                   tangle_t const *const tangle, snapshot_t *const snapshot,
                                                   uint64_t index) {
  retcode_t ret = RC_OK;
  state_delta_t merged_balance_changes = NULL;
  state_delta_t current_delta = NULL;
  iota_milestone_t *last_applied_milestone = NULL;
  DECLARE_PACK_SINGLE_MILESTONE(current_milestone, current_milestone_ptr, pack);
  UNUSED(milestone_service);

  for (uint64_t current_milestone_index = snapshot->metadata.index; current_milestone_index < index;
       ++current_milestone_index) {
    hash_pack_reset(&pack);
    ERR_BIND_GOTO(iota_tangle_milestone_load_by_index(tangle, current_milestone_index + 1, &pack), ret, cleanup);
    if (pack.num_loaded == 0) {
      log_warning(logger_id, "Target milestone was not loaded\n");
      break;
    } else {
      ERR_BIND_GOTO(iota_tangle_state_delta_load(tangle, current_milestone.index, &current_delta), ret, cleanup);
      if (current_delta != NULL) {
        ERR_BIND_GOTO(state_delta_apply_patch(&merged_balance_changes, &current_delta), ret, cleanup);
        last_applied_milestone = &current_milestone;
        state_delta_destroy(&current_delta);
        current_delta = NULL;
      }
    }
  }

  if (last_applied_milestone != NULL) {
    ERR_BIND_GOTO(iota_snapshot_apply_patch_no_lock(snapshot, &merged_balance_changes, last_applied_milestone->index),
                  ret, cleanup);
  }

cleanup:
  if (merged_balance_changes) {
    state_delta_destroy(&merged_balance_changes);
  }

  if (current_delta) {
    state_delta_destroy(&current_delta);
  }

  return ret;
}
