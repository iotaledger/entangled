/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __COMMON_CONCENSUS_MILESTONE_MILESTONE_H__
#define __COMMON_CONCENSUS_MILESTONE_MILESTONE_H__

#include <stdbool.h>
#include <stdint.h>
#include "common/errors.h"
#include "common/model/transaction.h"
#include "consensus/snapshot/snapshot.h"
#include "consensus/view_models/milestone.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct milestone_s {
  trit_array_p coordinator;
  // iota_transaction_validator
  bool testnet;
  bool accept_any_testnet_coo;
  size_t num_of_keys;
  milestone_view_t latest_milestone_hash;
  milestone_view_t latest_solid_milestone_hash;
  uint64_t latest_milestone_index;
  uint64_t latest_solid_milestone_index;
  uint64_t milestone_start_index;
  snapshot_t latest_snapshot;

  /*
  private final MessageQ messageQ;
  private LedgerValidator ledgerValidator;
  private final Set<Hash> analyzedMilestoneCandidates = new HashSet<>();
   */
} milestone_t;

extern retcode_t iota_milestone_init();
extern retcode_t iota_milestone_validate();
extern retcode_t iota_milestone_update_latest_solid_subtangle_milestone();
extern retcode_t iota_milestone_get_index();
extern retcode_t iota_milestone_shutdown();
extern retcode_t iota_milestone_report_to_slack();

#ifdef __cplusplus
}
#endif

#endif  // __COMMON_CONCENSUS_MILESTONE_MILESTONE_H__
