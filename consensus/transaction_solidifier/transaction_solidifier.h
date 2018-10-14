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
#include "common/storage/connection.h"
#include "consensus/tangle/tangle.h"
#include "gossip/components/requester.h"
#include "utils/handles/thread.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct transaction_solidifier_t {
  requester_state_t *requester;
  thread_handle_t thread;
  bool running;
} transaction_solidifier_t;

extern void iota_consensus_transaction_solidifier_init(
    transaction_solidifier_t *const ts, requester_state_t *const requester);

extern retcode_t iota_consensus_transaction_solidifier_start(
    transaction_solidifier_t *const ts);

extern retcode_t iota_consensus_transaction_solidifier_stop(
    transaction_solidifier_t *const ts);

extern retcode_t iota_consensus_transaction_solidifier_destroy(
    transaction_solidifier_t *const ts);

// TODO - complete

#ifdef __cplusplus
}
#endif

#endif  // __CONSENSUS_TRANSACTION_SOLIDIFIER_TRANSACTION_SOLIDIFIER_H__
