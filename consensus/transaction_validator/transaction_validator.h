/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/enggd
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __CONSENSUS_TRANSACTION_VALIDATOR_TRANSACTION_VALIDATOR_H__
#define __CONSENSUS_TRANSACTION_VALIDATOR_TRANSACTION_VALIDATOR_H__

#include <stdbool.h>

#include "common/errors.h"
#include "common/model/transaction.h"
#include "consensus/conf.h"
#include "consensus/snapshot/snapshots_provider.h"
#include "node/pipeline/transaction_requester.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Structure used to check transaction validity
 */
typedef struct transaction_validator_s {
  iota_consensus_conf_t *conf;
  snapshots_provider_t *snapshots_provider;
  transaction_requester_t *transaction_requester;
} transaction_validator_t;

/**
 * Initializes a transaction validator
 *
 * @param tv The transaction validator
 * @param conf A consensus configuration
 *
 * @return a status code
 */
retcode_t iota_consensus_transaction_validator_init(transaction_validator_t *const tv,
                                                    snapshots_provider_t *const snapshots_provider,
                                                    transaction_requester_t *const transaction_requester,
                                                    iota_consensus_conf_t *const conf);

/**
 * Destroys a transaction validator
 *
 * @param tv The transaction validator
 *
 * @return a status code
 */
retcode_t iota_consensus_transaction_validator_destroy(transaction_validator_t *const tv);

/**
 * Runs the following validation checks on a transaction:
 * - Sufficient weight
 * - Timestamps are above the last global snapshot and not too far in the future
 * - Values are lower than the maximum supply
 * - For value transactions, the address has a 0 as the last trit due to the
 * conversion between bytes and trits
 *
 * @param tv The transaction validator
 * @param transaction A transaction under test
 *
 * @return true if valid, false otherwise
 */
bool iota_consensus_transaction_validate(transaction_validator_t const *const tv,
                                         iota_transaction_t const *const transaction);

#ifdef __cplusplus
}
#endif

#endif  // __CONSENSUS_TRANSACTION_VALIDATOR_TRANSACTION_VALIDATOR_H__
