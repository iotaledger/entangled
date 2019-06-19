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
#include "consensus/snapshot/snapshots_provider.h"
#include "consensus/tangle/tangle.h"
#include "node/pipeline/transaction_requester.h"
#include "node/tips_cache.h"
#include "utils/containers/hash/hash243_set.h"
#include "utils/containers/hash/hash243_stack.h"
#include "utils/handles/cond.h"
#include "utils/handles/lock.h"
#include "utils/handles/thread.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct transaction_solidifier_s {
  iota_consensus_conf_t *conf;
  transaction_requester_t *transaction_requester;
  snapshots_provider_t *snapshots_provider;
  thread_handle_t thread;
  bool running;
  lock_handle_t lock;
  hash243_set_t newly_set_solid_transactions;
  tips_cache_t *tips;
  cond_handle_t cond;
} transaction_solidifier_t;

retcode_t iota_consensus_transaction_solidifier_init(transaction_solidifier_t *const ts,
                                                     iota_consensus_conf_t *const conf,
                                                     transaction_requester_t *const transaction_requester,
                                                     snapshots_provider_t *const snapshots_provider,
                                                     tips_cache_t *const tips);

retcode_t iota_consensus_transaction_solidifier_start(transaction_solidifier_t *const ts);

retcode_t iota_consensus_transaction_solidifier_stop(transaction_solidifier_t *const ts);

retcode_t iota_consensus_transaction_solidifier_destroy(transaction_solidifier_t *const ts);

retcode_t iota_consensus_transaction_solidifier_check_solidity(transaction_solidifier_t *const ts,
                                                               tangle_t *const tangle, flex_trit_t *const hash,
                                                               bool is_milestone, bool *const is_solid);

retcode_t iota_consensus_transaction_solidifier_check_and_update_solid_state(transaction_solidifier_t *const ts,
                                                                             tangle_t *const tangle,
                                                                             flex_trit_t *const hash);

retcode_t iota_consensus_transaction_solidifier_update_status(transaction_solidifier_t *const ts,
                                                              tangle_t *const tangle, iota_transaction_t *const tx);

#ifdef __cplusplus
}
#endif

#endif  // __CONSENSUS_TRANSACTION_SOLIDIFIER_TRANSACTION_SOLIDIFIER_H__
