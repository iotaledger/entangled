/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __CONSENSUS_BUNDLE_VALIDATOR_BUNDLE_VALIDATOR_H__
#define __CONSENSUS_BUNDLE_VALIDATOR_BUNDLE_VALIDATOR_H__

#include "common/model/bundle.h"
#include "consensus/tangle/tangle.h"

#ifdef __cplusplus
extern "C" {
#endif

retcode_t bundle_validate(const tangle_t* const tangle, trit_array_p tail_hash,
                          bundle_transactions_t* bundle, bool* is_valid);


retcode_t bundle_validate_init();
retcode_t bundle_validate_destroy();

#ifdef __cplusplus
}
#endif

#endif  //__CONSENSUS_BUNDLE_VALIDATOR_BUNDLE_VALIDATOR_H__
