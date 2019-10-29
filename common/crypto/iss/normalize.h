/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __COMMON_SIGN_NORMALIZE_H__
#define __COMMON_SIGN_NORMALIZE_H__

#include "common/trinary/flex_trit.h"

#ifdef __cplusplus
extern "C" {
#endif

bool normalized_hash_is_secure(byte_t const *const normalized_hash, size_t const length);
void normalize_hash(trit_t const *const hash, byte_t *const normalized_hash);
void normalize_hash_to_trits(trit_t const *const hash, trit_t *const normalized_hash);
void normalize_flex_hash(flex_trit_t const *const hash, byte_t *const normalized_hash);
void normalize_flex_hash_to_trits(flex_trit_t const *const hash, trit_t *const normalized_hash);

#ifdef __cplusplus
}
#endif

#endif  //  __COMMON_SIGN_NORMALIZE_H__
