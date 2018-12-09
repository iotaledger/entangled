/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __CONSENSUS_CONSENSUS_H__
#define __CONSENSUS_CONSENSUS_H__

#include "common/errors.h"
#include "consensus/bundle_validator/bundle_validator.h"
#include "consensus/conf.h"
#include "consensus/cw_rating_calculator/cw_rating_calculator.h"
#include "consensus/entry_point_selector/entry_point_selector.h"
#include "consensus/exit_probability_randomizer/exit_probability_randomizer.h"
#include "consensus/exit_probability_validator/exit_probability_validator.h"
#include "consensus/ledger_validator/ledger_validator.h"
#include "consensus/milestone_tracker/milestone_tracker.h"
#include "consensus/snapshot/snapshot.h"
#include "consensus/tangle/tangle.h"
#include "consensus/tip_selector/tip_selector.h"
#include "consensus/transaction_solidifier/transaction_solidifier.h"
#include "consensus/transaction_validator/transaction_validator.h"

typedef struct iota_consensus_s {
  iota_consensus_conf_t conf;
  cw_rating_calculator_t cw_rating_calculator;
  entry_point_selector_t entry_point_selector;
  ep_randomizer_t ep_randomizer;
  exit_prob_transaction_validator_t exit_prob_transaction_validator;
  ledger_validator_t ledger_validator;
  milestone_tracker_t milestone_tracker;
  snapshot_t snapshot;
  tangle_t tangle;
  tip_selector_t tip_selector;
  transaction_validator_t transaction_validator;
  transaction_solidifier_t transaction_solidifier;
} iota_consensus_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initializes all consensus components
 *
 * @param consensus The consensus
 * @param db_conf Database configuration
 * @param transaction_requester A transaction requester
 * @param tips A tips cache
 *
 * @return a status code
 */
retcode_t iota_consensus_init(iota_consensus_t* const consensus,
                              connection_config_t const* const db_conf,
                              requester_state_t* const transaction_requester,
                              tips_cache_t* const tips);

/**
 * Starts all consensus components
 *
 * @param consensus The consensus
 *
 * @return a status code
 */
retcode_t iota_consensus_start(iota_consensus_t* const consensus);

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
