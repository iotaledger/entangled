/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __CONSENSUS_TRANSACTION_VALIDATOR_TRANSACTION_VALIDATOR_H__
#define __CONSENSUS_TRANSACTION_VALIDATOR_TRANSACTION_VALIDATOR_H__

#include <stdbool.h>
#include <stdint.h>

#include "common/errors.h"
#include "common/storage/connection.h"
#include "consensus/tangle/tangle.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct transaction_validator_t {
  tangle_t *tangle;
  // TODO - lots
} transaction_validator_t;

extern retcode_t iota_consensus_transaction_validator_init(
    tangle_t *const tangle, transaction_validator_t *const tv);

// TODO - complete

#ifdef __cplusplus
}
#endif

#endif  // __CONSENSUS_TRANSACTION_VALIDATOR_TRANSACTION_VALIDATOR_H__
