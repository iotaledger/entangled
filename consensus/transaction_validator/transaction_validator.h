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
  uint64_t snapshot_timestamp_ms;
  uint8_t mwm;
} transaction_validator_t;

extern retcode_t iota_consensus_transaction_validator_init(
    transaction_validator_t *const tv, uint64_t snapshot_timestamp_ms,
    uint8_t mwm);

extern retcode_t iota_consensus_transaction_validator_destroy(
    transaction_validator_t *const tv);

extern bool iota_consensus_transaction_validate(
    transaction_validator_t *const tv,
    const iota_transaction_t const transaction);

// TODO - complete

#ifdef __cplusplus
}
#endif

#endif  // __CONSENSUS_TRANSACTION_VALIDATOR_TRANSACTION_VALIDATOR_H__
