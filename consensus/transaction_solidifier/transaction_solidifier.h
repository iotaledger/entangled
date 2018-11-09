/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __CONSENSUS_TRANSACTION_SOLIDIFIER_TRANSACTION_SOLIDIFIER_H__
#define __CONSENSUS_TRANSACTION_SOLIDIFIER_TRANSACTION_SOLIDIFIER_H__

#include <stdbool.h>
#include <stdint.h>

#include "common/errors.h"
#include "common/model/transaction.h"
#include "common/storage/connection.h"
#include "consensus/conf.h"
#include "consensus/tangle/tangle.h"
#include "gossip/components/transaction_requester.h"
#include "gossip/tips_cache.h"
#include "utils/containers/hash/hash243_set.h"
#include "utils/containers/hash/hash243_stack.h"
#include "utils/handles/lock.h"
#include "utils/handles/thread.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct transaction_solidifier_s {
  iota_consensus_conf_t *conf;
  tangle_t *tangle;
  requester_state_t *requester;
  thread_handle_t thread;
  bool running;
  lock_handle_t lock;
  hash243_set_t newly_set_solid_transactions;
  hash243_set_t solid_transactions_candidates;
  tips_cache_t *tips;
} transaction_solidifier_t;

retcode_t iota_consensus_transaction_solidifier_init(
    transaction_solidifier_t *const ts, iota_consensus_conf_t *const conf,
    tangle_t *const tangle, requester_state_t *const requester,
    tips_cache_t *const tips);

retcode_t iota_consensus_transaction_solidifier_start(
    transaction_solidifier_t *const ts);

retcode_t iota_consensus_transaction_solidifier_stop(
    transaction_solidifier_t *const ts);

retcode_t iota_consensus_transaction_solidifier_destroy(
    transaction_solidifier_t *const ts);

retcode_t iota_consensus_transaction_solidifier_check_solidity(
    transaction_solidifier_t *const ts, flex_trit_t *const hash,
    bool is_milestone, bool *const is_solid);

retcode_t iota_consensus_transaction_solidifier_check_and_update_solid_state(
    transaction_solidifier_t *const ts, flex_trit_t *const hash);

retcode_t iota_consensus_transaction_solidifier_update_status(
    transaction_solidifier_t *const ts, iota_transaction_t const tx);

#ifdef __cplusplus
}
#endif

#endif  // __CONSENSUS_TRANSACTION_SOLIDIFIER_TRANSACTION_SOLIDIFIER_H__
