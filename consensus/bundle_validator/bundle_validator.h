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

retcode_t iota_consensus_bundle_validator_init();
retcode_t iota_consensus_bundle_validator_destroy();
retcode_t iota_consensus_bundle_validator_validate(
    tangle_t const* const tangle, trit_array_p const tail_hash,
    bundle_transactions_t* const bundle, bool* const is_valid);

#ifdef __cplusplus
}
#endif

#endif  // __CONSENSUS_BUNDLE_VALIDATOR_BUNDLE_VALIDATOR_H__
