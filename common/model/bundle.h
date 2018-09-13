/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __COMMON_MODEL_BUNDLE_H__
#define __COMMON_MODEL_BUNDLE_H__

#include "common/trinary/flex_trit.h"

#ifdef __cplusplus
extern "C" {
#endif

void normalized_bundle(trit_t const *const bundle_hash,
                       byte_t *const normalized_bundle_hash);

void flex_normalized_bundle(flex_trit_t const *const bundle_hash,
                            byte_t *const normalized_bundle_hash);

#ifdef __cplusplus
}
#endif

#endif  //  __COMMON_MODEL_BUNDLE_H__
