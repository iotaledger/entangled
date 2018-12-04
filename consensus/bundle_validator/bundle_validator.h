/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __CONSENSUS_BUNDLE_VALIDATOR_BUNDLE_VALIDATOR_H__
#define __CONSENSUS_BUNDLE_VALIDATOR_BUNDLE_VALIDATOR_H__

#include "common/errors.h"
#include "common/model/bundle.h"
#include "common/trinary/trit_array.h"
#include "consensus/tangle/tangle.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum bundle_status_e {
  BUNDLE_VALID,
  BUNDLE_NOT_INITIALIZED,
  BUNDLE_TAIL_NOT_FOUND,
  BUNDLE_INCOMPLETE,
  BUNDLE_INVALID_TX,
  BUNDLE_INVALID_INPUT_ADDRESS,
  BUNDLE_INVALID_VALUE,
  BUNDLE_INVALID_HASH,
  BUNDLE_INVALID_SIGNATURE
} bundle_status_t;

retcode_t iota_consensus_bundle_validator_init();
retcode_t iota_consensus_bundle_validator_destroy();
retcode_t iota_consensus_bundle_validator_validate(
    tangle_t const* const tangle, trit_array_p const tail_hash,
    bundle_transactions_t* const bundle, bundle_status_t* const status);

#ifdef __cplusplus
}
#endif

#endif  // __CONSENSUS_BUNDLE_VALIDATOR_BUNDLE_VALIDATOR_H__
