/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "consensus/milestone_service/milestone_service.h"
#include "common/model/milestone.h"
#include "utils/logger_helper.h"

#define MILESTONE_SERVICE_LOGGER_ID "milestone_service"

static logger_id_t logger_id;

retcode_t iota_milestone_service_init(milestone_service_t *const milestone_service, iota_consensus_conf_t *conf) {
  retcode_t ret;
  milestone_service->conf = conf;
  logger_id = logger_helper_enable(MILESTONE_SERVICE_LOGGER_ID, LOGGER_DEBUG, true);

  connection_config_t db_conf = {.db_path = milestone_service->conf->db_path};

  if (strcmp(db_conf.db_path, "") != 0) {
    ERR_BIND_RETURN(iota_tangle_init(&milestone_service->tangle, &db_conf), ret);
  }

  return RC_OK;
}

retcode_t iota_milestone_service_destroy(milestone_service_t *const milestone_service) {
  if (iota_tangle_destroy(&milestone_service->tangle) != RC_OK) {
    log_critical(logger_id, "Destroying tangle connection failed\n");
  }

  logger_helper_release(logger_id);
  return RC_OK;
}

retcode_t iota_milestone_service_replay_milestones(milestone_service_t *const milestone_service,
                                                   snapshot_t *const snapshot, uint64_t index) {
  retcode_t ret;
  state_delta_t merged_balance_changes;
  state_delta_t current_delta;
  iota_milestone_t *last_applied_milestone = NULL;
  DECLARE_PACK_SINGLE_MILESTONE(current_milestone, current_milestone_ptr, pack);

  for (uint64_t current_milestone_index = snapshot->index + 1ULL; current_milestone_index <= index;
       ++current_milestone_index) {
    ERR_BIND_RETURN(
        iota_tangle_milestone_load_next(&milestone_service->tangle, current_milestone_index, current_milestone_index),
        ret);
    if (pack.num_loaded == 0) {
      log_error(logger_id, "Target milestone was not loaded\n");
      // TODO - add to skipped milestones
    } else {
      ERR_BIND_RETURN(iota_tangle_state_delta_load(&milestone_service->tangle, &current_milestone, &current_delta),
                      ret);
      ERR_BIND_RETURN(state_delta_apply_patch(merged_balance_changes, current_delta), ret);
      last_applied_milestone = &current_milestone;
    }
  }

  if (last_applied_milestone != NULL) {
    // TODO - mutex is not need to be taken inside "iota_snapshot_apply_patch" this time
    ERR_BIND_RETURN(iota_snapshot_apply_patch(snapshot, merged_balance_changes, last_applied_milestone->index), ret);
  }
  return RC_OK;
}