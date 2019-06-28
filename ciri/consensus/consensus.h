/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __CONSENSUS_CONSENSUS_H__
#define __CONSENSUS_CONSENSUS_H__

#include "ciri/consensus/bundle_validator/bundle_validator.h"
#include "ciri/consensus/conf.h"
#include "ciri/consensus/ledger_validator/ledger_validator.h"
#include "ciri/consensus/milestone/milestone_service.h"
#include "ciri/consensus/milestone/milestone_tracker.h"
#include "ciri/consensus/snapshot/local_snapshots/local_snapshots_manager.h"
#include "ciri/consensus/snapshot/snapshot.h"
#include "ciri/consensus/spent_addresses/spent_addresses_service.h"
#include "ciri/consensus/tip_selection/cw_rating_calculator/cw_rating_calculator.h"
#include "ciri/consensus/tip_selection/entry_point_selector/entry_point_selector.h"
#include "ciri/consensus/tip_selection/exit_probability_randomizer/exit_probability_randomizer.h"
#include "ciri/consensus/tip_selection/exit_probability_validator/exit_probability_validator.h"
#include "ciri/consensus/tip_selection/tip_selector.h"
#include "ciri/consensus/transaction_solidifier/transaction_solidifier.h"
#include "ciri/consensus/transaction_validator/transaction_validator.h"
#include "common/errors.h"

typedef struct iota_consensus_s {
  iota_consensus_conf_t conf;
  cw_rating_calculator_t cw_rating_calculator;
  entry_point_selector_t entry_point_selector;
  ep_randomizer_t ep_randomizer;
  ledger_validator_t ledger_validator;
  milestone_tracker_t milestone_tracker;
  tip_selector_t tip_selector;
  transaction_validator_t transaction_validator;
  transaction_solidifier_t transaction_solidifier;
  local_snapshots_manager_t local_snapshots_manager;
  snapshots_service_t snapshots_service;
  snapshots_provider_t snapshots_provider;
  spent_addresses_service_t spent_addresses_service;
  milestone_service_t milestone_service;
} iota_consensus_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initializes all consensus components
 *
 * @param consensus The consensus
 * @param tangle A tangle
 * @param transaction_requester A transaction requester
 * @param tips A tips cache
 *
 * @return a status code
 */
retcode_t iota_consensus_init(iota_consensus_t* const consensus, tangle_t* const tangle,
                              transaction_requester_t* const transaction_requester, tips_cache_t* const tips);

/**
 * Starts all consensus components
 *
 * @param consensus The consensus
 * @param tangle A tangle
 *
 * @return a status code
 */
retcode_t iota_consensus_start(iota_consensus_t* const consensus, tangle_t* const tangle);

/**
 * Stops all consensus components
 *
 * @param consensus The consensus
 *
 * @return a status code
 */
retcode_t iota_consensus_stop(iota_consensus_t* const consensus);

/**
 * Destroys all consensus components
 *
 * @param consensus The consensus
 *
 * @return a status code
 */
retcode_t iota_consensus_destroy(iota_consensus_t* const consensus);

#ifdef __cplusplus
}
#endif

#endif  // __CONSENSUS_CONSENSUS_H__
