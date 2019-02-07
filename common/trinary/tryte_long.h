/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __COMMON_TRINARY_TRYTE_LONG_H_
#define __COMMON_TRINARY_TRYTE_LONG_H_

#include "common/trinary/tryte.h"

#ifdef __cplusplus
extern "C" {
#endif

size_t min_trytes(int64_t const value);
int64_t trytes_to_long(tryte_t const *const trytes, size_t const length);
size_t long_to_trytes(int64_t const value, tryte_t *const trytes);

#ifdef __cplusplus
}
#endif

#endif  // __COMMON_TRINARY_TRYTE_LONG_H_
