/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __COMMON_SIGN_NORMALIZE_H__
#define __COMMON_SIGN_NORMALIZE_H__

#define TRYTE_HASH_LENGTH 81
#define TRIT_HASH_LENGTH 243
#define MAX_TRYTE_VALUE 13
#define MIN_TRYTE_VALUE -13
#define TRYTE_WIDTH 3
#define NUMBER_OF_SECURITY_LEVELS 3
#define NORMALIZED_FRAGMENT_LENGTH 27

#include "common/trinary/flex_trit.h"

#ifdef __cplusplus
extern "C" {
#endif

extern void normalize_hash(flex_trit_t const *const hash,
                           byte_t *const normalized_hash);

#ifdef __cplusplus
}
#endif

#endif  //  __COMMON_SIGN_NORMALIZE_H__
